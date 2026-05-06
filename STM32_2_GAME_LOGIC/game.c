#include "ee14lib.h"
#include "game.h"
#include <stdio.h>

#define GAME_LENGTH_MS 45000
#define MOLE_LIT_TIME_MS 1000
#define BUTTON_FLASH_MS 100


uint32_t next_mole_time;
uint32_t mole_lit_until;
uint32_t button_led_off_time;
int button_led_on;
int curr_high_score = 0;
bool game_mode;
volatile uint32_t cycles_count = 0;
volatile int button_pressed = -1;
volatile int active_led = -1;
volatile int led_duty = 0;
volatile uint32_t ms_ticks = 0;
volatile uint32_t last_press_time[4] = {0,0,0,0};
EE14Lib_Pin BUTTON_PINS[4] = {RED_BUTTON, YELLOW_BUTTON, GREEN_BUTTON, BLUE_BUTTON};
EE14Lib_Pin LED_PINS[4] = {RED_LED, YELLOW_LED, GREEN_LED, BLUE_LED}; 
EE14Lib_Pin LED_RING[4] = {RED_RING, YELLOW_RING, GREEN_RING, BLUE_RING};

// Delay function. Busy wait for 'us' microseconds
void delay_us(uint32_t us)
{
    /* Enable DWT cycle counter */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - start) < cycles) {}
}

//delay in milliseconds
void delay_ms(uint32_t ms)
{
    while(ms--) delay_us(1000);
}


void EXTI0_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << 0)) {
        uint32_t now = millis();

        if ((now - last_press_time[0]) > 50) {   // 50 ms debounce
            last_press_time[0] = now;

            if (button_pressed == -1) {
                button_pressed = 1;   // RED button
            }
        }

        EXTI->PR1 = (1 << 0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << 1)) {
        uint32_t now = millis();

        if ((now - last_press_time[1]) > 50) {
            last_press_time[1] = now;

            if (button_pressed == -1) {
                button_pressed = 2;   // YELLOW button
            }
        }
        EXTI->PR1 = (1 << 1);
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << 3)) {
        uint32_t now = millis();

        if ((now - last_press_time[2]) > 50) {
            last_press_time[2] = now;

            if (button_pressed == -1) {
                button_pressed = 3;   // GREEN button
            }
        }
        EXTI->PR1 = (1 << 3);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI->PR1 & (1 << 4)) {
        uint32_t now = millis();

        if ((now - last_press_time[3]) > 50) {
            last_press_time[3] = now;

            if (button_pressed == -1) {
                button_pressed = 4;   // BLUE button
            }
        }
        EXTI->PR1 = (1 << 4);
    }
}


void timer_init(void)
{
    // Generate interrupt every 1 ms
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL  = 0;

    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk |   // CPU clock
        SysTick_CTRL_TICKINT_Msk   |   // enable interrupt
        SysTick_CTRL_ENABLE_Msk;       // start
}

// Runs automatically every 1 ms
void SysTick_Handler(void)
{
    ms_ticks++;
}

//Returns milliseconds since startup
uint32_t millis(void)
{
    return ms_ticks;
}


void config_interrupt(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[0] &= ~(0xF << 0); // clear EXIT0
    SYSCFG->EXTICR[0] &= ~(0xF << 4); // clear EXIT1
    SYSCFG->EXTICR[0] &= ~(0xF << 12); // clear EXTI3
    SYSCFG->EXTICR[1] &= ~(0xF << 0); // clear EXIT4

    // Falling edge (CHANGE TO RISING EDGE (got changed from your code (ekow)))
    EXTI->RTSR1 |= (1 << 0);
    EXTI->RTSR1 |= (1 << 1);
    EXTI->RTSR1 |= (1 << 3);
    EXTI->RTSR1 |= (1 << 4);

    // unmask
    EXTI->IMR1 |= (1 << 0);
    EXTI->IMR1 |= (1 << 1);
    EXTI->IMR1 |= (1 << 3);
    EXTI->IMR1 |= (1 << 4);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_EnableIRQ(EXTI4_IRQn);
}


