#include "qpcpp.hpp"

#include "etl/memory.h"

#include "bsp.h"

#include "blinkLED/blinkLED.h"
#include "pushButton/pushButton.h"
#include "stm_app.h"

volatile uint32_t sysTicks = 0;

enum class STMAPP_SIGNALS : QP::QSignal
{
	NULL_SIGNALS = stmapp::APP_START_SIGNAL,
	LED_ON_SIGNAL,
	LED_OFF_SIGNAL,
};

/* Setting structs */
constexpr auto ledPin1 = stmapp::BlinkLED::constructLEDPin(GPIO_INSTANCE::GPIO_C, PIN::PIN_13);
constexpr auto blinkLed1Struct = stmapp::BlinkLED::constructVariableBlinkingLEDStruct(500, 1000);

constexpr auto ledPin2 = stmapp::BlinkLED::constructLEDPin(GPIO_INSTANCE::GPIO_B, PIN::PIN_12);
constexpr auto blinkLed2Struct = stmapp::BlinkLED::constructLEDStruct(true,
																	  STMAPP_SIGNALS::LED_ON_SIGNAL,
																	  STMAPP_SIGNALS::LED_OFF_SIGNAL);

const auto buttonReleasedEvent = QP::QEvt{
	stmapp::castToQPSignal(STMAPP_SIGNALS::LED_ON_SIGNAL), 0, 0};
constexpr auto buttonStruct = stmapp::PushButton::constructButtonWithReleasedEventOnly(25, &buttonReleasedEvent);
constexpr auto buttonPin1 = stmapp::PushButton::constructButtonInstance(GPIO_INSTANCE::GPIO_A, PIN::PIN_4);
/* Setting structs */

enum AO_PRIORITY : uint8_t
{
	BLINK_LED_WITH_TIMER = 1U,
	STATIC_LED,
	PUSH_BUTTON,
};

int main(void)
{

	/* Active objects from stmAPP */
	stmapp::BlinkLED LED1(&ledPin1, &blinkLed1Struct);
	QP::QActive *const Blink1 = &LED1;
	stmapp::BlinkLED LED2(&ledPin2, &blinkLed2Struct);
	QP::QActive *const Blink2 = &LED2;
	stmapp::PushButton Button1(&buttonPin1, &buttonStruct);
	QP::QActive *const PushButton1 = &Button1;
	/* Active objects from stmAPP */

	BSP::initBoard();

	/* Active object event queues */
	static QP::QEvt const *led1QueueStore[10];
	static QP::QEvt const *led2QueueStore[10];
	static QP::QEvt const *button1QueueStore[10];
	/* Active object event queues */

	/* Subscriber list array */
	static QP::QSubscrList subscribeTo[15];
	/* Subscriber list array */

	/** Event pool array definition */
	// static QF_MPOOL_EL(stmAPP::BlinkLED) smlPoolStore[20];
	static QF_MPOOL_EL(stmapp::PushButton) buttonPoolStore[10];
	/** Event pool array initializations */

	/* initialize the framework and the Vanilla scheduler 'qv' */
	QP::QF::init();

	/* initialize the publish subscribe facility */
	QP::QF::psInit(subscribeTo, Q_DIM(subscribeTo));
	/* initialize the publish subscribe facility */

	/* Intialize event pool arrays*/
	// QP::QF::poolInit(smlPoolStore, sizeof(smlPoolStore),
	// sizeof(smlPoolStore[0]));
	QP::QF::poolInit(buttonPoolStore,
					 sizeof(buttonPoolStore),
					 sizeof(buttonPoolStore[0]));
	/* Intialize event pool arrays*/

	/* board initialization such as Flash buffer, clock setup, SysTick_IRQN setup */
	BSP::initBoard(); /*todo: Investigate the init board issue with the new setup */

	/* Active objects start method */
	Blink1->start(AO_PRIORITY::BLINK_LED_WITH_TIMER, &led1QueueStore[0], Q_DIM(led1QueueStore), (void *)0, 0U);
	Blink2->start(AO_PRIORITY::STATIC_LED, &led2QueueStore[0], Q_DIM(led2QueueStore), (void *)0, 0U);
	PushButton1->start(AO_PRIORITY::PUSH_BUTTON,
					   &button1QueueStore[0],
					   Q_DIM(button1QueueStore),
					   (void *)0,
					   0U);
	/* Active objects start method */

	return QP::QF::run();
}
