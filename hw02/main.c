#include <stdio.h>
#include <math.h>

int genTrian(int min, int max){
    int res = 0;
    for (int a = min; a <= max; a++)
        for (int b = a; b <= max; b++) {
            int c_squared = a * a + b * b;
            int c = (int)sqrt(c_squared);
            if (c * c == c_squared && c <= max){
                res++;
                printf("* %d %d %d\n", a, b, c);
            }
        }

    return res;
}

int gcd(int a, int b){
    int remainder = a % b;
    while (remainder) {
        a = b; b = remainder; remainder = a % b;
    }
    return b;
}

int Pithagorean_triple(int min, int max){
    int res = 0;
    for (int m = 1; m * m <= max; m++)
        for (int n = 1; n < m; n++)
            if ((m - n) % 2 == 1 && gcd(m, n) == 1) {
                int a = m * m - n * n;
                int b = 2 * m * n;
                int c = m * m + n * n;

                for (int k = 1; k * c <= max; k++) {
                    int ka = k * a;
                    int kb = k * b;
                    int kc = k * c;

                    if (ka >= min && kb >= min && kc >= min && ka <= max && kb <= max && kc <= max)
                        res++;

                }
            }
    return res;
}

int main() {
    printf("Problems:\n");
    int lo, hi;
    char ch;
    while(scanf(" %c", &ch) == 1){
        char last;
        int res;
        if(ch == '?' || ch == '#'){
            if(scanf(" < %d ; %d %c", &lo, &hi, &last) != 3 || last != '>' || lo > hi || lo <= 0 || hi < 0){
                printf("Invalid input.\n");
                break;
            }

            if(ch == '?')
                res = genTrian(lo, hi);
            else
                res = Pithagorean_triple(lo, hi);

            printf("Total: %d\n", res);
        }
        else{
            printf("Invalid input.\n");
            break;
        }
    }

    return 0;
}
