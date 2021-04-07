#include "audio.h"

#include "bluepill.h"

#include <stdint.h>
#include <stdbool.h>

//Functions

//TODO add sweeping functionality and random (noise) mode

void Audio_init()
{
    //AFIO_MAPR = 0b01000000;//TESTING
    
    //TODO initialize pwm and systick timer
    GPIOA_CRH = (GPIOA_CRH & 0xFFFFFFF0) | 0x0000000B;//Set PA8 as alternate function 50mhz output
    //GPIOA_BSRR = 1 << 8;//TESTING
    
    TIM1_CCMR1 = 0x0068;//Enable PWM mode on channel 1 (PA8) + preload enable on TIM1_CCR1
    
    TIM1_CR1 = 0x0080;//Enable preload of TIM1_ARR
    
    //TIM1_PSC = (72000000 / AUDIO_TIMER_FREQ) - 1;//Division is done at compile time
    //TIM1_ARR = 0x1;//Random default value of ARR (does not matter, just can't be 0)
    TIM1_PSC = 0;//TESTING
    TIM1_ARR = 0xFFFF;//TESTING
    TIM1_CCR1 = 0x7FFF;//TESTING
    TIM1_CCER = 0x0001;//TESTING
    
    TIM1_EGR |= 0x0001;//Generate update event to copy TIM1_PSC, TIM1_ARR, and TIM1_CCR4 into actual (shadow) registers
    TIM1_CR1 |= 0x0001;//Enable counter (upcounting)
    
    //Audio_setFreq0(1000);//TESTING
}

void Audio_reset()
{
    //TODO
}

void Audio_setFreq0(uint_fast16_t frequency)
{
    if (frequency != 0)
    {
        //TIM1_CR1 &= 1;//Disable counter
        
        //Frequency=TIMER_FREQ/ARR so ARR=TIMER_FREQ/Frequency
        uint_fast16_t newMaxValue = AUDIO_TIMER_FREQ / frequency;//Hardware division so not too slow
        TIM1_ARR = newMaxValue;
        TIM1_CCR1 = newMaxValue / 2;//50% duty cycle
        TIM1_CCER = 0x0001;//Enable channel 1 (PA8) output (non-inverted polarity)
        
        //TIM1_EGR |= 0x0001;//Generate update event to copy TIM1_ARR and TIM1_CCR4 into actual (shadow) registers
        //TIM1_CR1 |= 0x0001;//Enable counter (upcounting)
    }
    else
    {
        TIM1_CCER = 0x0000;//Disable channel 1 (PA8) output
        TIM1_CR1 &= 1;//Disable counter
    }
}

void Audio_setFreq1(uint_fast16_t frequency)
{
    //TODO
}

void Audio_setFreq2(uint_fast16_t frequency)
{
    //TODO
}
