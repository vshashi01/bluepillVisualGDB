#include "bsp.h"
#include "stmCortexFunction/stmCortexFunction.h"
#include "stmFlash/stmFlash.h"
#include "stmClock/stmClock.h"
#include "qpcpp.hpp"
#include "core_cm3.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
// DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
//
enum KernelUnawareISRs {
	// see NOTE00
   // ...
   MAX_KERNEL_UNAWARE_CMSIS_PRI  // keep always last
}
;
// "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts
Q_ASSERT_COMPILE (MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
	SYSTICK_PRIO = QF_AWARE_ISR_CMSIS_PRI, 
	// see NOTE00
   // ...
   MAX_KERNEL_AWARE_CMSIS_PRI // keep always last
};
// "kernel-aware" interrupts should not overlap the PendSV priority
Q_ASSERT_COMPILE (MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >> (8 - __NVIC_PRIO_BITS)));

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
	setNVICPriorityGroup (NVIC_PRIORITY_GROUP_4);

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