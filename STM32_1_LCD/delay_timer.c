#include "ee14lib.h" //for timer peripheral

#define timer TIM1


void delay_timer_config() {
        //enable clock for timer 
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

        timer->PSC = 3;
        timer->ARR = 0xFFFF;


        timer->CR1 |= TIM_CR1_CEN;

}


void delay_us(uint32_t us) {
        uint32_t start = timer->CNT;

        while((timer->CNT - start) < us);
}

void delay_ms(uint32_t ms) {

        for (uint32_t i = 0; i < ms; i++) {
                delay_us(1000);
        }
}
