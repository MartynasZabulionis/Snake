#include "con_lib.h"
//#include "save_to_file.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <assert.h>

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

int show_newgame_menu(Snake, Box, Food);
int load_game(Snake, Box, Food);

Options opt;


void save_game(Snake s, Box b, Food f, Options opt)
{
    FILE *file;

    if (!(file = fopen("saved.txt", "wb")))
    {
        con_set_pos(0, 20);
        printf(" Zaidimas neissaugotas!");
        LOG_PRINT("Zaidimas neissaugotas.");
    }
    else
    {
        fwrite(&s.dir, 1, sizeof(Point), file);
        fwrite(&s.head, 1, sizeof(Point), file);
        fwrite(&s.number, 1, sizeof(int), file);
        fwrite(&s.score, 1, sizeof(int), file);
        fwrite(&s.speed, 1, sizeof(float), file);
        fwrite(s.segment, s.number - 1, sizeof(Point), file);

        fwrite(&b.stage, 1, sizeof(int), file);
        fwrite(&b.left_space, 1, sizeof(int), file);
        fwrite(&b.above_space, 1, sizeof(int), file);

        fwrite(&f.number, 1, sizeof(int), file);
        fwrite(f.coord, f.number, sizeof(Point), file);

        fwrite(&opt.box_width, 1, sizeof(int), file);
        fwrite(&opt.box_height, 1, sizeof(int), file);

        fclose(file);

        con_set_pos(0, 20);
        printf(" Zaidimas issaugotas!");
        LOG_PRINT("Zaidimas issaugotas.");
    }
}
void save_options(Options opt)
{
    FILE *file;
    if ((file = fopen("options.txt", "wb")))
    {
        fwrite(&opt.snake_speed, 1, sizeof(float), file);
        fwrite(&opt.food_number, 1, sizeof(int), file);
        fwrite(&opt.box_width_changed, 1, sizeof(int), file);
        fwrite(&opt.box_height_changed, 1, sizeof(int), file);
        fclose(file);
        LOG_PRINT("Nustatymai issaugoti.");
        return;
    }
    LOG_PRINT("Nustatymai neissaugoti.");
}
int open_stage(int stage, Box *box, Snake *snake, int load)
{
    char fname[12] = "stage_x.txt";
    fname[6] = (char) stage;
    FILE *f;
    f = fopen(fname, "r");
    if (!f)
    {
        LOG_PRINT("Labirintas %d neuzkrautas.", stage);
        return 0;
    }

    if (load)
        fscanf(f, "%d %d", &box->width, &box->width);
    else
        fscanf(f, "%d %d", &snake->head.x, &snake->head.y);
    fscanf(f, "%d %d", &box->width, &box->height);
    fscanf(f, "%d", &box->walls_number);

    box->wall_start = (Point*) malloc(sizeof(Point) * box->walls_number);
    box->wall_end = (Point*) malloc(sizeof(Point) * box->walls_number);

    for (int i = 0; i < box->walls_number; i++)
    {
        fscanf(f, "%d-%d-%d-%d", &(box->wall_start + i)->x, &(box->wall_end + i)->x, &(box->wall_start + i)->y, &(box->wall_end + i)->y);
    }

    fclose(f);
    LOG_PRINT("Labirintas %d uzkrautas.", stage);

    return 1;
}
void load_options(Options *opt)
{
    FILE *f;
    if ((f = fopen("options.txt", "rb")))
    {
        fread(&opt->snake_speed, 1, sizeof(float), f);
        fread(&opt->food_number, 1, sizeof(int), f);
        fread(&opt->box_width_changed, 1, sizeof(int), f);
        fread(&opt->box_height_changed, 1, sizeof(int), f);
        fclose(f);
        LOG_PRINT("Nustatymai uzkrauti.");
        return;
    }
    opt->snake_speed = SNAKE_SPEED;
    opt->food_number = FOOD_NUMBER;
    opt->box_width_changed = WIDTH;
    opt->box_height_changed = HEIGHT;

    LOG_PRINT("options.txt failas nerastas, nustatymai neuzkrauti.");
}


