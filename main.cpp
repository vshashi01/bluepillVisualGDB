#include "blinkLED/blinkLED.h"
#include "bsp.h"
#include "pushButton/pushButton.h"
#include "qpcpp.hpp"
#include "stm32f1xx_it.h"
#include "stmClock/stmClock.h"
#include "stmPin/stmPin.h"

#include <SysprogsProfiler.h>

volatile uint32_t sysTicks = 0;

/* Active objects from stmAPP */
stmAPP::BlinkLED LED1(GPIO_INSTANCE::GPIO_C, PIN::PIN_13, 500, 500, true);
// stmAPP::BlinkLED LED1(GPIOB, PIN::PIN_12, true);
QP::QActive *const Blink1 = &LED1;

//stmAPP::BlinkLED LED2(GPIO_INSTANCE::GPIO_C, PIN::PIN_13, true);
stmAPP::BlinkLED LED2(GPIO_INSTANCE::GPIO_B, PIN::PIN_12, true);
QP::QActive *const Blink2 = &LED2;

stmAPP::PushButton Button1(GPIO_INSTANCE::GPIO_A, PIN::PIN_5, 20);
QP::QActive *const PushButton1 = &Button1;
/* Active objects from stmAPP */

enum AO_PRIORITY : uint8_t
{
	BLINK_LED_WITH_TIMER = 1U,
	STATIC_LED,
	PUSH_BUTTON,
};

int main(void)
{
	//InitializeSamplingProfiler ();
	BSP::initBoard();
	//BSP::initClock();

	/** User defined Event structs */
	static stmAPP::BlinkLEDEvent highEvent;
	highEvent.sig = stmAPP::ON_LED_SIG;
	highEvent.poolId_ = 0;
	highEvent.refCtr_ = 0;
	highEvent.priority = AO_PRIORITY::BLINK_LED_WITH_TIMER;

	static stmAPP::BlinkLEDEvent lowEvent;
	lowEvent.sig = stmAPP::OFF_LED_SIG;
	lowEvent.poolId_ = 0;
	lowEvent.refCtr_ = 0;
	lowEvent.priority = AO_PRIORITY::BLINK_LED_WITH_TIMER;

	static stmAPP::BlinkLEDEvent restartTimerEvent;
	restartTimerEvent.sig = stmAPP::RESTART_TIMER_SIG;
	restartTimerEvent.poolId_ = 0;
	restartTimerEvent.refCtr_ = 0;
	restartTimerEvent.priority = AO_PRIORITY::BLINK_LED_WITH_TIMER;
	/** User defined Event structs */

	//Button1.setReleasedEvent(&lowEvent);
	Button1.setReleasedEvent(&restartTimerEvent);
	Button1.setPressedEvent(&lowEvent);

	/* Active object event queues */
	static QP::QEvt const *led1QueueStore[10];
	static QP::QEvt const *led2QueueStore[10];
	static QP::QEvt const *button1QueueStore[10];
	/* Active object event queues */

	/* Subscriber list array */
	static QP::QSubscrList subscribeTo[20];
	/* Subscriber list array */

	/** Event pool array definition */
	// static QF_MPOOL_EL(stmAPP::BlinkLED) smlPoolStore[20];
	static QF_MPOOL_EL(stmAPP::PushButton) buttonPoolStore[15];
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
