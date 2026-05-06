# Files and Meaning
driver_ssd1309.h/.c - Core driver code which handles sending instructions to the SSD1309 <br>
driver_ssd1309_basic - a implementation of a driver which uses the driver_ssd1309 interface <br>
iic.c - An implementation of the iic write and reading functions which are consistent with what the driver expects (used by us to create a similar i2c write function later in i2c.c) <br>
i2c.c - i2c reading and writing functions (mostly written by Bell aside from the last function)<br>
uart.c, gpio.c - provided lab functions <br>
<br>

# Brief Control Flow Discussion
At the start of the program the ssd1309_basic_init(ssd1309_interface_t interface, ssd1309_address_t addr) function takes the communication (in our case i2c) and the device address. The driver_ssd.h file has a struct of function pointers which holds all of the functions which the driver would use. The ssd1309_basic_init functions first sets the function pointers to functions which we provide. (i.e we have to define all the i2c writing reading, etc) Then the function runs many other functions which set up the initial state of the display. <br>
<br>

# Future Improvements
Current 

YOU CAN CHANGE THE THICKNESS AND LENGNTH OF THE NUMBERS IN LCD.H

THE FUNCTION YOU WANT TO USE IS draw_digit
