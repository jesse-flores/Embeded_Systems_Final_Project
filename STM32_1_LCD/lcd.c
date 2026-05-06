#include "ee14lib.h"
#include "lcd.h"

//#include "systick.c"

void dev_init(){
    //make Data/Command, CS and RST all outputs

    //CS = D3
    //DC = A2
    //RST = D6
    gpio_config_mode(CS, 0b01); 
    gpio_config_mode(DC, 0b01); 
    gpio_config_mode(RST, 0b01); 

    //make all of them high to match reference
    set_CS(1);
    set_DC(1);
    set_RST(1);

    timer_config_pwm(TIM1, 1000);
    
}


void set_CS(bool set){
    gpio_write(CS, set);
}

void set_DC(bool set){
    gpio_write(DC, set);
}

void set_RST(bool set){
    gpio_write(RST, set);
}

//SET BACKLIGHT FUNCTIONS
void set_backLight(unsigned int value){
    set_PWM(value);
}


void set_PWM(unsigned int value){
    TIM1->CCR1 = value; 
}

//INITIALIZE FUNCTIONS
void lcd_init(){
    //reset
    lcd_reset(); 
    //set attributes
    lcd_setAttributes(); 
    //initialize register
    lcd_initReg();

}

void lcd_reset(){
    set_RST(1);
    delay_ms(100);
    set_RST(0);
    delay_ms(100);
    set_RST(1);
    delay_ms(100);

}

void lcd_setAttributes(){
    lcd_sendCommand(0x36);
    lcd_sendData(0x00); //follow example
}

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
void lcd_initReg() {
    lcd_sendCommand(0x36);
    lcd_sendData(0x00);

    lcd_sendCommand(0x3A);
    lcd_sendData(0x05);

    lcd_sendCommand(0xB2);
    lcd_sendData(0x0B);
    lcd_sendData(0x0B);
    lcd_sendData(0x00);
    lcd_sendData(0x33);
    lcd_sendData(0x35);

    lcd_sendCommand(0xB7);
    lcd_sendData(0x11);

    lcd_sendCommand(0xBB);
    lcd_sendData(0x35);

    lcd_sendCommand(0xC0);
    lcd_sendData(0x2C);

    lcd_sendCommand(0xC2);
    lcd_sendData(0x01);

    lcd_sendCommand(0xC3);
    lcd_sendData(0x0D);

    lcd_sendCommand(0xC4);
    lcd_sendData(0x20);

    lcd_sendCommand(0xC6);
    lcd_sendData(0x13);

    lcd_sendCommand(0xD0);
    lcd_sendData(0xA4);
    lcd_sendData(0xA1);

    lcd_sendCommand(0xD6);
    lcd_sendData(0xA1);

    lcd_sendCommand(0xE0);
    lcd_sendData(0xF0);
    lcd_sendData(0x06);
    lcd_sendData(0x0B);
    lcd_sendData(0x0A);
    lcd_sendData(0x09);
    lcd_sendData(0x26);
    lcd_sendData(0x29);
    lcd_sendData(0x33);
    lcd_sendData(0x41);
    lcd_sendData(0x18);
    lcd_sendData(0x16);
    lcd_sendData(0x15);
    lcd_sendData(0x29);
    lcd_sendData(0x2D);

    lcd_sendCommand(0xE1);
    lcd_sendData(0xF0);
    lcd_sendData(0x04);
    lcd_sendData(0x08);
    lcd_sendData(0x08);
    lcd_sendData(0x07);
    lcd_sendData(0x03);
    lcd_sendData(0x28);
    lcd_sendData(0x32);
    lcd_sendData(0x40);
    lcd_sendData(0x3B);
    lcd_sendData(0x19);
    lcd_sendData(0x18);
    lcd_sendData(0x2A);
    lcd_sendData(0x2E);

    lcd_sendCommand(0xE4);
    lcd_sendData(0x25);
    lcd_sendData(0x00);
    lcd_sendData(0x00);

    lcd_sendCommand(0x21);

    lcd_sendCommand(0x11);
    delay_ms(120);

    lcd_sendCommand(0x29);
}


//CLEAR FUNCTIONS
void lcd_clear(uint16_t Color){
    uint16_t i,j;
    //#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
    
    lcd_set_windows(0,0,WIDTH,HEIGHT); 
    gpio_write(DC, 1);

    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            spi_write((Color >> 8) & 0xFF);
            spi_write(Color);
        }
    }
}


//SEND DATA/COMMANDS

void lcd_sendCommand(uint8_t data){
    set_DC(0);
    set_CS(0);
    spi_write(data);
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
void lcd_sendData(uint8_t data){

    set_DC(1);
    spi_write(data);
}

void lcd_send16Bit(uint16_t data){
    set_DC(1);
    set_CS(0);
    spi_write((data >> 8) & 0xFF);
    spi_write(data & 0xFF);
    set_CS(1);
}


//SET Functions:

void lcd_set_windows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
    //X start, Y start = 0, X End, Y end = 240 ,280
        lcd_sendCommand(0x2A);
        lcd_sendData(Xstart >> 8);
        lcd_sendData(Xstart);
        lcd_sendData((Xend-1) >> 8);
        lcd_sendData(Xend-1);

        // set the Y coordinates
        lcd_sendCommand(0x2B);
        lcd_sendData((Ystart+20) >> 8);
        lcd_sendData(Ystart+20);
        lcd_sendData((Yend+20-1) >> 8);
        lcd_sendData(Yend+20-1);

        lcd_sendCommand(0X2C);
}


