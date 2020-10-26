#ifndef CINTC_H_
#define CINTC_H_

#include <avr32/io.h>
#include <avr32/intc_101.h>
#include "compiler.h"
#include "intc.h"

#if __GNUC__
#define    __isr__        __attribute__((__interrupt__))
#elif __ICCAVR32__
#define    __isr__        __interrupt
#endif



#define INT0          0 //!< Lowest interrupt priority level.
#define INT1          1
#define INT2          2
#define INT3          3 //!< Highest interrupt priority level.

#define RTC_INT_LEVEL				INT3
#define TIMER_COUNTER_INT_LEVEL		INT2
#define USART_INT_LEVEL     		INT1
#define GPIO_INT_LEVEL				INT1
#define ADC_INT_LEVEL				INT0

//! Maximal number of interrupt request lines per group.
#define AVR32_INTC_MAX_NUM_IRQS_PER_GRP             32

//! Number of interrupt priority levels.
//#define AVR32_INTC_NUM_INT_LEVELS                   (1 << AVR32_INTC_IPR0_INTLEV_SIZE)



#ifdef __AVR32_ABI_COMPILER__
// Automatically defined when compiling for AVR32, not when assembling.


#endif
// __AVR32_ABI_COMPILER__

#endif /*CINTC_H_*/
