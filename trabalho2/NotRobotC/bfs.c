#include "notrobotc.h"

#define MAX_X 1000
#define MAX_Y 1000
#define MAX_CITYS 4

#define MIN_OBJECT_DIST 25
#define RED 5
#define BLACK 1
#define BLUE 2

#define VELOCITY 10
#define FORWARDVEL 10

#define SONAR S3
#define COLOR S4

#define motorA 0
#define motorB 1

#define S3 3
#define S4 4

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
int map[MAX_X][MAX_Y];
int max_row;
int max_col;

void printCell( int value ){
    if( value == 0 ){
        printf(" ");
        return;
    }
    else if( value == 1){
        printf("\x1B[35m");
        printf("#");
    }
    else if( value == 2){
        printf("\x1B[34m");
        printf("C");
    }
    else if( value == 3){
        printf("\x1B[31m");
        printf("C");
    }
    else if( value == 4){
        printf("\x1B[33m");
        printf("O");
    }
    printf("\033[0m");
}

void printfAnotherMap(){
    for( int i = 0 ; i < MAX_X ; i++ ){
        for( int j = 0 ; j < MAX_Y; j++ ){
            printf("%d ", parents_map[i][j]);
        }
        printf("\n");
    }
}

void printMap(){
    for( int i = 0 ; i < 50; i++){
        printf("\n");
    }
    for( int i = 0 ; i < 4; i++){
        if( citys_x[i] != -1 ){
            printf("\x1B[34m");
            printf("City %d: (%d,%d)\n", i, citys_x[i], citys_y[i]);
            printf("\033[0m");
        }
    }
    if( final_x != -1 ){
        printf("\x1B[31m");
        printf("Final: (%d,%d)\n", final_x, final_y);
        printf("\033[0m");
    }

    printf("Actual deep: %d | Max deep: %d\n\n"
            ,current_deep, max_deep);


    for( int i = 0 ; i < max_col+2 ; i++ ){
        printf("\x1B[35m");
        printf("#");
        printf("\033[0m");
    }
    printf("\n");
    for( int i = 0 ; i < max_row ; i++ ){
        printf("\x1B[35m");
        printf("#");
        printf("\033[0m");
        for( int j = 0 ; j < max_col; j++ ){
            if( ( current_x == i ) && (current_y == j ) ){
                printCell(4);
            }
            else{
                printCell(map[i][j]);
            }
        }
        printf("\x1B[35m");
        printf("#");
        printf("\033[0m");
        printf("\n");
    }

    clock_t start = clock();
    clock_t end = clock();
    while( ( (double)(end - start) / CLOCKS_PER_SEC) < 0.2 ){
        end = clock();
    }

}

void setMotorA( int speed ){
    motor[motorA] = speed;
}

void setMotorB( int speed ){
    motor[motorB] = speed;
}

void searchLine(){
    setMotorA(VELOCITY );
    setMotorB( -VELOCITY );
    int a = 0;
    while( (SensorValue[COLOR] != BLACK) && (a <= 10) ){
        a++;
    }
    setMotorB(0);
    setMotorA(0);
    setMotorA( -VELOCITY );
    setMotorB( VELOCITY );
    a = 0;
    while( (SensorValue[COLOR] != BLACK) && (a <= 20) ){
        a++;
    }
    setMotorB(0);
    setMotorA(0);

}

void goForward(){
    for( int a = 0 ; a < 18 ; a ++ ){
        searchLine();
        setMotorA( FORWARDVEL +1);
        setMotorB( FORWARDVEL );
        setMotorA( 0 );
        setMotorB( 0 );
    }

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
    final_x = -1;
    final_y = -1;

    origin_x = 0;
    origin_y = 0;
    current_x = origin_x;
    current_y = 0;

    parents_map[origin_x][origin_y] = VERIFIED;
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
    resetMotorEncoder( motorB );
    resetMotorEncoder( motorA );

    setMotorB( VELOCITY );
    setMotorA( -VELOCITY );
    // while( SensorValue[COLOR] != BLACK ){};
    setMotorB(0);
    setMotorA(0);

    current_direction = antiClockWiseDirection( current_direction );
}

