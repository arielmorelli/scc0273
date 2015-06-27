#pragma config(Sensor, S3,     ,               sensorEV3_Ultrasonic)
#pragma config(Sensor, S4,     ,               sensorEV3_Color, modeEV3Color_Color)
#pragma config(Motor,  motorA,          mA,            tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          mB,            tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          mC,            tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD,          mD,            tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "robotc.h"

#define MAX_X 11
#define MAX_Y 11
#define MAX_CITYS 4

#define MIN_OBJECT_DIST 25
#define RED 5
#define BLUE 2

#define ANGLE 90*2
#define VELOCITY 10
#define FORWARDVEL 20

#define SONAR S3
#define COLOR S4

typedef enum { NONE, UP, DOWN, LEFT, RIGHT, WALL, FREECITY, VERIFIED} direction;

direction current_direction;
int max_deep;
int citys_x[MAX_CITYS];
int citys_y[MAX_CITYS];
int current_deep;
int current_x, current_y;
int origin_x, origin_y;
int final_x, final_y;

direction parents_map[MAX_X][MAX_Y];

void setMotorA( int speed ){
    motor[motorA] = speed;
}

void setMotorB( int speed ){
    motor[motorB] = speed;
}

void goForward(){
    setMotorA( FORWARDVEL );
    setMotorB( FORWARDVEL );
    wait1Msec(3900);
    setMotorA( 0 );
    setMotorB( 0 );
}

void init(){
    setMotorA(0);
    setMotorB(0);
    current_deep = 0;
    max_deep = 0;

    for( int i = 0 ; i < MAX_X ; i++ ){
        for( int j = 0 ; j < MAX_Y; j++ ){
            parents_map[i][j] = NONE;
        }
    }
    for( int i = 0 ; i < MAX_CITYS ; i++ ){
        citys_x[i] = -1;
        citys_y[i] = -1;
    }

    origin_x = (int)(MAX_X/2);
    origin_y = 0;
    current_x = origin_x;

    parents_map[origin_x][origin_y] = FREECITY;
    current_y = origin_y;
    current_direction = DOWN;
    max_deep++;
}

direction antiClockWiseDirection( direction dir ){
    switch(dir){
        case UP:
            return LEFT;
        case LEFT:
            return DOWN;
        case DOWN:
            return RIGHT;
        case RIGHT:
            return UP;
        default:
            return NONE;
    }
}

direction clockWiseDirection( direction dir ){
    switch(dir){
        case UP:
            return RIGHT;
        case LEFT:
            return UP;
        case DOWN:
            return LEFT;
        case RIGHT:
            return DOWN;
        default:
            return NONE;
    }
}

direction oppositeDirection( direction dir ){
    switch(dir){
        case UP:
            return DOWN;
        case LEFT:
            return RIGHT;
        case DOWN:
            return UP;
        case RIGHT:
            return LEFT;
        default:
            return NONE;
    }
}

int getForwardX(){
    switch( current_direction ){
        case UP:
            return current_x;
        case LEFT:
            return current_x - 1;
        case DOWN:
            return current_x;
        case RIGHT:
            return current_x + 1;
        default:
            return current_x;
    }
}

int getForwardY(){
    switch( current_direction ){
        case UP:
            return current_y - 1;
        case LEFT:
            return current_y;
        case DOWN:
            return current_y + 1;
        case RIGHT:
            return current_y;
        default:
            return current_y;
    }
}

void turnAntiClockWise(){
    resetMotorEncoder( motorA );
    resetMotorEncoder( motorB );

    setMotorTarget( motorA, ANGLE, -VELOCITY );
    setMotorTarget( motorB, ANGLE, VELOCITY );
    while( getMotorMoving(motorA) ){};
    setMotorA(0);
    setMotorB(0);

    current_direction = antiClockWiseDirection( current_direction );
}

void turnClockWise(){
    resetMotorEncoder( motorA );
    resetMotorEncoder( motorB );

    setMotorTarget( motorA, ANGLE, VELOCITY );
    setMotorTarget( motorB, ANGLE, -VELOCITY );
    while( getMotorMoving(motorB) ){};
    setMotorA(0);
    setMotorB(0);

    current_direction = clockWiseDirection( current_direction );
}

