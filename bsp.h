#ifndef _BSP_H
#define _BSP_H


#include "stmClock/stmClock.h"

static SysClock systemClock (SYSCLK_SOURCE::SYSCLK_SOURCE_PLL,
	AHB_DIVIDER::AHB_SYSCLK_Div_NO,
	APB1_DIVIDER::APB1_HCLK_Div_2, 
	APB2_DIVIDER::APB2_HCLK_Div_NO,
	FlashLatencyWait::FLASH_LATENCY_WAIT_2);

SysClock* const clock = &systemClock;



namespace BSP {

	void initBoard ();
	void initClock ();
}



#endif