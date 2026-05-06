#include "ee14lib.h"
#include "fonts.h"
#include "stdio.h"
sFONT font;       

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

#include "lcd.h"
static void draw_minus(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;               x < X + SEG_LEN;       x++)
    for (uint16_t y = Y;               y < Y + THICK;         y++)
        draw_pixel(240-y, x, color);
}

void draw_score(int score) 
{
    if (score < 0) {
        // printf("score is negative: %d\n", score);
        draw_minus(65, 130, BLACK);
        score = -score;
    } 
    if (score < 10){
        draw_digit(score, 100, 100, BLACK);
    }else {
        uint8_t ones = score % 10;
        uint8_t tens = score / 10;
        draw_digit(tens, 100, 100, BLACK);
        draw_digit(ones, 135, 100, BLACK);
    }
    
}

int main() 
{

    host_serial_init(9600);
    SysTick_initialize();

    spi_init();
    dev_init();
    lcd_init();
    gpio_config_mode(A5, INPUT);
    gpio_config_pullup(A5, PULL_DOWN);
    gpio_config_mode(A6, INPUT);
    gpio_config_pullup(A6, PULL_DOWN);
    gpio_config_mode(A3, INPUT);
    gpio_config_pullup(A3, PULL_DOWN);
    

    // font = Font12;
    
    lcd_clear(WHITE);
    set_backLight(1000);

 
    int score = 0;
    lcd_clear(WHITE);
    draw_score(score);
    delay_ms(500);
    while(1) {

        // printf("increase: %d\n", gpio_read(A6));
        // printf("decrease: %d\n", gpio_read(A5));
        // printf("reset: %d\n", gpio_read(A3));

        if(gpio_read(A6) == 1) {
            lcd_clear(WHITE);
            draw_score(++score);
        } else if (gpio_read(A5) == 1) {
            lcd_clear(WHITE);
            draw_score(--score);
        } else if(gpio_read(A3) == 1) {
            score = 0;
            lcd_clear(WHITE);
            draw_score(score);

            // delay_ms(500);
     }
    //  printf("what?\n");
        // delay_ms(100);

        // for (int i = -20; i < 10; i++) {
        //     lcd_clear(WHITE);
        //     draw_score(i);
            
        // }
    // }
    
    // lcd_clear(WHITE);
    // char character = 'd';
    // draw_char(300, 50, character, &Font12, WHITE, BLACK);

    // printf("fonid\n");
    }
    return 0;
}