//PAINT FUNCTIONS
void draw_pixel(uint16_t X, uint16_t Y, uint16_t Color) {
    
    if (X >= WIDTH || Y >= HEIGHT || X <= 0 || Y <= 0) {
        return; // Guard: do nothing if outside screen
    }
    lcd_set_windows(WIDTH - X, HEIGHT - Y, WIDTH - X, HEIGHT - Y); // Set a 1x1 window at (X,Y)
    lcd_send16Bit(Color);
}


void paint_rectangle(uint16_t Xin, uint16_t Yin, uint16_t Radius, uint16_t Color) {
    
    for(int x = Xin; x < Xin + Radius; x++){
        for (int y = Yin; y < Yin + Radius; y++){
            draw_pixel(x, y, Color);
        }
    }
}


void lcd_setBackground(uint16_t color){
    lcd_clear(color);
}


//ball has radius of 10, 
void draw_ball(uint16_t Xin, uint16_t Yin, uint16_t color) {
    const int radius = 5;
    // iterate over a box surrounding the circle
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            // check if (dx,dy) is inside the circle
            if (dx*dx + dy*dy <= radius*radius) {
                int x = (int)Xin + dx;
                int y = (int)Yin + dy;
                // simple clipping to screen bounds
                if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                    draw_pixel((uint16_t)x, (uint16_t)y, color);
                } 
            }
        }
    }
}

void draw_ball_test(uint16_t Xin, uint16_t Yin, uint16_t color, uint16_t radius){
    // iterate over a box surrounding the circle
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            // check if (dx,dy) is inside the circle
            if (dx*dx + dy*dy <= radius*radius) {
                int x = (int)Xin + dx;
                int y = (int)Yin + dy;
                // simple clipping to screen bounds
                if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                    draw_pixel((uint16_t)x, (uint16_t)y, color);
                } 
            }
        }
    }
}

void draw_hole(uint16_t Xin, uint16_t Yin, uint16_t color) {
    const int radius = 20;
    // iterate over a box surrounding the circle
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            // check if (dx,dy) is inside the circle
            if (dx*dx + dy*dy <= radius*radius) {
                int x = (int)Xin + dx;
                int y = (int)Yin + dy;
                // simple clipping to screen bounds
                if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                    draw_pixel((uint16_t)x, (uint16_t)y, color);
                } 
            }
        }
    }

    //draw the flag now
    for(int i = 0; i < 20; i++){
        draw_pixel(Xin + i-5, Yin, BLACK); 
    }
    for(int j = 0; j < 20; j++){
        draw_pixel(Xin + j-5, Yin - 1, BLACK);
    }
    for(int k = 0; k < 20; k++){
        draw_pixel(Xin + k-5, Yin - 2, BLACK);
    }
    for(int i = 8; i > 0; i--){
        for (int j = 2; j < 10; j++){
            draw_pixel(Xin + 15 - i, Yin+j-1, RED);
        }
    }
    

}



// --- Helper function: draw a pixel rotated by 90 degrees clockwise ---
void draw_pixel_rotated(uint16_t x, uint16_t y, uint16_t color) {
    draw_pixel(HEIGHT - 1 - y, x, color);
}

// --- Helper function: draw the waving flag with rotation ---
void draw_flag(int offset) {
    const uint16_t circle_center_x = 140;
    const uint16_t circle_center_y = 120 + 40;

    // Clear old flag (overdraw with WHITE)
    for (int i = 0; i <= 10; i++) {
        for (int j = 0; j <= 10; j++) {
            draw_pixel_rotated(circle_center_x + j, circle_center_y - 20 + i, WHITE);
        }
    }

    // Draw new waving flag
    for (int i = 0; i <= 10; i++) {
        for (int j = 0; j <= i; j++) {
            draw_pixel_rotated(circle_center_x + j + offset, circle_center_y - 20 + i, RED);
        }
    }
}

