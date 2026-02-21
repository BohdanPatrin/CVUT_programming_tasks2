#include <stdio.h>
#include <stdbool.h>

struct Vehicle{
    int from, to, cap, rent;
};

bool correct_info(struct Vehicle veh){
    return veh.from >= 0 && veh.to >= 0 && veh.rent > 0 && veh.cap > 0 && veh.from <= veh.to;
}

void calculate_order(int vehicle_count, int start, long long amount, struct Vehicle *transports, int oldest_time){
    int end = start;
    long long cost = 0;
    for(int i = start; i <= oldest_time+1; i++){
        if(amount <= 0) {
            printf("Finish: %d, cost: %lld\n", end, cost);
            return;
        }
        end = i;
        for(int j= 0; j < vehicle_count; j++)
            if(transports[j].from <= i && transports[j].to >= i){
             amount -= transports[j].cap;
             cost += transports[j].rent;
            }
    }
    printf("Cargo too big, not enough capacity.\n");
}

int main(void) {
    int vehicle_count = 0;
    struct Vehicle list[100000];
    int oldest_time = 0;
    printf("Cargo delivery:\n");
    char ch;
    if (scanf(" %c", &ch) != 1 || ch != '{') {
        printf("Invalid input.\n");
        return 1;
    }
    while (1) {
        if (vehicle_count >= 100000) {
            printf("Invalid input.\n");
            return 1;
        }
        char last_bracket;
        if ((scanf(" [ %d - %d , %d , %d %c", &list[vehicle_count].from,  &list[vehicle_count].to,  &list[vehicle_count].cap,  &list[vehicle_count].rent, &last_bracket) != 5)) {
            printf("Invalid input.\n");
            return 1;
        }
        if(!correct_info(list[vehicle_count]) == 1 || last_bracket != ']'){
            printf("Invalid input.\n");
            return 1;
        }
        if(list[vehicle_count].to > oldest_time)
            oldest_time = list[vehicle_count].to;

        vehicle_count++;

        if (scanf(" %c", &ch) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        if (ch == '}') {
            break;
        } else if (ch != ',') {
            printf("Invalid input.\n");
            return 1;
        }
    }
    printf("Cargo:\n");
    int start;
    long long amount;
    while (1) {
        int result = scanf("%d %lld", &start, &amount);
        if (result == 2 && start >= 0 && amount > 0)
            calculate_order(vehicle_count, start, amount, list, oldest_time);
        else if (result == EOF)
            break;
        else {
            printf("Invalid input.\n");
            return 1;
        }
    }
    return 0;
}
