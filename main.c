#include "ee14lib.h"
#include "driver_ssd1309_basic.h"               

#define I2C_ADDR 0x3c
volatile uint32_t time_count = 0;

int _write(int file, char *data, int len) {
    serial_write(USART2, data, len);
    return len;
} 

void SysTick_Handler(void) {
    time_count++;
}

void delay_ms(uint32_t ms) {
        uint32_t stop_time = ms;
        uint32_t start = time_count;
       

        while((uint32_t)(time_count - start) <= stop_time);
}

void SysTick_initialize(void) {
    // TODO: figure out what each line of code in this function does
    SysTick->CTRL = 0;
    SysTick->LOAD = 3999; // TODO: fill this in with an appropriate value
    // This sets the priority of the interrupt to 15 (2^4 - 1), which is the
    // largest supported value (aka lowest priority)
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}


int main() 
{
        host_serial_init(9600);
        SysTick_initialize();
        delay_ms(3000);
        ssd1309_basic_init(SSD1309_INTERFACE_IIC, SSD1309_ADDR_SA0_0);
        uint8_t res = ssd1309_basic_display_on();
        ssd1309_basic_clear();
        delay_ms(5000);
        res =  ssd1309_basic_rect(0, 0, 50, 40, 180);
        printf("Result: %d\n", res);
        ssd1309_basic_clear();
        return 0;
}