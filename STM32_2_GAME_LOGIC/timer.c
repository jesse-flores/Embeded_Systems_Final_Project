#include "ee14lib.h"
//#include <stdio.h>

#define CLK 4000000 //System clock frequency 

EE14Lib_Err timer_config_pwm(TIM_TypeDef* const timer, const unsigned int freq_hz)
{
    // Enable the clock for the timer
    if(timer == TIM1){
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    }
    else if(timer == TIM2){
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    }
    else if(timer == TIM15){
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    }
    else if(timer == TIM16){
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    }
    else{
        return EE14Lib_Err_NOT_IMPLEMENTED;
    }

    // Set the prescaler
    timer->PSC = CLK/(freq_hz * 65536);
    if (timer->PSC > 0) timer->PSC -= 1;
    
    // Set the reload value
    timer->ARR = CLK/(freq_hz * (timer->PSC + 1))-1;

    // Set the main output enable
    timer->BDTR |= TIM_BDTR_MOE;

    // And enable the timer itself
    timer->CR1 |= TIM_CR1_CEN;

    return EE14Lib_Err_OK;
}


// Mapping of GPIO pin to timer channel
// Lowest bit is whether this is an inverted channel (N),
// remaining bits are zero-indexed (0-3 for TIMx_1-TIMx_4)
//   1  -> 0
//   1N -> 1
//   2  -> 2
//   2N -> 3
//   3  -> 4
//   3N -> 5
//   4  -> 6
// -1 for pins which are not PWM-capable / not on this timer.
int g_Timer1Channel[D13+1] = {
  -1,-1,-1,-1,  // A0=PA0,A1=PA1,A2=PA3,A3=PA4
  -1,-1, 1,-1,  // A4=PA5,A5=PA6,A6=PA7,A7=PA2
   4, 2,-1, 3,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
  -1,-1, 5,-1,  // D4=PB7,D5=PB6,D6=PB1,D7=PC14
  -1, 0, 6,-1,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
  -1,-1         // D12=PB4,D13=PB3.
};

int g_Timer2Channel[D13+1] = {
   0, 2, 6,-1,  // A0=PA0,A1=PA1,A2=PA3,A3=PA4
   0,-1,-1, 4,  // A4=PA5,A5=PA6,A6=PA7,A7=PA2
  -1,-1,-1,-1,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
  -1,-1,-1,-1,  // D4=PB7,D5=PB6,D6=PB1,D7=PC14
  -1,-1,-1,-1,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
  -1, 2         // D12=PB4,D13=PB3.
};

int g_Timer15Channel[D13+1] = {
  -1, 1, 2,-1,  // A0=PA0,A1=PA1,A2=PA3,A3=PA4
  -1,-1,-1, 0,  // A4=PA5,A5=PA6,A6=PA7,A7=PA2
  -1,-1,-1,-1,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
  -1,-1,-1,-1,  // D4=PB7,D5=PB6,D6=PB1,D7=PC14
  -1,-1,-1,-1,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
  -1,-1         // D12=PB4,D13=PB3.
};

int g_Timer16Channel[D13+1] = {
  -1,-1,-1,-1,  // A0=PA0,A1=PA1,A2=PA3,A3=PA4
  -1, 0,-1,-1,  // A4=PA5,A5=PA6,A6=PA7,A7=PA2
  -1,-1,-1,-1,  // D0=PA10,D1=PA9,D2=PA12,D3=PB0
  -1, 1,-1,-1,  // D4=PB7,D5=PB6,D6=PB1,D7=PC14
  -1,-1,-1,-1,  // D8=PC15,D9=PA8,D10=PA11,D11=PB5
  -1,-1         // D12=PB4,D13=PB3.
};

EE14Lib_Err timer_config_channel_pwm(TIM_TypeDef* const timer, const EE14Lib_Pin pin, const unsigned int duty)
{
    int channel = -1;
    if(timer == TIM1){
        channel = g_Timer1Channel[pin];
    } else if(timer  == TIM2){
        channel = g_Timer2Channel[pin];
    } else if(timer == TIM15){
        channel = g_Timer15Channel[pin];
    } else if(timer == TIM16){
        channel = g_Timer16Channel[pin];
    }

    if(channel < 0){
        return EE14Lib_ERR_INVALID_CONFIG;
    }

    int channel_idx = channel >> 1; // Lowest bit is N

    *((unsigned int*)timer + 13 + channel_idx) = (duty * (timer->ARR+1))/1023;

    // Enable PWM mode, and set preload enable (only update counter on rollover)
    if(channel_idx == 0){
        timer->CCMR1 &= ~(TIM_CCMR1_OC1M);
        timer->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
        timer->CCMR1 |= TIM_CCMR1_OC1PE;
    } else if(channel_idx == 1){
        timer->CCMR1 &= ~(TIM_CCMR1_OC2M);
        timer->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
        timer->CCMR1 |= TIM_CCMR1_OC2PE;
    } else if(channel_idx == 2){
        timer->CCMR2 &= ~(TIM_CCMR2_OC3M);
        timer->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);
        timer->CCMR2 |= TIM_CCMR2_OC3PE;
    } else { // Must be 3
        timer->CCMR2 &= ~(TIM_CCMR2_OC4M);
        timer->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
        timer->CCMR2 |= TIM_CCMR2_OC4PE;
    }

    // Enable the capture/compare output
    timer->CCER |= 1 << (2*channel); // Primary enables are 0, 4, 8, 12; inverted are 2, 6

    if(timer == TIM1 || timer == TIM2){
        gpio_config_alternate_function(pin, 1); // AFR = 1 is timer mode for timers 1 & 2
    } else {
        gpio_config_alternate_function(pin, 14); // AFR = 14 for timers 15 & 16
    }

    return EE14Lib_Err_OK;
}