void turnClockWise(){
    resetMotorEncoder( motorA );
    resetMotorEncoder( motorB );

    setMotorA( VELOCITY );
    setMotorB( -VELOCITY );
    //   while( SensorValue[COLOR] != BLACK){};
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
    }
    return dist;
}

bool checkSonar(){
    int forward_x = getForwardX();
    int forward_y = getForwardY();
    if( map[forward_x][forward_y] == 1 ){
        printf("TEM PAREDE SIM");
        return true;
    }
    return false;
}

bool verifyWall( int next_x, int next_y ){
    if( (next_x >= 0) && (next_x < MAX_X) ){
        if( (next_y >= 0) && (next_y < MAX_Y)){
            if( map[next_x][next_y] == 1 ){
                return true;
            }
            if( parents_map[next_x][next_y] == FREECITY ){
                return false;
            }
            if( parents_map[next_x][next_y] == FREECITY ){
                return false;
            }
            if( parents_map[next_x][next_y] == NONE ){
                return false;
            }
        }
    }
    return true;
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
    printMap();
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
    printMap();
}

void clearMap(){
    for( int i = 0 ; i < MAX_X ; i++ ){
        for( int j = 0 ; j < MAX_Y; j++ ){
            if( parents_map[i][j] == VERIFIED ){
                parents_map[i][j] = FREECITY;
            }
        }
    }
    parents_map[origin_x][origin_y] = VERIFIED;
}

void turnToNextDirection(){
    turnClockWise();
}

void addCity(){
    int i = 0;
    while( citys_x[i] != -1 ){
        if( citys_x[i] == current_x
                && citys_y[i] == current_y){
            return;
        }
        i++;
    }
    citys_x[i] = current_x;
    citys_y[i] = current_y;

}

void isFinal(){
    if( map[current_x][current_y] == 3 ){
        final_x = current_x;
        final_y = current_y;
    }
    else if( map[current_x][current_y] == 2 ){
        addCity();
    }
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
            printf("Verificando origem\n");
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

void loadMap( char *map_file ){
    FILE *f;
    f = fopen( map_file, "r");
    int row = 0;
    int col = 0;
    max_col = 0;
    max_row = 0;
    char read;

    if( f == NULL ){
        printf("Error loading map.");
        exit(1);
    }

    while( !feof(f) ){
        fscanf( f, "%c", &read );
        if( read == '\n' ){
            map[row][col] = 1;
            row++;
            if( row > max_row ){
                max_row = row;
            }
            col = 0;
        }
        else{
            map[row][col] = (int)(read - '0');
            col++;
            if( col > max_col ){
                max_col = col;
            }
        }
    }
    for( int i = 0 ; i < max_col+1 ; i++){
        map[row-1][i] = 1;
    }
    fclose(f);
}

void goToFinal(){
    while( current_deep != 0 ){
        backToParent();
    }
    max_deep = 999;

    while( true ){
        if( map[current_x][current_y] == 3){
            return;
        }
        turnToNextDirection();

        while( !goToSon() ){
            turnAntiClockWise();
            if( verifyOrigin() ){
                continue;
            }
            if( current_direction == parents_map[current_x][current_y] ){
                backToParent();
                continue;
            }
        }
    }
}

task main( int argc, char **argv ){

    if( argc != 2 ){
        printf("Usage: BFS [FILE]...\n");
        return 0;
    }
    printf("Map: %s\n", argv[1]);

    setSensorMode(COLOR, 8);
    setSensorMode(COLOR, 2);

    init();

    loadMap( argv[1]);

    isFinal();

    printMap();
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
            if( verifyOrigin() ){
                continue;
            }
            if( current_direction == parents_map[current_x][current_y] ){
                backToParent();
                continue;
            }
        }
    }
    goToFinal();
}
