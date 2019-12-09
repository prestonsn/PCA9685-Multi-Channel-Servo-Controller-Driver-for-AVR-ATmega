# PCA9685-Multi-Channel-Servo-Controller-Driver-for-AVR-ATmega

Driver for the 32 channel servo controller. 

Example usage:

``` C
#include <avr/io.h>
#include <util/delay.h>
#include "pca9685.h"


int main(void)
{
    pca9685_init(0x00, 50); // start PCA9685 device 0x00 at 50 Hz output

    _delay_ms(5);

    int8_t angle = 0;

    pca9685_servo(0, 0);

    while (1) {
        pca9685_servo(0, 0); // set servo0 position to 0 degrees

        for (angle = -90; angle <= 90; angle += 5) {
            pca9685_servo(0, angle);
            _delay_ms(25);
        }

        for (angle = 90; angle >= -90; angle -= 5) {
            pca9685_servo(0, angle);
            _delay_ms(25);
        }
    }

    return 0;
}

```