// Pieðia staciakampá
void draw_box(Box *b)
{
    int i;
    // Remelio piesimas
    for (i = 0; i < opt.box_width; i++)
    {
        con_set_pos(b->left_space + i, b->above_space);
        printf("#");

        con_set_pos(b->left_space + i, b->above_space + opt.box_height - 1);
        printf("#");
    }

    for (i = 0; i < opt.box_height; i++)
    {
        con_set_pos(b->left_space, b->above_space + i);
        printf("#");

        con_set_pos(b->left_space + opt.box_width - 1, b->above_space + i);
        printf("#");
    }
    int check, check2;
    // Sienu piesimas
    con_set_color(COLOR_ORANGE, COLOR_ORANGE);
    for (int j = 0; j < b->walls_number; j++)
    {
        if ((b->wall_start + j)->x != (b->wall_end + j)->x)
        {
            if ((b->wall_start + j)->x > 2)
                i = ((b->wall_start + j)->x - 1) * (((float) opt.box_width) / b->width);
            else
                i = (b->wall_start + j)->x - 1;

            if (b->width - (b->wall_end + j)->x > 2 || b->width - (b->wall_end + j)->x == 0 || opt.box_width == b->width)
                check = (b->wall_end + j)->x * (((float) opt.box_width) / b->width);
            else
                check = (b->wall_end + j)->x * (((float) opt.box_width) / b->width) + ((b->width - (b->wall_end + j)->x) * ( (float) opt.box_width / b->width) - 0.5);

            if ((b->wall_start + j)->y > 2)
            {
                if (b->height - (b->wall_start + j)->y == 1 && ((float) opt.box_height) / b->height != 1)
                    check2 = (b->wall_start + j)->y * (((float) opt.box_height) / b->height) + b->above_space + ((b->height - (b->wall_end + j)->y) * ( (float) opt.box_height / b->height) - 0.5) - 1;
                else
                    check2 = (b->wall_start + j)->y * (((float) opt.box_height) / b->height) + b->above_space - 1;
            }
            else
                check2 = (b->wall_start + j)->y - 1 + b->above_space;

            i += b->left_space;
            check += b->left_space;
            (b->wall_start + j)->x = i;
            (b->wall_end + j)->x = check;
            (b->wall_start + j)->y = check2;
            (b->wall_end + j)->y = check2 + 1;
            for (; i < check; i++)
            {
                con_set_pos(i, check2);
                printf("p");
            }
        }
        else
        {
            if ((b->wall_start + j)->y > 2)
                i = ((b->wall_start + j)->y - 1) * (((float) opt.box_height) / b->height);
            else
                i = (b->wall_start + j)->y - 1;

            if (b->height - (b->wall_end + j)->y > 2 || b->height - (b->wall_end + j)->y == 0  || opt.box_height == b->height)
                check = (b->wall_end + j)->y * (((float) opt.box_height) / b->height);
            else
                check = (b->wall_end + j)->y * (((float) opt.box_height) / b->height) + ((b->height - (b->wall_end + j)->y) * ( (float) opt.box_height / b->height) - 0.5);

            if ((b->wall_start + j)->x > 2)
            {
                if (b->width - (b->wall_start + j)->x == 1 && ((float)opt.box_width/b->width) != 1)
                    check2 = (b->wall_start + j)->x * (((float) opt.box_width) / b->width) + b->left_space + ((b->width - (b->wall_end + j)->x) * ( (float) opt.box_width / b->width) - 0.5) - 1;
                else
                    check2 = (b->wall_start + j)->x * (((float) opt.box_width) / b->width) + b->left_space - 1;
            }
            else
                check2 = (b->wall_start + j)->x - 1 + b->left_space;

            i += b->above_space;
            check += b->above_space;
            (b->wall_start + j)->y = i;
            (b->wall_end + j)->y = check;
            (b->wall_start + j)->x = check2;
            (b->wall_end + j)->x = check2 + 1;
            for (; i < check; i++)
            {
                con_set_pos(check2, i);
                printf("p");
            }
        }
    }
}


int hits_wall(Point h, Box b)
{
    for (int i = 0; i < b.walls_number; i++)
    {
        if (h.x >= (b.wall_start + i)->x && h.x < (b.wall_end + i)->x && h.y >= (b.wall_start + i)->y && h.y < (b.wall_end + i)->y)
            return 1;
    }
    return 0;
}
int bites_itself(Snake s)
{
    int i;
    for (i = s.number - 2; i >= 2; i--)
    {
        if (s.head.x == (s.segment + i)->x && s.head.y == (s.segment + i)->y)
            return 1;
    }
    return 0;
}
int on_snake_or_food(int num, Food food, Snake s)
{
    if ((food.coord + num)->x == s.head.x && (food.coord + num)->y == s.head.y)
        return 1;

    int i;
    for (i = s.number - 2; i >= 0; i--)
    {
        if ((food.coord + num)->x == (s.segment + i)->x && (food.coord + num)->y == (s.segment + i)->y)
            return 1;
    }
    for (i = 0; i < food.number; i++)
    {
        if (i != num && (food.coord + num)->x == (food.coord + i)->x && (food.coord + num)->y == (food.coord + i)->y)
            return 1;
    }
    return 0;
}

