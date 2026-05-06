# Whack-A-Mole

![Picture From Showcase - 1](/assets/Showcase_1.jpg)

For our final project for embeeded systems, our team decided to build an interactible one-player whack-a-mole game. Tasked to communicate with pheripherals we settled on a display with SPI communication allowing us to display scores. Our design consists of two STM32 systems. Spliting the task across two STM32 boards, we achieved robust game-handling and live-score displaying though the use of two wires connecting the two boards together for updating.


# Poster
![Poster Designed for Showcase at Tufts' Tsungming Tu Complex](/assets/EE14_Amouzou_Flores_Lin_Williams.png)

# Brief Control Flow Discussion

![Gameplay demo 2](/assets/Game_Play_2.gif)

At the start of the program the ssd1309_basic_init(ssd1309_interface_t interface, ssd1309_address_t addr) function takes the communication (in our case i2c) and the device address. The driver_ssd.h file has a struct of function pointers which holds all of the functions which the driver would use. The ssd1309_basic_init functions first sets the function pointers to functions which we provide. (i.e we have to define all the i2c writing reading, etc) Then the function runs many other functions which set up the initial state of the display. <br>

We then rely on our other STM32 board to activate the appropiate data line we're using to signal an increase or decrase in score for the player activated by our game logic when a player either gets the button-to-mole correct as in our simple-mode or gets wrong -- decreasing the score -- as per our challenge-mode. The following video demo showcases the playthrough.
<br>

# Files and Structure

## STM32_1_LCD
driver_ssd1309.h/.c - Core driver code which handles sending instructions to the SSD1309 <br>
driver_ssd1309_basic - a implementation of a driver which uses the driver_ssd1309 interface <br>
iic.c - An implementation of the iic write and reading functions which are consistent with what the driver expects (used by us to create a similar i2c write function later in i2c.c) <br>
i2c.c - i2c reading and writing functions (mostly written by Bell aside from the last function)<br>
uart.c, gpio.c - provided lab functions <br>
<br>

## STM32_2_GAME_LOGIC
game.h/.c - Core game logic employing deep technical skills learnt across EE14 with finite state machines, struct and typedef datatypes. 
<br>



# Gameplay Demo
![Gameplay demo 1](/assets/Game_Play_1.gif)

# Showcase

![Picture From Showcase - 2](/assets/Showcase_2.jpg)

![Picture From Showcase - 3](/assets/Showcase_3.jpg)

![Picture From Showcase - 4](/assets/Showcase_4.jpg)
