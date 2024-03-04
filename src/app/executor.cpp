/**
 * Executor is used to as a central business-logic module.
 * This helps to declutter main.cpp. In our case, it also helps to initialize
 * functions that aren't tied to any specific module (eg. main clock setup).
 */

#include <libopencm3/stm32/rcc.h>
#include "executor.h"

Executor::Executor(void)
{
    this->init();
}

/**
 * Executor::init() - Initialize all submodules
 *
 * Return: void
 */
void Executor::init(void)
{
    this->clock_setup();
    specAnalyzer = Spec();
}

/**
 * Executor::exec() - Run main for-loop
 *
 * Return: void
 */
void Executor::exec(void)
{
    specAnalyzer.calcFFT();
}

/**
 * Executor::clock_setup() - Init STM32 to 168 MHz
 *
 * Return: void
 */
void inline Executor::clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}