void new_food(int num, Food food, Snake s, Box b)
{
    int success = 0;

    (food.coord + num)->x = rand() % (opt.box_width - 2) + 1 + b.left_space;
    (food.coord + num)->y = rand() % (opt.box_height - 2) + 1 + b.above_space;

    for (int i = 0; i < 5; i++)
    {
        if (on_snake_or_food(num, food, s) || hits_wall(*(food.coord + num), b))
        {
            (food.coord + num)->x = rand() % (opt.box_width - 2) + 1 + b.left_space;
            (food.coord + num)->y = rand() % (opt.box_height - 2) + 1 + b.above_space;
        }
        else
        {
            success = 1;
            break;
        }
    }

    if (!success)
    {
        while (1)
        {
            (food.coord + num)->x++;

            if ((food.coord + num)->x > opt.box_width - 2 + b.left_space)
            {
                (food.coord + num)->x = 1 + b.left_space;
                (food.coord + num)->y++;
            }

            if ((food.coord + num)->y > opt.box_height - 2 + b.above_space)
            {
                (food.coord + num)->x = 1 + b.left_space;
                (food.coord + num)->y = 1 + b.above_space;
            }

            if (!on_snake_or_food(num, food, s) && !hits_wall(*(food.coord + num), b))
                break;

        }
    }
    con_set_color(COLOR_GREEN, COLOR_GREEN);
    con_set_pos((food.coord + num)->x, (food.coord + num)->y);
    printf("#");
    fflush(stdout);
    LOG_PRINT("Naujas maistas. Koordinates: (%d, %d).", (food.coord + num)->x, (food.coord + num)->y);
}
void clear_menu(int delete)
{
    con_set_pos(0, 0);
    for (int i = 0; i < 10; i++)
        printf("                          \n");

    if (!delete)
        return;

    con_set_pos(0, 20);
    printf("                       \n               ");
}
void clear_all()
{
    // Paslepia ávedamus simbolius
    con_show_echo(0);

    // Paslepia cursoriø
    con_show_cursor(0);

    // Pieðiam rëmelá
    con_clear();
}
// open_stage buvo
void show_options_menu()
{
    LOG_PRINT("Paspausti nustatymai.");
    while (1)
    {
        con_set_color(COLOR_BLACK, COLOR_GRAY);
        clear_menu(0);
        con_set_pos(0, 0);

        printf(" Nustatymai:\n\n");

        printf(" 1 - Gyvates greitis: ");
        con_set_color(COLOR_RED, COLOR_BLACK);
        printf("%.2f\n", opt.snake_speed);

        con_set_color(COLOR_BLACK, COLOR_GRAY);
        printf(" 2 - Maisto skaicius: ");
        con_set_color(COLOR_GREEN, COLOR_BLACK);
        printf("%d\n", opt.food_number);

        con_set_color(COLOR_BLACK, COLOR_GRAY);
        printf(" 3 - Lentos plotis: ");
        con_set_color(COLOR_MAGENTA, COLOR_BLACK);
        printf("%d\n", opt.box_width_changed);

        con_set_color(COLOR_BLACK, COLOR_GRAY);
        printf(" 4 - Lentos aukstis: ");
        con_set_color(COLOR_MAGENTA, COLOR_BLACK);
        printf("%d\n\n", opt.box_height_changed);

        con_set_color(COLOR_BLACK, COLOR_GRAY);
        printf(" q - Grizti\n");

        int key;
        while ((key = getch()))
        {
            if (key == '1')
            {
                con_set_pos(22, 2);
                con_set_color(COLOR_RED, COLOR_BLACK);
                printf("    ");
                con_set_pos(22, 2);
                scanf("%f", &opt.snake_speed);
                if (opt.snake_speed < 0.02)
                    opt.snake_speed = 0.02;
                if (opt.snake_speed > 5)
                    opt.snake_speed = 5;

                LOG_PRINT("Pakeistas gyvates greitis: %.2f.", opt.snake_speed);
                while(getchar() != '\n');
                break;
            }
            else if (key == '2')
            {
                con_set_pos(22, 3);
                con_set_color(COLOR_GREEN, COLOR_BLACK);
                printf("  ");
                con_set_pos(22, 3);
                scanf("%d", &opt.food_number);
                if (opt.food_number < 1)
                    opt.food_number = 1;
                /*if (opt.food_number > 50)
                    opt.food_number = 50;
*/
                LOG_PRINT("Pakeistas maisto skaicius: %d.", opt.food_number);
                while(getchar() != '\n');
                break;
            }
            else if (key == '3')
            {
                con_set_pos(20, 4);
                con_set_color(COLOR_MAGENTA, COLOR_BLACK);
                printf("   ");
                con_set_pos(20, 4);
                scanf("%d", &opt.box_width_changed);
                if (opt.box_width_changed < 50)
                    opt.box_width_changed = 50;
                if (opt.box_width_changed > 150)
                    opt.box_width_changed = 150;

                LOG_PRINT("Pakeistas lentos dydis: %d.", opt.box_width_changed);
                while(getchar() != '\n');
                break;
            }
            else if (key == '4')
            {
                con_set_pos(21, 5);
                con_set_color(COLOR_MAGENTA, COLOR_BLACK);
                printf("   ");
                con_set_pos(21, 5);
                scanf("%d", &opt.box_height_changed);
                if (opt.box_height_changed < 25)
                    opt.box_height_changed = 25;
                if (opt.box_height_changed > 65)
                    opt.box_height_changed = 65;

                LOG_PRINT("Pakeistas lentos aukstis: %d.", opt.box_height_changed);
                while(getchar() != '\n');
                break;
            }
            else if (key == 'q')
                return;
        }
    }
}

