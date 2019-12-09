#ifndef pca9685_h
#define pca9685_h

#define SERVO0 0x06

#define NEUTRAL_PULSE 1500
#define MAX_PULSE 2100
#define MIN_PULSE 900
#define MAX_ANGLE 90

/*
 * Configure PCA9685 slave device
 *
 * Sets the slave device MODE1, PRESCALE (controls the output frequency) and
 * MODE2 register values.
 *
 * Arguments:
 *      address    The PCA9685 hardwired 6-bit slave device address.
 *
 *      freq       Output driver frequency (in Hz). Must be a value between
 *                 24 Hz and 1526 Hz
 */
void pca9685_init(uint8_t address, uint16_t freq);

/*
 * Set the servo horn angle
 *
 * Sets the slave device MODE1, PRESCALE (controls the output frequency) and
 * MODE2 register values.
 *
 * Arguments:
 *      servoNum    The index of the servo motor. A number between 0 and 15.
 *
 *      angle       Angle in degrees. Must be a value between -90 and 90
 *                  degrees.
 */
void pca9685_servo(uint8_t servoNum, float angle);

#endif
