include "ee14lib.h"
#include "bang_i2c.h"
#include "fonts.h"
#include "driver_ssd1306_basic.h"
#include "stdio.h"



int main() 
{

    host_serial_init(9600);
    delay_timer_config();
    uint8_t res;
    bang_i2c_init();
   uint8_t data[1] = {0};
   
    //write garbage data to display
    bang_i2c_write(I2C_ADDR, 0x00, data, 1);
    
    res = ssd1306_basic_init(SSD1306_INTERFACE_IIC, SSD1306_ADDR_SA0_0);
    if (res != 0)
    {
        return 1;
    }

    
    res = ssd1306_basic_clear();
    if (res != 0)
    {
        ssd1306_interface_debug_print("ssd1306: clear screen failed.\n");
        (void)ssd1306_basic_deinit();

        return 1;
    }

    res = ssd1306_basic_string(0, 0, "IT FIRCKIN WOORKSS!!", 21, 1, SSD1306_FONT_12);
    if (res != 0)
    {
        ssd1306_interface_debug_print("ssd1306: show string failed.\n");
        (void)ssd1306_basic_deinit();

        return 1;
    }

    delay_ms(10000);

    res = ssd1306_basic_clear();
    if (res != 0)
    {
        ssd1306_interface_debug_print("ssd1306: clear screen failed.\n");
        (void)ssd1306_basic_deinit();

        return 1;
    }

    res = ssd1306_basic_rect(0, 0, 31, 31, 1);
    if (res != 0)
    {
        (void)ssd1306_basic_deinit();

        return 1;
    }    
    delay_ms(1000);
    // ...
    (void)ssd1306_basic_deinit();

    return 0;
}
