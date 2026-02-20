#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(void) {
    printf("Price, queries:\n");
    char ch;
    int count = 0;
    int capacity = 100;
    int start, end;
    int* prices = (int *)malloc(capacity *sizeof(int));
    while(scanf(" %c", &ch) == 1) {
        if (ch == '+') {
           if(count >= capacity){
               capacity += (int)capacity*0.1;
               prices = (int *)realloc(prices, capacity * sizeof(int));
           }
           int return_value = scanf(" %d", &prices[count]);
           if(return_value != 1 || prices[count] < 0){
               printf("Invalid input.\n");
               free(prices);
               return 1;
           }
               count++;
        }
        else if(ch == '?'){
            int return_value = scanf("%d %d", &start, &end);
            if(return_value != 2 || start > end || end >= count || start < 0 || end < 0){
                printf("Invalid input.\n");
                free(prices);
                return 1;
            }
            else if(start == end)
                printf("Max profit: N/A\n"
                              "Max loss: N/A\n");
            else{
                int max_price = prices[start], min_price = prices[start], max_profit = INT_MIN, max_loss = INT_MAX,
                buy_date_p = start, sell_date_p = start, buy_date_l = start, sell_date_l = start,
                min_price_index = start, max_price_index = start;

                for (int i = start + 1; i <= end; i++) {
                    int loss = prices[i] - max_price;
                    int profit = prices[i] - min_price;

                    if (loss < max_loss ||
                        (loss == max_loss && max_price_index < buy_date_l) ||
                        (loss == max_loss && max_price_index == buy_date_l && i < sell_date_l)) {
                        max_loss = loss;
                        buy_date_l = max_price_index;
                        sell_date_l = i;
                    }

                    if (profit > max_profit ||
                        (profit == max_profit && min_price_index < buy_date_p) ||
                        (profit == max_profit && min_price_index == buy_date_p && i < sell_date_p)) {
                        max_profit = profit;
                        buy_date_p = min_price_index;
                        sell_date_p = i;
                    }

                    if (prices[i] > max_price) {
                        max_price = prices[i];
                        max_price_index = i;
                    }
                    if (prices[i] < min_price) {
                        min_price = prices[i];
                        min_price_index = i;
                    }
                }

                if (max_profit <= 0)
                    printf("Max profit: N/A\n");
                else
                    printf("Max profit: %d (%d - %d)\n", max_profit, buy_date_p, sell_date_p);

                if (max_loss >= 0)
                    printf("Max loss: N/A\n");
                else
                    printf("Max loss: %d (%d - %d)\n", -max_loss, buy_date_l, sell_date_l);

            }
        }
        else {
            printf("Invalid input.\n");
            free(prices);
            return 1;
        }
    }
    free(prices);
    return 0;
}
