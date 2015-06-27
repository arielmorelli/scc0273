#include <stdio.h>
#include <stdlib.h>

#define motorA 0
#define motorB 1

#define S3 3
#define S4 4

typedef int task;

int motor[2];
int SensorValue[5];

void wait1Msec(int vel){}

void resetMotorEncoder( int motor){}

bool getMotorMoving( int motor ){ return true;}

void setMotorTarget( int motor, int angle, int vel ){}
        
void setSensorMode( int color, int mode ){}

void delay( int value ){}

void printUsage();