//Initialize all gpio pins needed
void gpio_init(void)
{
    gpio_config_direction(display_score_decrease, OUTPUT);
    gpio_config_direction(display_score_increase, OUTPUT);
    for (volatile int i = 0; i < NUM_MOLES; i++) {
        gpio_config_direction(BUTTON_PINS[i], INPUT);
        gpio_config_direction(LED_PINS[i], OUTPUT);
        gpio_config_direction(LED_RING[i], OUTPUT);
        if (i == 0 || i == 2) gpio_write(LED_PINS[i], 1); // turn everything off
        else gpio_write(LED_PINS[i], 0);
        gpio_write(LED_RING[i], 0);
        gpio_config_pullup(BUTTON_PINS[i], PULL_UP);
    }
}

//Returns number in range [300ms, 2s]
uint32_t scale_to_timeout_range()
{
    return 300 + (rand() % (2000 - 300 + 1));
}

//Selects which mole should be lit up after the random_mole_timeout has run and returned a timeout value
int random_mole_index()
{
    return (rand() % 4) + 1;
}

/* can use bool once we get it working
Game_mode: 1 is hard setting where score redueces if wrong mole hit and 0 is normal where score only increments when target is hit
*/
void update_score(GameContext *game, int was_hit, bool game_mode)
{
    if (game_mode){
        if (was_hit) {
            game->score++;
            gpio_write (display_score_increase, 1);
            gpio_write(display_score_decrease, 0);
            delay_ms(20);
            printf("increase\n");
            gpio_write (display_score_increase, 0);
            delay_ms(20);
        }
        else {
            game->score--;
            gpio_write (display_score_decrease, 1);
            gpio_write(display_score_increase, 0);
            delay_ms(20);
            printf("decrease\n");
            gpio_write (display_score_decrease, 0);
            delay_ms(20);
        }
    }

    else {
        if (was_hit) {
            game->score++;
            gpio_write (display_score_increase, 1);
            gpio_write(display_score_decrease, 0);
            printf("increase\n");
            delay_ms(20);
            gpio_write (display_score_increase, 0);
            delay_ms(20);
        }
    }
    
    //display_score(game->score);
}

void turn_all_leds_off(void)
{
    active_led = -1;
    led_duty = 0;
    for (int i = 0; i < 4; i++) {
        gpio_write(LED_PINS[i], 0);
        gpio_write(LED_RING[i], 0);
    }
}

// not done
void enter_playing(GameContext *game)
{
    game->mole_on = random_mole_index();
    game->t_mole_lit_at = millis();
    game->mole_lit_until = game->t_mole_lit_at + MOLE_LIT_TIME_MS;

    active_led = game->mole_on;

    //turn on led ring around mole that is on
    for (int i = 0; i < NUM_MOLES; i++) {
        if (active_led == i + 1) {
            gpio_write(LED_RING[i], 1);
        }
    }
    if (game->score <= curr_high_score) curr_high_score = game->score;
    game->num_of_moles_up++; //Keep track of number of moles that lit up throughout the game
}

void enter_idle(GameContext *game) // resets everything
{
    turn_all_leds_off();
    //game->current_state = GAME_IDLE;
    game->score = 0;
    game->num_of_moles_up = 0;
    game->mole_on = -1; //None of the moles are on
    button_pressed = -1;
    //display_idle_screen(); // opening screen
}


