#ifndef _BSP_H
#define _BSP_H

#include "qpcpp.h"
#include "stmClock/stmClock.h"

static SysClock systemClock(SYSCLK_SOURCE::SYSCLK_SOURCE_PLL,
							AHB_DIVIDER::AHB_SYSCLK_Div_NO,
							APB1_DIVIDER::APB1_HCLK_Div_2,
							APB2_DIVIDER::APB2_HCLK_Div_NO,
							FlashLatencyWait::FLASH_LATENCY_WAIT_2);

SysClock *const clock = &systemClock;

namespace BSP
{

	void initBoard();
	void initClock();
} // namespace BSP

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
// DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
//
enum KernelUnawareISRs
{
	// see NOTE00
	// ...
	MAX_KERNEL_UNAWARE_CMSIS_PRI // keep always last
};
// "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs
{
	SYSTICK_PRIO = QF_AWARE_ISR_CMSIS_PRI,
	TIM3_PRIO,
	ECHO_RECEIVE_PRIO,
	BT_PRIO,
	// see NOTE00
	// ...
	MAX_KERNEL_AWARE_CMSIS_PRI // keep always last
};
// "kernel-aware" interrupts should not overlap the PendSV priority
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >> (8 - __NVIC_PRIO_BITS)));

#endif