void show_main_menu(Snake s, Box b, Food f)
{
    LOG_PRINT("Ijungtas pagrindinis meniu.");
    int exit = 0;
    while (!exit)
    {
        exit = 1;
        clear_menu(0);
        con_set_pos(0, 0);
        printf (" Meniu:\n\n");
        printf (" 1 - Naujas zaidimas\n");
        printf (" 2 - Uzkrauti zaidima\n\n");
        printf (" 3 - Nustatymai\n\n");

        printf (" q - Iseiti is zaidimo\n");

        int key;
        while ((key = getch()))
        {
            if (key == '1')
            {
                if (!show_newgame_menu(s, b, f))
                    exit = 0;
                else
                    return;
                break;
            }
            else if (key == '2')
            {
                if (!load_game(s, b, f))
                    exit = 0;
                break;
            }
            else if (key == '3')
            {
                show_options_menu();
                exit = 0;
                break;
            }
            else if (key == 'q')
            {
                save_options(opt);
                save_highscore(b, s);
                break;
            }
        };
    }
}
void show_info()
{
    con_set_pos(0, 0);
    printf(" m - Meniu");
}
int show_ingame_menu(Snake s, Box b, Food f)
{
    LOG_PRINT("Ijungtas meniu zaidimo metu.");
    int exit = 0;
    while (!exit)
    {
        exit = 1;
        con_set_color(COLOR_BLACK, COLOR_GRAY);
        clear_menu(0);
        con_set_pos(0, 0);

        printf (" Meniu:\n\n");
        printf (" 1 - Naujas zaidimas\n");
        printf (" 2 - Issaugoti zaidima\n\n");

        printf (" 3 - Nustatymai\n\n");

        printf (" r - Testi zaidima\n");

        printf (" q - Iseiti is zaidimo\n");


        int key;
        while ((key = getch()))
        {
            if (key == '1')
            {
                if (!show_newgame_menu(s, b, f))
                    exit = 0;
                else
                    return 1;
                break;
            }
            else if (key == '2')
            {
                save_game(s, b, f, opt);
            }
            else if (key == '3')
            {
                show_options_menu();
                exit = 0;
                break;
            }
            else if (key == 'r')
            {
                clear_menu(1);
                show_info();
                return 0;
            }
            else if (key == 'q')
            {
                // Jei paspausta q, tai issaugom ir iðeinam ið programos
                save_game(s, b, f, opt);
                save_options(opt);
                save_highscore(b, s);
                return 1;
            }
        }
    }
    return 0;
}

