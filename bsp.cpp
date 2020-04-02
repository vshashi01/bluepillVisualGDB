#include "bsp.h"
#include "stmCortexFunction/stmCortexFunction.h"
#include "stmFlash/stmFlash.h"
#include "stmClock/stmClock.h"
#include "qpcpp.hpp"
#include "core_cm3.h"
#include "stmTimer/stm_timer.h"
#include "stmEXTI/stm_exti.h"

extern const stmcpp::TimeBaseInterruptGenerator *ultrasonic_trigger_timer_ptr;
//extern const stmcpp::ExternalInterrupt *echo_interrupt_ptr;
extern const stmcpp::CollectionExternalInterrupt<EXTI_LINE_5_9> *echo_interrupt_ptr;
extern const stmcpp::DigitalOut *ultrasonic_trigger_pin_ptr;

void BSP::initBoard () {
    
	enablePrefetchBuffer ();              //enable prefetch buffer

	if (!isEnabledPrefetchBuffer ()) return;              //if not enabled exit 

	SystemCoreClockUpdate ();

	//setNVICPriorityGroup(NVIC_PRIORITY_GROUP_DEFAULT); //set the Priority Group to be used
	
	/*setNVICPriorityGroup (NVIC_PRIORITY_GROUP_4);

	InitTick (SystemCoreClock, 1000);

	systemClock.setPLL (PLL_SOURCE_HSE, PLL_HSE_DIV_NO, PLLMUL_9);

	systemClock.setup ();    

	setNVICPriority (SysTick_IRQn, PRIORITY_NUM_0, PRIORITY_NUM_0);

	delay (100);*/
    
}

void BSP::initClock ()
{
	stmcortexfunction::setNVICPriorityGroup (NVIC_PRIORITY_GROUP_4);

	InitTick (SystemCoreClock, 1000);

	systemClock.setPLL (PLL_SOURCE_HSE, PLL_HSE_DIV_NO, PLLMUL_9);

	systemClock.setup (1000, SYSTICK_PRIO);    

	//setNVICPriority (SysTick_IRQn, PRIORITY_NUM_0, PRIORITY_NUM_0);
	//NVIC_SetPriority (SysTick_IRQn, KernelAwareISRs::SYSTICK_PRIO);

	delay (100);
	
	//NVIC_EnableIRQ (SysTick_IRQn);
}

namespace QP {
	void QF::onStartup (void) 
	{  
		BSP::initClock ();
		ultrasonic_trigger_pin_ptr->setup();
		ultrasonic_trigger_timer_ptr->setup();
		ultrasonic_trigger_timer_ptr->enable();

		echo_interrupt_ptr->setup();
		echo_interrupt_ptr->enable();
	} 

	void QF::onCleanup () 
	{

	}

	void QV::onIdle () 
	{

		QF_INT_ENABLE ();

	}

	extern "C" void Q_onAssert (char const *module, int loc) 
	{
		//
		// NOTE: add here your application-specific error handling
		//
		(void)module;
		(void)loc;
		QS_ASSERTION (module, loc, static_cast<uint32_t>(10000U));
		//NVIC_SystemReset ();
	}
    
}