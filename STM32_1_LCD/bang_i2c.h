#include <stdio.h>
#include "ee14lib.h" // used for delay functions and pins

#define SDA D0
#define SCL D1
#define HIGH 0b01
#define LOW 0b00


uint8_t bang_i2c_init();
uint8_t bang_i2c_deinit();

// i2c writing funcitons
uint8_t bang_i2c_write_cmd(uint8_t addr, uint8_t *data, uint16_t len);
uint8_t bang_i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len);

//i2c helper functions
void i2c_start();
void i2c_end();
bool i2c_write_byte(uint8_t byte);
// uint8_t bang_i2c_read(uint8_t addr, uint8_t *data, uint16_t len);
