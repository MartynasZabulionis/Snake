#ifndef CON_LIB_H
#define CON_LIB_H

// Spalvos
#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_ORANGE    3
#define COLOR_BLUE      4
#define COLOR_MAGENTA   5
#define COLOR_CYAN      6
#define COLOR_GRAY      7

// Iðvalo ekranà
void con_clear();

// Nuskaito vienà klaviðà. 
// Graþina 0, jei nëra ko daugiau skaityti
int con_read_key();

// Nustato fono ir teksto spalvà
// * bg - fono spalva (COLOR_*)
// * fg - teksto spalva (COLOR_*)
void con_set_color(int bg, int fg);

// Nustato dabartinæ iðvedimo pozicijà. x, y - koordinatës:
// * virðutinis kairys terminalo kampas yra (0, 0)
// * x-ai didëja ið kairës á deðinæ
// * y-ai didëja ið virðaus á apaèià
void con_set_pos(int x, int y);

// Nustato cursoriaus rodymo rëþimà.
// * Jei show == 0, cursoriø paslepia
// * Jei show == 1, cursoriø rodo
void con_show_cursor(int show);

// Padaro, kad nesimatytø ávedamø simboliø (tik LINUX os)
// * Jei show == 0, tai ávedamø simboliø neatkartoja á ekranà
// * Jei show == 1, tai ávedamus simbolius rodo 
// * Ant Windows nieko nedaro 
void con_show_echo(int show);

// Miega nurodytà sekundþiø skaièiø.
// * seconds turi bûti intervale [0.01; 100.0]
void con_sleep(float seconds);

#endif // CON_LIB_H