void renew_score(int score, int height)
{
    con_set_color(COLOR_MAGENTA, COLOR_BLACK);
    con_set_pos(LEFT_SPACE, height);
    printf ("Taskai: %d", score);
    LOG_PRINT("Atnaujinti taskai: %d.", score);
}
void snake_draw(Snake s)
{
    con_set_color(COLOR_RED, COLOR_RED);
    con_set_pos(s.head.x, s.head.y);
    printf("#");
    fflush(stdout);

    for (int i = 0; i < s.number - 1; i++)
    {
        // Nupiesiame gyvate
        con_set_pos((s.segment + i)->x, (s.segment + i)->y);
        printf("#");
        fflush(stdout);
    }
}
int read_key(Point *dir, Snake s, Box b, Food f)
{
    // Nuskaitom visus paspaustus klaviðus ið klaviatûros
    int key;
    while ((key = con_read_key()))
    {
        LOG_PRINT("Paspausta %c.", key);
        if (key == 'w')
        {
            dir->x = 0;
            dir->y = -1;
        }
        else if (key == 's')
        {
            dir->x = 0;
            dir->y = 1;
        }
        else if (key == 'a')
        {
            dir->x = -1;
            dir->y = 0;
        }
        else if (key == 'd')
        {
            dir->x = 1;
            dir->y = 0;
        }
        else if (key == 'm')
        {
            if (show_ingame_menu(s, b, f))
                return 1;
            break;
        }
        else if (key == 'q')
        {
            // Jei paspausta q, tai issaugom ir iðeinam ið programos
            save_game(s, b, f, opt);
            save_options(opt);
            save_highscore(b, s);
            return 1;
        }
    }
    return 0;
}
int check_if_ate(Point h, Food f)
{
    for (int i = 0; i < f.number; i++)
    {
        if (h.x == (f.coord + i)->x && h.y == (f.coord + i)->y)
            return i;
    }
    return -1;
}
void gyvate_juda(Snake s, Box b, Food f)
{
    // Judëjimo kryptis
    Point last_dir = s.dir;

    // Paskutines galvos koordinates
    Point last_head = s.head;

    int ate = -1;;
    while (1)
    {
        // mygtuko nuskaitymas
        if (read_key(&s.dir, s, b, f))
            return;

        // I priesinga puse negalima pasukt
        if ((s.dir.x == -1 && last_dir.x == 1) || (s.dir.x == 1 && last_dir.x == -1))
            s.dir.x = last_dir.x;

        else if ((s.dir.y == 1 && last_dir.y == -1) || (s.dir.y == -1 && last_dir.y == 1))
            s.dir.y = last_dir.y;

        // Atnaujinam galvos koordinates
        s.head.x += s.dir.x;
        s.head.y += s.dir.y;

        // Issaugom dabartine krypti
        last_dir = s.dir;

        // Gal atsitrenke i siena?
        if (hits_wall(s.head, b))
        {
            LOG_PRINT("Gyvate atsitrenke i siena.");
            save_options(opt);
            save_highscore(b, s);
            return;
        }

        // Atnaujinam X pozicijà: pasiekus ekrano galà, perðokam á prieðingà pusæ
        if (s.head.x < 1 + b.left_space)
            s.head.x = opt.box_width - 2 + b.left_space;

        else if (s.head.x > opt.box_width - 2 + b.left_space)
            s.head.x = 1 + b.left_space;

        // Atnaujinam Y pozicijà: pasiekus ekrano galà, perðokam á prieðingà pusæ
        else if (s.head.y < 1 + b.above_space)
            s.head.y = opt.box_height - 2 + b.above_space;

        else if (s.head.y > opt.box_height - 2 + b.above_space)
            s.head.y = 1 + b.above_space;

        // Tikrinam, ar suvalge maista
        ate = check_if_ate(s.head, f);

        // Tikrinam, ar atsitrenke i save arba i siena
        if (ate == -1 && (bites_itself(s) || hits_wall(s.head, b)))
        {
            LOG_PRINT("Gyvate atsitrenke i siena arba i save.");
            save_options(opt);
            save_highscore(b, s);
            return;
        }

        // Nupiesiam galva naujose koordinatese
        con_set_color(COLOR_RED, COLOR_RED);
        con_set_pos(s.head.x, s.head.y);
        printf("#");
        fflush(stdout);

        if (ate != -1)
        {
            renew_score(++s.score, opt.box_height + b.above_space);
            s.segment = (Point*) realloc(s.segment, sizeof(Point) * (++s.number - 1));
            LOG_PRINT("Gyvate suvalge maista (galvos koord. (%d, %d)).", (f.coord + ate)->x, (f.coord + ate)->y);
        }
        else
        {
            // Istriniam uodegos gala
            con_set_color(COLOR_BLACK, COLOR_GRAY);
            con_set_pos((s.segment + s.number - 2)->x, (s.segment + s.number - 2)->y);
            printf(" ");
        }

        // Atnaujinam gyvates gabalu pozicijas
        for (int i = s.number - 2; i > 0; i--)
        {
            (s.segment + i)->x = (s.segment + i - 1)->x;
            (s.segment + i)->y = (s.segment + i - 1)->y;
        }
        // Antro segmento koordinates yra senosios galvos koordinates
        *s.segment = last_head;

        // Issaugom sena galva
        last_head = s.head;

        // Sukuriam nauja maista
        if (ate != -1)
            new_food(ate, f, s, b);

        // Pamiegam kazkiek laiko
        con_sleep(s.speed);
    }
}
void start_snake_game(Snake s, int load, Box b, Food f)
{
    LOG_PRINT("Prasideda zaidimas.");
    // Istrinam viska
    clear_all();

    // Parodom informacija
    show_info();

    // Sienu braizymas
    draw_box(&b);

    // Gyvates nupiesimas
    snake_draw(s);

    // Statistikos atnaujinimas
    renew_score(s.score, opt.box_height + b.above_space);

    // Ar reikia reikalauti w a s d mygtuku paspaudimo?
    if (load)
    {
        for (int i = 0; i < f.number; i++)
        {
            con_set_color(COLOR_GREEN, COLOR_GREEN);
            con_set_pos((f.coord + i)->x, (f.coord + i)->y);
            printf("#");
            fflush(stdout);
        }
        con_set_color(COLOR_BLACK, COLOR_GRAY);
        con_set_pos(0, 0);
        printf(" Spauskite a, w, s, d.");
        int key;
        while ((key = getch()) != 'a' && key != 'w' && key != 's' && key != 'd');
        clear_menu(0);
        show_info();
    }
    else
    {
        for (int i = 0; i < f.number; i++)
            new_food(i, f, s, b);
    }

    // Veiksmas
    gyvate_juda(s, b, f);
}
int load_game(Snake snake, Box box, Food food)
{
        clear_all();

        FILE *f;
        if (!(f = fopen("saved.txt", "rb")))
        {
            con_set_pos(0, 20);
            printf(" Nera issaugotas\n joks zaidimas!");
            LOG_PRINT("Zaidimo uzkrauti nepavyko.");
            return 0;
        }

        fread(&snake.dir, 1, sizeof(Point), f);
        fread(&snake.head, 1, sizeof(Point), f);
        fread(&snake.number, 1, sizeof(int), f);
        fread(&snake.score, 1, sizeof(int), f);
        fread(&snake.speed, 1, sizeof(int), f);
        snake.segment = (Point*) malloc(sizeof(Point) * (snake.number - 1));
        fread(snake.segment, snake.number - 1, sizeof(Point), f);

        fread(&box.stage, 1, sizeof(int), f);
        fread(&box.left_space, 1, sizeof(int), f);
        fread(&box.above_space, 1, sizeof(int), f);

        fread(&food.number, 1, sizeof(int), f);
        food.coord = (Point*) malloc(sizeof(Point) * food.number);
        fread(food.coord, food.number, sizeof(Point), f);

        fread(&opt.box_width, 1, sizeof(int), f);
        fread(&opt.box_height, 1, sizeof(int), f);

        fclose(f);

        // Labirinto nuskaitymas
        if (box.stage == 0 || !open_stage(box.stage, &box, &snake, 1))
        {
            box.height = HEIGHT;
            box.width = WIDTH;
            box.stage = 0;
            box.walls_number = 0;
        }
        LOG_PRINT("Zaidimas uzkrautas. Labirintas: %d.", box.stage);
        start_snake_game(snake, 1, box, food);
        return 1;
}
void init_new_game(Snake snake, Box box, Food food, int stage)
{
    free(snake.segment);
    free(box.wall_start);
    free(box.wall_end);
    free(food.coord);

    box.stage = stage;

    opt.box_width = opt.box_width_changed;
    opt.box_height = opt.box_height_changed;

    if (box.stage == 0 || !open_stage(box.stage, &box, &snake, 0))
    {
        box.width = WIDTH;
        box.height = HEIGHT;
        box.above_space = ABOVE_SPACE;
        box.left_space = LEFT_SPACE;
        box.stage = 0;
        box.walls_number = 0;
        box.wall_start = NULL;
        box.wall_end = NULL;

        snake.head.x = SNAKE_HEAD_X;
        snake.head.y = SNAKE_HEAD_Y;
    }

    box.above_space = ABOVE_SPACE;
    box.left_space = LEFT_SPACE;

    snake.head.x *= (((float) opt.box_width) / box.width);
    snake.head.y *= (((float) opt.box_height) / box.height);
    snake.head.x += box.left_space;
    snake.head.y += box.above_space;
    snake.number = SNAKE_LEN;

    snake.segment = (Point*) malloc(sizeof(Point) * (snake.number - 1));
    for (int i = 0; i < snake.number - 1; i++)
    {
        (snake.segment + i)->x = snake.head.x - 1 - i;
        (snake.segment + i)->y = snake.head.y;
    }

    snake.dir.x = 1;
    snake.dir.y = 0;
    snake.score = 0;
    snake.speed = opt.snake_speed;

    food.number = opt.food_number;
    food.coord = (Point*) malloc(sizeof(Point) * food.number);

    for (int i = 0; i < food.number; i++)
    {
        (food.coord + i)->x = 0;
        (food.coord + i)->y = 0;
    }

    start_snake_game(snake, 0, box, food);
}
int show_newgame_menu(Snake snake, Box box, Food food)
{
    LOG_PRINT("Paspaustas naujo zaidimo meniu");
    clear_menu(1);
    con_set_pos(0, 0);

    printf (" Pasirinkite labirinta:\n\n");
    printf (" 1 - Be labirintu\n");
    printf (" 2 - Labirintas 1\n");
    printf (" 3 - Labirintas 2\n");
    printf (" 4 - Labirintas 3\n");

    printf ("\n q - Grizti\n");

    int key;
    while ((key = getch()))
    {
        if (key >= '1' && key <= '4')
        {
            init_new_game(snake, box, food, key - 1);
            return 1;
        }
        else if (key == 'q')
            return 0;
    }
    return 0;
}
void replace_char(char *str, char c1, char c2)
{
    int i = 0;
    while (str[i] != 0)
    {
        if (str[i] == c1)
            str[i] = c2;
        i++;
    }
}
int check_name(char *str1, char *str2)
{
    int i = 0;
    while (str1[i] != ' ' && str1[i] != 0)
    {
        if (str1[i] != str2[i++])
            return 0;
    }
    return 1;
}
void save_highscore(Box b, Snake s)
{
    char name[20], info[50];
    con_set_pos(0, opt.box_height + b.above_space + 2);
    printf("Iveskite savo varda: ");
    scanf("%s", name);

    replace_char(name, ' ', '\\');
    FILE *f;
    if (!(f = fopen("pasiekimai.txt", "r+")))
        return;

    assert(check_name("55", "5 5") == 0);
    assert(check_name("55", "55 5") == 1);
    assert(check_name("", " 5") == 1);
    assert(check_name("asd", "asd") == 1);
    assert(check_name("asd", "") == 0);

    fpos_t pos;
    int i;
    while(1)
    //for (i = 0; i < 5; i++)
    {
        if (feof(f))
        {
            fprintf(f, "%s %d %d\n", name, b.stage, s.score);
            break;
        }
        fgetpos(f, &pos);
        fgets(info, 50, f);
        fscanf(f, "%s %d %d", info, i, i);
        if (check_name(info, name))
        {
            fsetpos(f, &pos);
            //fseek(f, -1, SEEK_CUR);
            fprintf(f, "%s %d %d\n", name, b.stage, s.score);
            //fseek(f, 0, SEEK_CUR);
        }

    }
    fclose(f);
}
void the_end_of_program()
{
    LOG_PRINT("Zaidimo pabaiga.");
}
// load_options buvo
int main(int argc, char** argv)
{
    // Rand
    srand(time(NULL));

    atexit(the_end_of_program);

    Snake snake;
    Box box;
    Food food;

    load_options(&opt);

    // Meniu rodymas
    clear_all();
    show_main_menu(snake, box, food);
}
