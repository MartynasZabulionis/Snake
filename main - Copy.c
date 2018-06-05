#include "con_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct
{
	int x, y;
} Point;

typedef struct
{
	Point *segment; // Point *head;
	int number;
	int max_size;
} Snake;

// Pieðia staciakampá
// x, y - virðutinio kairio kampo koordinatës
// w, h - staèiakampio plotis (width) ir aukðtis (height)
const int height = 25, width = 50;

int snake_len = 15, bite_x, bite_y;

void draw_box(int x, int y, int w, int h)
{
    int i;
    for (i = 0; i < w; i++)
    {
        con_set_pos(x + i, y);
        printf("#");

        con_set_pos(x + i, y + h - 1);
        printf("#");
    }

    for (i = 0; i < h; i++)
    {
        con_set_pos(x, y + i);
        printf("#");

        con_set_pos(x + w - 1, y + i);
        printf("#");
    }
}
void new_bite(int lastx, int lasty)
{
    // Iðtriniam senà kasni
    if (lastx != 0)
    {
        con_set_color(COLOR_BLACK, COLOR_GRAY);
        con_set_pos(lastx, lasty);
        printf(" ");
    }

    // Nupieðiam naujà kasni
    con_set_color(COLOR_GREEN, COLOR_GREEN);
    con_set_pos((bite_x = rand() % (width - 2) + 1 + 15), (bite_y = rand() % (height - 2) + 1));
    printf("#");
    fflush(stdout);
}
int bites_itself(int *pos_x, int *pos_y, int snake_len)
{
    int i;
    for (i = 0; i < snake_len - 3; i++)
    {
        if (pos_x[snake_len-1] == pos_x[i] && pos_y[snake_len-1] == pos_y[i])
            return 1;
    }
    return 0;
}
void game_over()
{

}
int main(int argc, char** argv)
{
    // Creating snake
    Snake *snake;

    srand(time(NULL));


    // Paslepia ávedamus simbolius
    con_show_echo(0);

    // Paslepia cursoriø
    con_show_cursor(0);

    // Sena pozicija
    int old_x[1000], old_y[1000];

    // Judëjimo kryptis
    int dir_x = 1, dir_y = 0, last_dir_x = 1, last_dir_y = 0;

    int i;

    // Dabartinë pozicija
    int pos_x[1000], pos_y[1000];

    for (i = 0; i < snake_len; i++)
    {
        pos_x[i] = 35+i;
        old_x[i] = 35+i;

        pos_y[i] = 10;
        old_y[i] = 10;
    }

    // Pieðiam rëmelá
    con_clear();
    draw_box(15, 0, width, height);

    new_bite(bite_x, bite_y);

    while (1)
    {
        int key = 0;
        int ate = 0;
        int count = 0;
        int c1;
        int c2;

        // Nuskaitom visus paspaustus klaviðus ið klaviatûros
        while (key = con_read_key())
        {
            if (key == 'w')
            {
                dir_x = 0;
                dir_y = -1;
            }
            else if (key == 's')
            {
                dir_x = 0;
                dir_y = 1;
            }
            else if (key == 'a')
            {
                dir_x = -1;
                dir_y = 0;
            }
            else if (key == 'd')
            {
                dir_x = 1;
                dir_y = 0;
            }
            else if (key == 'q')
            {
                // Jei paspausta q, tai iðeinam ið programos
                return 0;
            }
            if (++count == 1)
                c1 = key;
            else
                c2 = key;
        }
        if (count >= 2)
        {
            if (c1 == 'a' && c2 == 'w' && last_dir_x == 1)
            {
                dir_x = -1;
                dir_y = -1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                dir_y = 0;
            }
            else if (c1 == 'a' && c2 == 's' && last_dir_x == 1)
            {
                dir_x = -1;
                dir_y = 1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_y = 0;
            }
            else if (c1 == 'd' && c2 == 'w' && last_dir_x == -1)
            {
                dir_x = 1;
                dir_y = -1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_y = 0;
            }
            else if (c1 == 'd' && c2 == 's' && last_dir_x == -1)
            {
                dir_x = 1;
                dir_y = 1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_y = 0;
            }


            else if (c1 == 'w' && c2 == 'a' && last_dir_y == 1)
            {
                dir_x = -1;
                dir_y = -1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_x = 0;
            }
            else if (c1 == 'w' && c2 == 'd' && last_dir_y == 1)
            {
                dir_x = 1;
                dir_y = -1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_x = 0;
            }
            else if (c1 == 's' && c2 == 'a' && last_dir_y == -1)
            {
                dir_x = -1;
                dir_y = 1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_x = 0;
            }
            else if (c1 == 's' && c2 == 'd' && last_dir_y == -1)
            {
                dir_x = 1;
                dir_y = 1;
        pos_x[snake_len - 1] += dir_x;
        pos_y[snake_len - 1] += dir_y;
                        dir_x = 0;
            }
            else {            // Atnaujinam X pozicijà: pasiekus ekrano galà, perðokam á prieðingà pusæ
            if (dir_x == 1 && last_dir_x == -1)
                dir_x = -1;

            else if (dir_x == -1 && last_dir_x == 1)
                dir_x = 1;

            if (dir_y == 1 && last_dir_y == -1)
                dir_y = -1;

            else if (dir_y == -1 && last_dir_y == 1)
                dir_y = 1;

            pos_x[snake_len - 1] += dir_x;
            pos_y[snake_len - 1] += dir_y;}

        }
        else
        {
            // Atnaujinam X pozicijà: pasiekus ekrano galà, perðokam á prieðingà pusæ
            if (dir_x == 1 && last_dir_x == -1)
                dir_x = -1;

            else if (dir_x == -1 && last_dir_x == 1)
                dir_x = 1;

            if (dir_y == 1 && last_dir_y == -1)
                dir_y = -1;

            else if (dir_y == -1 && last_dir_y == 1)
                dir_y = 1;

            pos_x[snake_len - 1] += dir_x;
            pos_y[snake_len - 1] += dir_y;
        }

        last_dir_x = dir_x;
        last_dir_y = dir_y;

        if (pos_x[snake_len - 1] == bite_x && pos_y[snake_len - 1] == bite_y)
        {
            //ate = 1;
            new_bite(bite_x, bite_y);
        }

        else if (bites_itself(pos_x, pos_y, snake_len))
        {
            game_over();
            break;
        }

        else if (pos_x[snake_len - 1] < 1 + 15)
            pos_x[snake_len - 1] = width - 2 + 15;

        else if (pos_x[snake_len - 1] > width - 2 + 15)
            pos_x[snake_len - 1] = 1 + 15;

        // Atnaujinam Y pozicijà: pasiekus ekrano galà, perðokam á prieðingà pusæ
        if (pos_y[snake_len - 1] < 1)
            pos_y[snake_len - 1] = height - 2;

        else if (pos_y[snake_len - 1] > height - 2)
            pos_y[snake_len - 1] = 1;

        // Nupieðiam naujà
        con_set_color(COLOR_RED, COLOR_RED);
        con_set_pos(pos_x[snake_len - 1], pos_y[snake_len - 1]);
        printf("#");
        fflush(stdout);

        if (ate)
        {
            snake_len++;
            for (i = snake_len - 1; i > 0; i--)
            {
                pos_x[i] = pos_x[i-1];
                pos_y[i] = pos_y[i-1];
            }

            // Iðsaugom senas koordinates
            for (i = 0; i < snake_len; i++)
            {
                old_x[i] = pos_x[i];
                old_y[i] = pos_y[i];
            }

            // Pamiegam 0.08 sekundës
            con_sleep(0.08);
            continue;
        }

        for (i = snake_len - 2; i >= 0; i--)
        {
            pos_x[i] = old_x[i+1];
            pos_y[i] = old_y[i+1];

/*          con_set_color(COLOR_RED, COLOR_RED);
            con_set_pos(pos_x[i], pos_y[i]);
            printf("#");
            fflush(stdout); */
        }
        // Iðtriniam senà simbolá
        con_set_color(COLOR_BLACK, COLOR_GRAY);
        con_set_pos(old_x[0], old_y[0]);
        printf(" ");

        // Iðsaugom senas koordinates
        for (i = 0; i < snake_len; i++)
        {
            old_x[i] = pos_x[i];
            old_y[i] = pos_y[i];
        }

        // Pamiegam 0.08 sekundës
        con_sleep(0.08);
    }
}
