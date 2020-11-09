//
// Created by mkotyk on 11/8/20.
//

#include "PolledSoftSerialHandler.h"

#include <lpc17xx_rit.h>
#include <lpc17xx_clkpwr.h>
#include <debug_frmwrk.h>
#include "libs/Kernel.h"
#include "SerialPinConfig.h"

const int OVERSAMPLE = 3;
const int INTERRUPT_PRIORITY = 0;

PolledSoftSerialHandler::PolledSoftSerialHandler() : baud(0) {
    RIT_Init(LPC_RIT);
    NVIC_SetPriority(RIT_IRQn, NVIC_EncodePriority(0, INTERRUPT_PRIORITY, 0));
}

void PolledSoftSerialHandler::setBaud(int baud) {
    if (baud != this->baud) {
        NVIC_DisableIRQ(RIT_IRQn);
        if (baud > 0) {
            uint32_t clock_rate, cmp_value;
            // Get PCLK value of RIT
            clock_rate = CLKPWR_GetPCLK(CLKPWR_PCLKSEL_RIT);
            cmp_value = clock_rate / (baud * OVERSAMPLE);
            LPC_RIT->RICOMPVAL = cmp_value;
            LPC_RIT->RICOUNTER = 0x00000000;
            /* Set timer enable clear bit to clear timer to 0 whenever
            * counter value equals the contents of RICOMPVAL
            */
            LPC_RIT->RICTRL |= (1 << 1);
            NVIC_EnableIRQ(RIT_IRQn);
        }
        this->baud = baud;
    }
}

[[gnu::always_inline, gnu::optimize("O3")]] inline void PolledSoftSerialHandler::handle_interrupt() {
    tickCount++;
    for (auto pc:pin_configs) pc->tick(tickCount % OVERSAMPLE);
}

extern "C"
[[gnu::optimize("O3")]] void RIT_IRQHandler(void) {
    LPC_RIT->RICTRL |= 1;
    PolledSoftSerialHandler::instance().handle_interrupt();
}
