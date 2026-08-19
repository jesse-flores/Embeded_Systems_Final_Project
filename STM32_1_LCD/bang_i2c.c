#include "bang_i2c.h"


uint8_t bang_i2c_init() {

        gpio_config_mode(SDA, OUTPUT);
        gpio_config_mode(SCL, OUTPUT);
        gpio_config_ospeed(SCL, 0b10 );
        gpio_config_ospeed(SDA, 0b10);
        gpio_config_otype(SCL, 0b00);
        gpio_config_otype(SDA, 0b00);

        gpio_write(SDA, HIGH);
        gpio_write(SCL, HIGH);

        return 0;
}

//helper funcitons
void i2c_start() {
        gpio_config_mode(SDA, OUTPUT);  

        gpio_write(SDA, HIGH);
        gpio_write(SCL, HIGH);

        delay_us(4);
        gpio_write(SDA, LOW);
        delay_us(4);
        gpio_write(SCL, LOW);
}

void i2c_end() {
        gpio_config_mode(SDA, OUTPUT);  

        gpio_write(SDA, LOW);
        gpio_write(SCL, LOW);

        delay_us(4);
        gpio_write(SCL, HIGH);
        delay_us(4);
        gpio_write(SDA, HIGH);
}

bool i2c_nack() {

        uint16_t error_time = 0;
        gpio_write(SDA, HIGH);
        gpio_config_mode(SDA, INPUT);
        delay_us(4);

        gpio_write(SCL, HIGH);
        bool nack = gpio_read(SDA);
        gpio_write(SCL, LOW);

        gpio_config_mode(SDA, OUTPUT);

        return nack;
}

bool i2c_write_byte(uint8_t byte) {

        gpio_write(SCL, LOW);
        for (int i = 0; i < 8; i++) {
                gpio_write(SDA, ((byte & 0x80) >> 7));
                gpio_write(SCL, HIGH);
                delay_us(4);
                gpio_write(SCL, LOW);

                byte <<= 1;
                delay_us(2);
        }

        return !i2c_nack();
}


uint8_t bang_i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len) {

        //send addr
        i2c_start();
        if (!i2c_write_byte((addr << 1))) {
                i2c_end();
                return 1;
        }

        //send register num
         if (!i2c_write_byte(reg)) {
                i2c_end();
                return 1;
        }

        //send over data
        for (uint16_t i = 0; i < len; i++) {
                if (!i2c_write_byte(data[i])) {
                        i2c_end();
                        return 1;
                }  
        }

        i2c_end();
        return 0;
}
