#ifndef GAME_H
#define GAME_H


#include "ee14lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_MOLES 4
#define RED_BUTTON A0
#define YELLOW_BUTTON A1
#define GREEN_BUTTON A2
#define BLUE_BUTTON A3
#define RED_LED D1
#define YELLOW_LED A5
#define GREEN_LED A6
#define BLUE_LED D2
#define RED_RING D9
#define YELLOW_RING D10
#define GREEN_RING D11
#define BLUE_RING D12
#define display_score_increase D3
#define display_score_decrease D6


typedef enum {
    GAME_IDLE,
    GAME_PLAYING,
    GAME_OVER
} GameState;

typedef struct {
    GameState current_state;
    int score;
    uint32_t game_start_time;

    int mole_on;
    uint32_t t_mole_lit_at;

    int num_of_moles_up;
    uint32_t mole_timeout_ms;

    uint32_t next_mole_time;
    uint32_t mole_lit_until;

    int button_led_on;
    uint32_t button_led_off_time;

    bool game_mode;

    int led_on;
    int last_button_pressed;

} GameContext;


void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void timer_init(void);
uint32_t millis(void);
void config_interrupt(void);
void gpio_init(void);
uint32_t rand_num();
uint32_t random_mole_timeout();
uint32_t scale_to_timeout_range();
int random_mole_index();
void update_score(GameContext *game, int was_hit, bool game_mode);
void turn_all_leds_off(void);
void enter_playing(GameContext *game);
void enter_idle(GameContext *game);
void Game_Update(GameContext *game, bool game_mode);
void Game_Init(GameContext *game);
void light_show(void);

#endif