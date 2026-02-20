#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

struct Point{
    double x;
    double y;
};

bool compare(double d1, double d2){
    return (fabsl(d1 - d2) <= 100*DBL_EPSILON*(fabsl(d1)+ fabsl(d2)));
}

bool threePoints_oneLine(struct Point A,struct Point B, struct Point C){
    return compare(((B.y - A.y)*(C.x - B.x)), ((C.y - B.y)*(B.x - A.x)));
}

struct Point fourth_p(struct Point A,struct Point B, struct Point C){
    struct Point new_p;
    new_p.x = B.x + C.x - A.x;
    new_p.y = B.y + C.y - A.y;
    return new_p;
}

double dist(struct Point a, struct Point b) {
    return (sqrt(powl(a.x - b.x, 2) + powl(a.y - b.y, 2)));
}

const char* result(struct Point a, struct Point b, struct Point c, struct Point d){
    double ab = dist(a, b);
    double bc = dist(b, c);
    double ac = dist(a, c);
    double bd = dist(b, d);

    if(compare(ac, bd)) {
        if (compare(ab, bc))
            return "square";
        return "rectangle";
    }
    else if(compare(ab, bc))
        return "rhombus";
    else
        return "parallelogram";
}

int main() {
    struct Point points[3];
    char end_brac[3];
    char comma[3];
    char name = 'A';
    for(int i = 0; i < 3; i++){
        printf("Point %c:\n", (name+i));
        if(scanf(" [%lf %c%lf %c", &points[i].x, &comma[i], &points[i].y, &end_brac[i]) != 4 || end_brac[i] != ']' || comma[i] != ','){
            printf("Invalid input.\n");
            return 0;
        }
    }

    if(threePoints_oneLine(points[0], points[1], points[2])) {
        printf("Parallelograms do not exist.\n");
        return 0;
    }

    struct Point a = fourth_p(points[0], points[1],points[2] );
    struct Point b = fourth_p(points[1], points[0],points[2] );
    struct Point c = fourth_p(points[2], points[0],points[1] );

    printf("A': [%.20g,%.20g], %s\n", a.x, a.y, result(points[0], points[1], a, points[2]));
    printf("B': [%.20g,%.20g], %s\n", b.x, b.y, result(points[0], points[1], points[2], b));
    printf("C': [%.20g,%.20g], %s\n", c.x, c.y, result(points[0], c, points[1], points[2]));
    return 0;
}
