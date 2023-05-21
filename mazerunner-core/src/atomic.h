/******************************************************************************
 * Project: mazerunner-core                                                   *
 * File:    atomic.h                                                          *
 * File Created: Thursday, 27th October 2022 11:00:18 pm                      *
 * Author: Peter Harrison                                                     *
 * -----                                                                      *
 * Last Modified: Thursday, 27th October 2022 11:04:40 pm                     *
 * -----                                                                      *
 * Copyright 2022 - 2022 Peter Harrison, Micromouseonline                     *
 * -----                                                                      *
 * Licence:                                                                   *
 *     Use of this source code is governed by an MIT-style                    *
 *     license that can be found in the LICENSE file or at                    *
 *     https://opensource.org/licenses/MIT.                                   *
 ******************************************************************************/

#if defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_AVR)
#include <util/atomic.h>
#endif
#ifdef ARDUINO_ARCH_NRF52840
#define ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#endif

#ifndef ATOMIC_BLOCK
// Processor: ARDUINO_ARCH_RP2040
// Board: ARDUINO_NANO_RP2040_CONNECT
#ifdef ARDUINO_ARCH_RP2040

// similar to https://arduino.stackexchange.com/questions/77494/which-arduinos-support-atomic-block/77579#77579
// also see https://github.com/raspberrypi/pico-sdk/blob/bfcbefafc5d2a210551a4d9d80b4303d4ae0adf7/src/rp2_common/hardware_sync/include/hardware/sync.h#L198=
//
// NOTE: This might only work on one core. The other core might not be affected by this
// disable  interrupt, and the PIO certainly won't be. But for our code this should be ok.
//
#define ATOMIC_BLOCK(type) for(type; type##_OBJECT_NAME.run(); \
    type##_OBJECT_NAME.stop())
#define ATOMIC_RESTORESTATE_OBJECT_NAME AtomicBlockRestoreState_
#define ATOMIC_RESTORESTATE AtomicBlockRestoreState ATOMIC_RESTORESTATE_OBJECT_NAME

class AtomicBlockRestoreState
{
private:
    bool running;
    uint32_t status;
public:
    inline AtomicBlockRestoreState():running(true) {
        __asm volatile ("mrs %0, PRIMASK" : "=r" (status)::);
        __asm volatile ("cpsid i");
    }
    inline ~AtomicBlockRestoreState() { asm volatile ("msr PRIMASK,%0"::"r" (status) : ); }
    inline bool run() { return running; }
    inline void stop() { running = false; }
};

#endif
#endif
