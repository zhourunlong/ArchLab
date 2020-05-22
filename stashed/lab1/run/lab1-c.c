#define MAXN 512
#define MAXM 512

#define WINDOW_N 3 
#define WINDOW_M 3
#define WINDOW_SIZE ( WINDOW_N * WINDOW_M )

unsigned int n = 0,m = 0;

typedef unsigned short pixel;

pixel img[MAXN][MAXM] = {};

#include "mystdio.h"

int main() {
    int i = 0, j = 0;
    n = _scanf_num();
    m = _scanf_num();
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            img[i][j] = _scanf_num();
    
    _printf_num(n);
    _printf_char(' ');
    _printf_num(m);
    _printf_char('\n');
    pixel s = 0;
    for (j = 0; j < m; j++) {
        _printf_num(img[0][j]);
        _printf_char(' ');
    }
    _printf_char('\n');
    for (i = 1; i < n - 1; i++) {
        _printf_num(img[i][0]);
        _printf_char(' ');
        for (j = 1; j < m - 1; j++) {
            s = (img[i - 1][j - 1] + img[i - 1][j + 1] + img[i + 1][j - 1] + img[i + 1][j + 1]
                + 2 * (img[i - 1][j] + img[i][j - 1] + img[i][j + 1] + img[i + 1][j])
                + 4 * img[i][j])
                / 16;
            _printf_num(s);
            _printf_char(' ');
        }
        _printf_num(img[i][m - 1]);
        _printf_char(' ');
        _printf_char('\n');
    }
    for (j = 0; j < m; j++) {
        _printf_num(img[n - 1][j]);
        _printf_char(' ');
    }
    _printf_char('\n');
    return 0;
}

