driver_ssd1309.h/.c - Core driver code which handles sending instructions to the SSD1309
driver_ssd1309_basic - a implementation of a driver which uses the driver_ssd1309 interface
iic.c - An implementation of the iic write and reading functions which are consistent with what the driver expects (used by us to create a similar i2c write function later in i2c.c)
i2c.c - i2c reading and writing functions (mostly written by Bell aside from the last function)
uart.c, gpio.c - provided lab functions

At the start of the program the ssd1309_basic_init(ssd1309_interface_t interface, ssd1309_address_t addr) function takes the communication (in our case i2c) and the device address. The driver_ssd.h file has a struct of function pointers which holds all of the functions which the driver would use. The ssd1309_basic_init functions first sets the function pointers to functions which we provide. (i.e we have to define all the i2c writing reading, etc) Then the function runs many other functions which set up the initial state of the display.

When we run the initialize function, we don't get any error messages which implies that it works; however, when we attempt to turn on the screen we don't see any changes.
   
We then use the ssd1309_basic_display_on function, clear the screen and attempt to dispaly a rectangle but the display doesn't change. So we don't know if 
