/**
 * By Preston Sundar
 *
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include "pca9685.h"
#include "i2c.h"

uint16_t globalFrequency;
uint8_t globalAddress;

void pca9685_init(uint8_t address, uint16_t freq)
{
    globalFrequency = freq;
    globalAddress = address;

    i2c_init();

    /* Select slave device, select MODE1 register, then set AI (auto-increment)
       enable, SLEEP active, and ALLCALL enable */
    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(0x40 + address); // 0x40 adds addr MSB (fig 4 in datasheet)
    i2c_tx_byte(0x00); // MODE1 register
    i2c_tx_byte(0b00110001); // set register configuration
    i2c_tx_stop();

    _delay_ms(1);

    /* Calculate frequency prescalar for usage below. PCA9685 contains a 25 MHz
       internal clock that can be prescaled to achieve the desired output
       frequency. The prescalar can be a number between 0xFF (24 Hz) and 0x03
       (1526 Hz). Multiply frequency by 0.92 to compensate for frequency
       inaccuracy. Declare the variable as volatile to force computation
       where written in source, otherwise compiler will place expression inline
       where it is used (during I2C transmission, causing a brief delay due to
       time it takes to compute result). Note: equation can be optimized
       without needing a float) */
    volatile uint8_t prescalar = (25000000 / ((float)4096 * freq * 0.92)) - 1;

    /* Select slave device, select PRESCALE register, then set output driver
       frequency using prescalar */
    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(0x40);
    i2c_tx_byte(0xFE); // PRESCALE register
    i2c_tx_byte(prescalar); // set register configuration
    i2c_tx_stop();

    _delay_ms(1);

    /* Select slave device, select MODE1 register, then set AI (auto-increment)
       enable, SLEEP disable, and ALLCALL enable */
    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(0x40);
    i2c_tx_byte(0x00); // MODE1 register
    i2c_tx_byte(0b10100001); // set register configuration
    i2c_tx_stop();

    _delay_ms(1);

    /* Select slave device, select MODE2 register, then set INVRT (inverted
       output) disable, OCH (outputs change on STOP command) enable, OUTDRV
       (output driver configuration) to totem pole output and OUTNE (output not
       enable mode) to 0x00 */
    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(0x40);
    i2c_tx_byte(0x01); // MODE2 register
    i2c_tx_byte(0b00000100); // set register configuration
    i2c_tx_stop();
}

void pca9685_servo(uint8_t servoNum, float angle)
{
    /* Set limits on angle (-90 to 90 degrees) */
    if (angle > 90) {
        angle = 90;
    } else if (angle < -90) {
        angle = -90;
    } else {}

    /* Calculate the pulse duration in us (note: equation can be optimized) */
    uint16_t pulse_us = NEUTRAL_PULSE + angle *\
                        ((float)(MAX_PULSE - MIN_PULSE) / (2 * MAX_ANGLE));

    /* Convert pulse duration into a value from 0 to 4096, which will be
       repeated every (1 / frequency) * 1000000 us */
    uint16_t period_us = (float)1000000 / globalFrequency;
    uint16_t count = ((float)pulse_us / period_us) * 4096;

    /* Output turns on at 0 counts (simplest way), and will turn off according
       to calculations above. Break the 12-bit count into two 8-bit values */
    uint8_t offLowCmnd = count;
    uint8_t offHighCmnd = count >> 8;

    /* Each output is controlled by 2x 12-bit registers: ON to specify the count
       time to turn on the LED (a number from 0-4095), and OFF to specify the
       count time to turn off the LED (a number from 0-4095). Each 12-bit
       register is composed of 2 8-bit registers: a high and low. */

    /* Select slave device, select LEDXX_ON_L register, set contents of
       LEDXX_ON_L, then set contents of next 3 registers in sequence (only if
       auto-increment is enabled). */
    i2c_tx_start(MASTER_TRANSMITTER);
    i2c_tx_address(0x40 + globalAddress);
    i2c_tx_byte(SERVO0 + (4 * servoNum)); // select LEDXX_ON_L register
    i2c_tx_byte(0x00); // set value of LEDXX_ON_L
    i2c_tx_byte(0x00); // set value of LEDXX_ON_H
    i2c_tx_byte(offLowCmnd); // set value of LEDXX_OFF_L
    i2c_tx_byte(offHighCmnd); // set value of LEDXX_OFF_H
    i2c_tx_stop();
}
