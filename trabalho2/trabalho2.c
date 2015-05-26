#define MAX_X 11
#define MAX_y 11

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
    origin_y = (int)(MAX_Y/2);
    current_x = origin_x;
    current_y = origin_y;
    current_direction = DOWN;
}

direction AntiClockWiseDirection( direction dir ){
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

void turnAntiClockWise(){
    //TODO: função de virar o robo 90 graus
    current_direction = AntiClockWiseDirection( current_direction );
}


int main( int argc, char **argv ){
    init();

    if( isFinal() ){
        finish();
    }
    max_deep++;

    while( 1 ) {
        if( current_x == origin_x ){
            if( current_y == origin_y ){
                if( current_direction == TOP ){
                    turnToFirstDirection();
                }
            }
        }
        if( current_deep == max_deep ){
            if( isFinal() ){
                finish();
            }
            backToParent();
            continue;
        }
        else if( previous_deep < current_deep ){
            turnToFirstDirection();
        }
        else{
            turnToNextDirection();
        }
        while( hasWall() ){
            turnAntiClockWise();
        }
        if( current_direction == parents_map[current_x][current_y] ){
            backToParent();
            continue;
        }
        goToSon();
    }
}