int readSonar(){
    int repetitions = 0;
    int dist = 999999;
    int new_read;

    for( int count = 0 ; count < 20 ; count++ ){
        new_read = SensorValue[SONAR];
        if( abs(dist - new_read) < 0.01 ){
            repetitions++;
        }
        else{
            dist = new_read;
            repetitions = 0;
        }
        if( repetitions == 5 ){
            return dist;
        }
        wait1Msec(50);
    }
    return dist;
}

bool checkSonar(){
    if( readSonar() < MIN_OBJECT_DIST ){
        return true;
    }
    return false;
}

bool verifyWall( int next_x, int next_y ){
    if( (next_x < 0)  || (next_x >= MAX_X)
            || (next_y < 0)  || (next_y >= MAX_Y)
            || ( parents_map[next_x][next_y] == VERIFIED )
            || ( parents_map[next_x][next_y] == WALL ) ){
        return true;
    }
    if( parents_map[next_x][next_y] == FREECITY ){
        return false;
    }
    else if( parents_map[next_x][next_y] == NONE ){
        if( checkSonar() ){
            parents_map[next_x][next_y] = WALL;
            return true;
        }
    }
    return false;
}

bool goToSon(){
    int next_x = getForwardX();
    int next_y = getForwardY();
    if( verifyWall( next_x, next_y ) ){
        return false;
    }
    goForward();
    current_x = next_x;
    current_y = next_y;
    parents_map[current_x][current_y] = oppositeDirection(current_direction);
    current_deep++;
    return true;
}

void nextDeep(){
    max_deep++;
}

void backToParent(){
    do{
        turnAntiClockWise();
    }while( current_direction != parents_map[current_x][current_y] );
    parents_map[current_x][current_y] = VERIFIED;
    current_x = getForwardX();
    current_y = getForwardY();
    goForward();
    current_deep--;
}

void clearMap(){
    for( int i = 0 ; i < MAX_X ; i++ ){
        for( int j = 0 ; j < MAX_Y; j++ ){
            if( parents_map[i][j] == VERIFIED ){
                parents_map[i][j] = FREECITY;
            }
        }
    }
}



void turnToNextDirection(){
    turnClockWise();
}

void addCity(){
    int i = 0;
    while( citys_x[i] != -1 ){
        i++;
    }
    citys_x[i] = current_x;
    citys_y[i] = current_y;

}

void isFinal(){
    int repetitions = 0;
    int color = 0;
    int new_read;

    for( int count = 0 ; count < 20 ; count++ ){
        new_read = SensorValue[COLOR];
        if( color == new_read ){
            repetitions++;
        }
        else{
            color = new_read;
            repetitions = 0;
        }
        if( repetitions == 5 ){
            break;
        }
        wait1Msec(50);
    }
    if( color == RED ){
        final_x = current_x;
        final_y = current_y;
    }
    else if( color == BLUE ){
        addCity();
    }
}

bool verifyRegion( int x, int y ){
    if( ((x - 1) >= 0)
            && (parents_map[x-1][y] == NONE) ){
        return true;
    }
    if( ((x + 1) < MAX_X)
            && (parents_map[x+1][y] == NONE) ){
        return true;
    }
    if( ((y - 1) >= 0)
            && (parents_map[x][y-1] == NONE) ){
        return true;
    }
    if( ((y + 1) < MAX_Y)
            && (parents_map[x][y+1] == NONE) ){
        return true;
    }
    return false;
}

bool hasAnyCityLeft(){
    if( final_x == -1 ){
        return true;
    }
    for( int i = 0 ; i < MAX_CITYS ; i++ ){
        if( citys_x[i] == -1 ){
            return true;
        }
    }
    return false;
}

bool verifyOrigin(){
    if( current_x == origin_x ){
        if( current_y == origin_y ){
            if( current_direction == UP ){
                nextDeep();
                clearMap();
                turnAntiClockWise();
                return true;
            }
        }
    }
    return false;
}

task main( ){
    setSensorMode(COLOR, 8);
    delay(1000);
    setSensorMode(COLOR, 2);

    init();

    isFinal();

    while( hasAnyCityLeft() ) {
        if( current_deep == max_deep ){
            isFinal();
            backToParent();
            continue;
        }
        turnToNextDirection();
        verifyOrigin();

        while( !goToSon() ){
            turnAntiClockWise();
            if( current_direction == parents_map[current_x][current_y] ){
                backToParent();
                continue;
            }

        }
    }

    //colocar o codigo pra ir pro final aqui
}
