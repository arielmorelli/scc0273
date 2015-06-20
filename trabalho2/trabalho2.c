#define MAX_X 11
#define MAX_Y 11

#define MIN_OBJECT_DIST 10

#define COLOR S2
#define GIRO S3
#define SONAR S4

enum direction{ NONE, UP, DOWN, LEFT, RIGHT, WALL};
enum bool{ false, true};

direction current_direction;
int max_deep;
int current_deep;
int current_x, current_y;
int origin_x, origin_y;

direction parents_map[MAX_X][MAX_Y];

void init(){
    current_deep = 0;
    max_deep = 0;

    for( int i = 0 ; i < MAX_X ; i++ ){
        for( int j = 0 ; j < < MAX_Y; j++ ){
            parents_map[i][j] = NONE;
        }
    }

    origin_x = (int)(MAX_X/2);
    origin_y = 0;
    current_x = origin_x;
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
            return current_x
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
    //TODO: função de virar o robo 90 graus sentido anti-horario
    current_direction = antiClockWiseDirection( current_direction );
}

void turnClockWise(){
    //TODO: função de virar o robo 90 graus sentido horario
    current_direction = clockWiseDirection( current_direction );
}

int readSonar(){
    int repetitions = 0;
    int dist = 0;
    int new_read;

    for( int count = 0 ; count < 20 ; count++ ){
        new_read = SensorValue[SONAR];
        if( dist == new_read ){
            repetitions++
        }
        else{
            dist = new_read;
            repetitions = 0;
        }
        if( repetitions == 5 ){
            return dist;
        }
        delay(10);
    }
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
            || ( parents_map[next_x][next_y] == WALL ) ){
        return true;
    }
    if( parents_map[next_x][next_y] == NONE ){
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
    current_x = getForwardX();
    current_y = getForwardY();
    goForward();
    current_deep--;
}

void verifyOrigin(){
    if( current_x == origin_x ){
        if( current_y == origin_y ){
            if( current_direction == TOP ){
                nextDeep();
                do{
                    turnAntiClockWise();
                }while( !goToSon() );
            }
        }
    }
}

void turnToNextDirection(){
    turnClockWise();
}

int main( int argc, char **argv ){
    bool finished = false;

    init();

    if( isFinal() ){
        finished = true;
    }

    while( !finished ) {

        verifyOrigin();

        if( current_deep == max_deep ){
            if( isFinal() ){
                finished = true;
                continue;
            }
            backToParent();
            continue;
        }

        turnToNextDirection();

        while( !goToSon() ){
            turnAntiClockWise();
            if( current_direction == parents_map[current_x][current_y] ){
                backToParent();
                continue;
            }
        }
    }

}
