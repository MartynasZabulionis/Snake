#include <stdio.h>
#include <stdlib.h>
#include "con_lib.h"
#include "save_to_file.h"

void save_game(Snake s, Box b, Food f, Options opt)
{
    FILE *file;

    if (!(file = fopen("saved.txt", "wb")))
    {
        con_set_pos(0, 20);
        printf(" Zaidimas neissaugotas!");
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
        return;
    }
}
int open_stage(int stage, Box *box, Snake *snake, int load)
{
    char fname[12] = "stage_x.txt";
    fname[6] = (char) stage;
    FILE *f;
    f = fopen(fname, "r");
    if (!f)
        return 0;

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

    return 1;
}
void load_options(Options opt)
{
    FILE *f;
    if ((f = fopen("options.txt", "rb")))
    {
        fread(&opt.snake_speed, 1, sizeof(float), f);
        fread(&opt.food_number, 1, sizeof(int), f);
        fread(&opt.box_width_changed, 1, sizeof(int), f);
        fread(&opt.box_height_changed, 1, sizeof(int), f);
        fclose(f);
        return;
    }
    opt.snake_speed = SNAKE_SPEED;
    opt.food_number = FOOD_NUMBER;
    opt.box_width_changed = WIDTH;
    opt.box_height_changed = HEIGHT;
}
