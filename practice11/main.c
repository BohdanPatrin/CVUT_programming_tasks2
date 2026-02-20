#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int recursion(int nominal, int coins[], int amount, int note[]) {
    //edge cases
    if (nominal == 0)  return 0;
    if (note[nominal] != -1) return note[nominal];

    int res = INT_MAX;
    for (int i = 0; i < amount; i++)
        if (coins[i] <= nominal) {
            int interMedRes = recursion(nominal - coins[i], coins, amount, note);
            if (interMedRes != INT_MAX && interMedRes + 1 < res) res = interMedRes + 1;
        }
    note[nominal] = res;
    return res;
}

int main(){
    int input = 0;
    int sizeCoins = 20;
    int* coins = (int*)malloc(sizeCoins * sizeof(int));
    int amount = 0;
    int nominal = 0;
    printf("Coins:\n");
    while((input = scanf(" %d ", &nominal)) == 1){
        if(nominal < 0){
            printf("Invalid input.\n");
            return 0;
        }
        else if(nominal == 0) break; //end of input

        coins[amount] = nominal;
        amount++;
        if(amount + 2 >= sizeCoins){ //resize array of coins
            sizeCoins *= 2;
            coins = (int*)realloc(coins, sizeCoins * sizeof(int));
        }
    }
    if(input == EOF || amount == 0 || (nominal != 0 && input == 0)){
        printf("Invalid input.\n");
        return 0;
    }
    printf("Amounts:\n");
    while((input = scanf(" %d ", &nominal)) == 1){
        if(nominal < 0){
            printf("Invalid input.\n");
            return 0;
        }
        int* note = (int*)malloc(sizeof(int) * (nominal + 1));
        for (int i = 0; i <= nominal; i++) note[i] = -1; //make elements initialized
        int res = recursion(nominal, coins, amount, note); //compute how many coins must be used to pay

        if(res == INT_MAX){
            printf("= no solution\n");
            continue;
        }
        printf("= %d\n", res);
    }
    if(input != EOF && input == 0){
        printf("Invalid input.\n");
        return 0;
    }

    return 0;
}