void Game_Update(GameContext *game, bool game_mode)
{
    switch (game->current_state) {

    case GAME_IDLE:
    //printf("Waiting to begin...\n\n");

    if (button_pressed == 1) {          // red = hard
        game->game_mode = 1;
        game->game_start_time = millis();
        game->next_mole_time = millis();
        button_pressed = -1;
        light_show();
        game->current_state = GAME_PLAYING;
    }
    else if (button_pressed == 3) {     // green = easy
        game->game_mode = 0;
        game->game_start_time = millis();
        game->next_mole_time = millis();
        button_pressed = -1;
        light_show();
        game->current_state = GAME_PLAYING;
    }
    else if (button_pressed != -1) button_pressed = -1;   // ignore yellow/blue in idle

    break;

    case GAME_PLAYING: {
    uint32_t now = millis();
    uint32_t elapsed = now - game->game_start_time;

    //printf("Time passed: %lu\n", elapsed);
    //printf("PLAYER SCORE: %d\n\n", game->score);

    if (elapsed >= GAME_LENGTH_MS) {
        game->current_state = GAME_OVER;
        break;
    }

    // If a mole is currently lit, turn it off after fixed lit time
    if (game->mole_on != -1 && now >= game->mole_lit_until) {
        turn_all_leds_off();
        game->mole_on = -1;
        game->next_mole_time = now + scale_to_timeout_range();
    }

    // If no mole is lit and enough time has passed, spawn next mole
    if (game->mole_on == -1 && now >= game->next_mole_time) {
        enter_playing(game);
    }

    int pressed = button_pressed;

    if (pressed != -1) {

        // Light the pressed button immediately
        gpio_write(LED_PINS[pressed - 1], 1);
        game->button_led_on = pressed;
        game->button_led_off_time = now + BUTTON_FLASH_MS;

        // Score only if mole is still currently active
        if (game->mole_on != -1 && pressed == game->mole_on) {
            update_score(game, 1, game->game_mode);
            turn_all_leds_off();
            game->mole_on = -1;
            game->next_mole_time = now + scale_to_timeout_range();
        }
        else {
            update_score(game, 0, game->game_mode);
        }
        button_pressed = -1;
    }

    // Turn off button flash after 200 ms
    if (game->button_led_on != -1 && now >= game->button_led_off_time) {
        gpio_write(LED_PINS[game->button_led_on - 1], 0);
        game->button_led_on = -1;
    }

    break;
}

    case GAME_OVER:
       // printf("GAME OVER!!!\n\n");
        light_show();
        //printf("HIGH SCORE IS: %d\n\n", curr_high_score);
        enter_idle(game);
        Game_Init(game);
        game->current_state = GAME_IDLE;

        break;

    default:
        game->current_state = GAME_IDLE;
    }
}


void Game_Init(GameContext *game)
{
    // stm and hardware setup
    game->game_start_time = 0;
    gpio_init();
    config_interrupt();
    timer_init();
    srand(millis()^ SysTick->VAL);
    //display_idle_screen();
    // initialize the lcd screen with function
    game->current_state = GAME_IDLE;
    game->score = 0;
    game->mole_on = -1;
    game->next_mole_time = 0;
    game->mole_lit_until = 0;
    game->button_led_on = -1;
    game->button_led_off_time = 0;
    game->game_mode = 0;
    delay_ms(2000);
    //printf ("Game Initialized!\n");
}

void light_show(void)
{
    // Flash all button LEDs and ring LEDs together
    for (int repeat = 0; repeat < 3; repeat++) {
        for (int i = 0; i < NUM_MOLES; i++) {
            gpio_write(LED_PINS[i], 1);
            gpio_write(LED_RING[i], 1);
        }

        delay_ms(150);

        for (int i = 0; i < NUM_MOLES; i++) {
            gpio_write(LED_PINS[i], 0);
            gpio_write(LED_RING[i], 0);
        }

        delay_ms(150);
    }

    // Sweep forward
    for (int i = 0; i < NUM_MOLES; i++) {
        gpio_write(LED_PINS[i], 1);
        gpio_write(LED_RING[i], 1);
        delay_ms(120);
        gpio_write(LED_PINS[i], 0);
        gpio_write(LED_RING[i], 0);
    }

    // Sweep backward
    for (int i = NUM_MOLES - 1; i >= 0; i--) {
        gpio_write(LED_PINS[i], 1);
        gpio_write(LED_RING[i], 1);
        delay_ms(120);
        gpio_write(LED_PINS[i], 0);
        gpio_write(LED_RING[i], 0);
    }

    // Alternating pattern: red/green, then yellow/blue
    for (int repeat = 0; repeat < 3; repeat++) {
        gpio_write(LED_PINS[0], 1);
        gpio_write(LED_RING[0], 1);
        gpio_write(LED_PINS[2], 1);
        gpio_write(LED_RING[2], 1);

        gpio_write(LED_PINS[1], 0);
        gpio_write(LED_RING[1], 0);
        gpio_write(LED_PINS[3], 0);
        gpio_write(LED_RING[3], 0);

        delay_ms(180);

        gpio_write(LED_PINS[0], 0);
        gpio_write(LED_RING[0], 0);
        gpio_write(LED_PINS[2], 0);
        gpio_write(LED_RING[2], 0);

        gpio_write(LED_PINS[1], 1);
        gpio_write(LED_RING[1], 1);
        gpio_write(LED_PINS[3], 1);
        gpio_write(LED_RING[3], 1);

        delay_ms(180);
    }

    for (int i = 0; i < NUM_MOLES; i++) {
        gpio_write(LED_PINS[i], 1);
        gpio_write(LED_RING[i], 1);
    }

    delay_ms(300);
    turn_all_leds_off();

}