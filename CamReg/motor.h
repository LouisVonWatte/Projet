#ifndef MOTOR_H_
#define MOTOR_H_

#define FORWARD 0
#define RIGHT 1
#define LEFT 2
#define BACKWARDS 3

int check_turn (int color);

void stop(void);

void move(int speed, int direction, int steps);

void keep(double distance);

void calibration_motor(void);

void go_straight(void);

#endif /* MOTOR_H_ */
