#ifndef SAVE_TO_FILE
#define SAVE_TO_FILE

typedef struct
{
	int x, y;

} Point;

typedef struct
{
	Point *segment;
	Point head;
	Point dir;
	int number, score;
	float speed;

} Snake;

typedef struct
{
    int width, height, left_space, above_space, walls_number, stage;
    Point *wall_start, *wall_end;

} Box;

typedef struct
{
    Point *coord;
    int number;

} Food;

typedef struct
{
    int food_number, box_width, box_height, box_width_changed, box_height_changed;
    float snake_speed;

} Options;

const int HEIGHT = 25, WIDTH = 50, SNAKE_LEN = 5;
const float SNAKE_SPEED = 0.08;

// Dezes atstumas nuo kairio ir virsutinio krasto
const int LEFT_SPACE = 27, ABOVE_SPACE = 0;

// Gyvates galvos pozicija
const int SNAKE_HEAD_X = 14, SNAKE_HEAD_Y = 10;

// Maisto gabalu kiekis, gyvates greitis, dezes dydis
const int FOOD_NUMBER = 1;


void save_game(Snake s, Box b, Food f, Options opt);
int open_stage(int stage, Box *box, Snake *snake, int load);

void save_options(Options opt);
void load_options(Options opt);

#endif
