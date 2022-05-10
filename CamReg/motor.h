#ifndef MOTOR_H_
#define MOTOR_H_

#define FORWARD 0
#define RIGHT 1
#define LEFT 2

void stop(void);

void move(int speed, int direction, int steps);

void keep(double distance);

void calibration_motor(void);

void motor_start(void);

#endif /* MOTOR_H_ */