// --- Full title screen drawing with rotation ---
void draw_title_screen(void) {
    // 1. Fill screen with Yellow background
    lcd_setBackground(YELLOW);

    // 2. Draw the green rectangle (golf course)
    const uint16_t course_x = 40;
    const uint16_t course_y = 60 + 40;
    const uint16_t course_width = 200;
    const uint16_t course_height = 120;
    for (uint16_t y = course_y; y < course_y + course_height; y++) {
        for (uint16_t x = course_x; x < course_x + course_width; x++) {
            draw_pixel_rotated(x, y, golfgreen);
        }
    }

    // 3. Draw the white circle (the hole)
    const uint16_t circle_center_x = 140;
    const uint16_t circle_center_y = 120 + 40;
    const uint16_t circle_radius = 50;
    for (int dy = -circle_radius; dy <= circle_radius; dy++) {
        for (int dx = -circle_radius; dx <= circle_radius; dx++) {
            if (dx * dx + dy * dy <= circle_radius * circle_radius) {
                draw_pixel_rotated(circle_center_x + dx, circle_center_y + dy, WHITE);
            }
        }
    }

    // 4. Draw the black flagpole
    for (int i = -20; i <= 20; i++) {
        draw_pixel_rotated(circle_center_x, circle_center_y + i, BLACK);
    }

    // 5. Draw the first flag at offset 0
    draw_flag(0);
}



// ─── Seven segment helpers ──────────────────────────────────────────────
// Segment A: top horizontal
static void draw_segA(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;               x < X + SEG_LEN;       x++)
    for (uint16_t y = Y;               y < Y + THICK;         y++)
        draw_pixel(240-y, x, color);
}

// Segment B: upper-right vertical
static void draw_segB(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X + SEG_LEN - THICK;  x < X + SEG_LEN;       x++)
    for (uint16_t y = Y;                   y < Y + SEG_LEN;       y++)
        draw_pixel(240-y, x, color);
}

// Segment C: lower-right vertical
static void draw_segC(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X + SEG_LEN - THICK;     x < X + SEG_LEN;          x++)
    for (uint16_t y = Y + SEG_LEN + THICK;     y < Y + 2*SEG_LEN + THICK; y++)
        draw_pixel(240-y, x, color);
}

// Segment D: bottom horizontal
static void draw_segD(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;                x < X + SEG_LEN;       x++)
    for (uint16_t y = Y + 2*SEG_LEN;   y < Y + 2*SEG_LEN + THICK; y++)
        draw_pixel(240-y, x, color);
}

// Segment E: lower-left vertical
static void draw_segE(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;                  x < X + THICK;           x++)
    for (uint16_t y = Y + SEG_LEN + THICK; y < Y + 2*SEG_LEN + THICK; y++)
        draw_pixel(240-y, x, color);
}

// Segment F: upper-left vertical
static void draw_segF(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;               x < X + THICK;       x++)
    for (uint16_t y = Y;               y < Y + SEG_LEN;     y++)
        draw_pixel(240-y, x, color);
}

// Segment G: middle horizontal
static void draw_segG(uint16_t X, uint16_t Y, uint16_t color) {
    for (uint16_t x = X;               x < X + SEG_LEN;       x++)
    for (uint16_t y = Y + SEG_LEN;     y < Y + SEG_LEN + THICK; y++)
        draw_pixel(240-y, x, color);
}

// ─── draw_digit remains unchanged ────────────────────────────────────────
void draw_digit(uint8_t d, uint16_t X, uint16_t Y, uint16_t color) {
    static const bool segs[10][7] = {
        {1,1,1,1,1,1,0}, //0
        {0,1,1,0,0,0,0}, //1
        {1,1,0,1,1,0,1}, //2
        {1,1,1,1,0,0,1}, //3
        {0,1,1,0,0,1,1}, //4
        {1,0,1,1,0,1,1}, //5
        {1,0,1,1,1,1,1}, //6
        {1,1,1,0,0,0,0}, //7
        {1,1,1,1,1,1,1}, //8
        {1,1,1,1,0,1,1}  //9
    };
    if (d > 9) return;
    if (segs[d][0]) draw_segA(X, Y, color);
    if (segs[d][1]) draw_segB(X, Y, color);
    if (segs[d][2]) draw_segC(X, Y, color);
    if (segs[d][3]) draw_segD(X, Y, color);
    if (segs[d][4]) draw_segE(X, Y, color);
    if (segs[d][5]) draw_segF(X, Y, color);
    if (segs[d][6]) draw_segG(X, Y, color);
}

void draw_char(uint16_t x, uint16_t y, const char character, sFONT *font, 
                uint16_t color_background, uint16_t color_foreground)
{
    uint16_t page, col;

    uint32_t char_offset = (character - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[char_offset];

    for (page = 0; page < font->Height; page ++ ) {
        for (col = 0; col < font->Width; col ++ ) {

            //To determine whether the font background color and screen background color is consistent
            if (WHITE == color_background) { //this process is to speed up the scan
                if (*ptr & (0x80 >> (col % 8)))
                    draw_pixel(x + col, y + page, color_foreground);
                    // Paint_DrawPoint(x + col, y + page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            } else {
                if (*ptr & (0x80 >> (col % 8))) {
                    draw_pixel(x + col, y + page, color_foreground);
                    // Paint_DrawPoint(x + col, y + page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                } else {
                    draw_pixel(x + col, y + page, color_background);
                    // Paint_DrawPoint(x + col, y + page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            //One pixel is 8 bits
            if (col % 8 == 7)
                ptr++;
        }// Write a line
        if (font->Width % 8 != 0)
            ptr++;
    }// Write all
}