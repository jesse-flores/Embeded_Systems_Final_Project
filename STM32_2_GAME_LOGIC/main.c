#include "ee14lib.h"
#include "game.h"
#include <stdio.h>

// BOTH -VE MUST BE GROUNDED, LONGER +VE IS 5V AND FINAL +VE IS DATA
int _write(int file, char *data, int len) {
    serial_write_nonblocking(USART2, data, len);
    return len;
    }


int main()
{
    host_serial_init(9600);
    GameContext whack_a_mole;
    Game_Init(&whack_a_mole);

    while (1) {
        Game_Update(&whack_a_mole, 0);
        int r = gpio_read(RED_BUTTON);
        //printf("red button read\n");
        int y = gpio_read(YELLOW_BUTTON);
        int g = gpio_read (GREEN_BUTTON);
        int b = gpio_read (BLUE_BUTTON);
        //printf("r is %d\ny is %d\ng is %d\nb is %d\n\n\n", r, y, g, b);
        delay_ms(500);
    }

}