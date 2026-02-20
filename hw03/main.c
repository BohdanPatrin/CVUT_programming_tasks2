#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

constexpr unsigned DOW_MON      = 0b0000'0001;
constexpr unsigned DOW_TUE      = 0b0000'0010;
constexpr unsigned DOW_WED      = 0b0000'0100;
constexpr unsigned DOW_THU      = 0b0000'1000;
constexpr unsigned DOW_FRI      = 0b0001'0000;
constexpr unsigned DOW_SAT      = 0b0010'0000;
constexpr unsigned DOW_SUN      = 0b0100'0000;
constexpr unsigned DOW_WORKDAYS = DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI;
constexpr unsigned DOW_WEEKEND  = DOW_SAT | DOW_SUN;
constexpr unsigned DOW_ALL      = DOW_WORKDAYS | DOW_WEEKEND;

typedef struct TDate
{
  unsigned m_Year;
  unsigned m_Month;
  unsigned m_Day;
} TDATE;

TDATE makeDate ( unsigned y,
                 unsigned m,
                 unsigned d )
{
  TDATE res = { y, m, d };
  return res;
}
#endif /* __PROGTEST__ */
bool isLeapYear(unsigned year) { //check if year is leap
    if ((year % 4 == 0 && year % 100 != 0) ||((year % 400 == 0) && (year % 4000 != 0)))
        return true;

    return false;
}

unsigned int days_in_month(unsigned month, unsigned int year) { //gives how many days are in month
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

TDATE plusDays(struct TDate d, unsigned days){
    while (days > 0) {
        unsigned int days_in_current_month = days_in_month(d.m_Month, d.m_Year);

        if (d.m_Day + days <= days_in_current_month) {
            d.m_Day += days;
            days = 0;
        }
        else {
            days -= (days_in_current_month - d.m_Day + 1);
            d.m_Day = 1;
            if (++d.m_Month > 12) {
                d.m_Month = 1;
                d.m_Year++;
            }
        }
    }
    return d;
}

bool isValidDate(struct TDate d) { //check if date is valid
    if (d.m_Month < 1 || d.m_Month > 12 || d.m_Day < 1 || d.m_Day > 31)
        return false;

    return d.m_Day <= days_in_month(d.m_Month, d.m_Year);
}

bool isLater(struct TDate d1, struct TDate d2){ //check if d1 is later than d2
    if(d1.m_Year > d2.m_Year)
        return false;
    else if(d1.m_Year == d2.m_Year){
        if(d1.m_Month > d2.m_Month)
            return false;
        else if(d1.m_Month == d2.m_Month)
            if(d1.m_Day > d2.m_Day)
                return false;
    }

    return true;
}

unsigned dayOfTheWeek(TDATE date) //gives what day of the week is based on date
{
    date.m_Day += date.m_Month < 3 ? date.m_Year-- : date.m_Year - 2;
    unsigned dayOfWeek = ( 23 * date.m_Month / 9 + date.m_Day + 4 + date.m_Year / 4 - date.m_Year / 100 + date.m_Year / 400 - date.m_Year / 4000) % 7;
    return dayOfWeek; // 0 -> Sunday, 1 -> Monday, ..., 6 -> Saturday
}

unsigned int days_from_start_of_year(TDATE date) { //counts how many days passed since the beginning of the year to the date
    unsigned int days = date.m_Day;
    for (unsigned int m = 1; m < date.m_Month; m++) {
        days += days_in_month(m, date.m_Year);
    }
    return days;
}

unsigned int days_bettween_year(unsigned int year1,unsigned year2) {
    if(year1 == year2)
        return 0;

    unsigned day_sum = (((year2 - year1)) * 365) + (((year2 - year1)+2) /4);
    if ((year1 % 100) == 0) {
        day_sum -= 1;
    }
    for (unsigned i = year1; i < year2;) {
        i += (100 - (i % 100));
        if (i % 4000 == 0) {
            day_sum -= 1;
            continue;
        }
        if (i % 400 == 0) {
            continue;
        }
        day_sum -= 1;
    }
    return day_sum+2;
}

unsigned countDays(TDATE start, TDATE end){ //counts how many days are between two days
    if(start.m_Year == end.m_Year)
        return (days_from_start_of_year(end) - days_from_start_of_year(start)+1);
    else
        if(start.m_Day == 1 && start.m_Month == 1)
           return days_bettween_year(start.m_Year, end.m_Year) + days_from_start_of_year(end) -2;
        else{
            return days_bettween_year(start.m_Year+1, end.m_Year) + days_from_start_of_year(end) -1 + ((isLeapYear(start.m_Year) ? (366 - days_from_start_of_year(start)) : (365 - days_from_start_of_year(start) +2)));
        }
}

long long countConnections ( TDATE     from,
                             TDATE     to,
                             unsigned  perWorkDay,
                             unsigned  dowMask ) {
    if (!isValidDate(from) || !isValidDate(to) || !isLater(from, to)) //incorrect input
        return -1;

    long long res = 0;
    unsigned days = countDays(from, to); //compute how many days are in interval inclusively
    unsigned weeks = days / 7; // weeks in interval
    unsigned remaining_days = days % 7; // days which are not enough to form a week
    //array of connections per day to make computations simpler and optimize the program (pWD - per Work Day)
    unsigned daily_pWD[7] = {0};  // 0 = Sunday, ..., 6 = Saturday
    if (dowMask & DOW_MON) daily_pWD[1] = perWorkDay;
    if (dowMask & DOW_TUE) daily_pWD[2] = perWorkDay;
    if (dowMask & DOW_WED) daily_pWD[3] = perWorkDay;
    if (dowMask & DOW_THU) daily_pWD[4] = perWorkDay;
    if (dowMask & DOW_FRI) daily_pWD[5] = perWorkDay;
    if (dowMask & DOW_SAT) daily_pWD[6] = (perWorkDay + 1) / 2;
    if (dowMask & DOW_SUN) daily_pWD[0] = (perWorkDay + 2) / 3;

    if (weeks > 0) {
        for (int i = 0; i < 7; i++) //compute how much connections are done per week
            res += daily_pWD[i];

        res *= weeks; //and multiply it by number of weeks in given interval
    }
    unsigned start_day = dayOfTheWeek(from); //compute what day of week was date 'from'

    for (unsigned int i = 0; i < remaining_days; i++) //go for each remaining day and add its pWD to result
        res += daily_pWD[(start_day + i) % 7];
    return res;
}
TDATE     endDate          ( TDATE     from,
                             long long connections,
                             unsigned  perWorkDay,
                             unsigned  dowMask )
{
    if(!isValidDate(from) || connections < 0 || perWorkDay <= 0 || connections < countConnections(from, from, perWorkDay, dowMask) || dowMask == 0) //incorrect input
        return makeDate(0, 0, 0);
    long long left_con = connections;
    //array of connections per day to make computations simpler and optimize the program (pWD - per Work Day)
    unsigned daily_pWD[7] = {0};  // 0 = Sunday, ..., 6 = Saturday
    if (dowMask & DOW_MON) daily_pWD[1] = perWorkDay;
    if (dowMask & DOW_TUE) daily_pWD[2] = perWorkDay;
    if (dowMask & DOW_WED) daily_pWD[3] = perWorkDay;
    if (dowMask & DOW_THU) daily_pWD[4] = perWorkDay;
    if (dowMask & DOW_FRI) daily_pWD[5] = perWorkDay;
    if (dowMask & DOW_SAT) daily_pWD[6] = (perWorkDay + 1) / 2;
    if (dowMask & DOW_SUN) daily_pWD[0] = (perWorkDay + 2) / 3;

    int days = 0; //amount of days which we need to add to 'from' to have 'to' date
    //compute how many connection do we cover per one week
    long long conn_per_week = 0;
    for(int i = 0; i < 7; i++)
        conn_per_week += daily_pWD[i];
    //compute how many weeks are passing to cover needed amount of connections and how much are left to cover by remaining days
    if(left_con > conn_per_week){
        days += 7*(connections/conn_per_week);
        left_con = (connections%conn_per_week);
    }
        unsigned start_day = dayOfTheWeek(from);
        int last_remaining_day = 0; // go from day of the week which is 'from' date and increment it
        while(left_con >= 0){ //we go the first day which is out of needed interval and return the date with -1 day
            left_con -= daily_pWD[(start_day+last_remaining_day)%7]; //in cycle we subtract from left connections pWD from our day
            last_remaining_day++; //and go to another day
        }
        return plusDays(from, days+last_remaining_day-2);
}
#ifndef __PROGTEST__
int main ()
{
  TDATE d;

     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_ALL ) == 31 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 10, DOW_ALL ) == 266 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_WED ) == 5 );
     assert ( countConnections ( makeDate ( 2024, 10, 2 ), makeDate ( 2024, 10, 30 ), 1, DOW_WED ) == 5 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_TUE ) == 10 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_WED ) == 0 );
    //printf("%lld\n", countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 5, DOW_MON | DOW_FRI | DOW_SAT ));
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 5, DOW_MON | DOW_FRI | DOW_SAT ) == 7462 );
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 0, DOW_MON | DOW_FRI | DOW_SAT ) == 0 );
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 100, 0 ) == 0 );
     assert ( countConnections ( makeDate ( 2024, 10, 10 ), makeDate ( 2024, 10, 9 ), 1, DOW_MON ) == -1 );
     assert ( countConnections ( makeDate ( 2024, 2, 29 ), makeDate ( 2024, 2, 29 ), 1, DOW_ALL ) == 1 );
     assert ( countConnections ( makeDate ( 2023, 2, 29 ), makeDate ( 2023, 2, 29 ), 1, DOW_ALL ) == -1 );
     assert ( countConnections ( makeDate ( 2100, 2, 29 ), makeDate ( 2100, 2, 29 ), 1, DOW_ALL ) == -1 );
     assert ( countConnections ( makeDate ( 2400, 2, 29 ), makeDate ( 2400, 2, 29 ), 1, DOW_ALL ) == 1 );
     assert ( countConnections ( makeDate ( 4000, 2, 29 ), makeDate ( 4000, 2, 29 ), 1, DOW_ALL ) == -1 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_ALL );
     assert ( d . m_Year == 2025 && d . m_Month == 1 && d . m_Day == 8 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 6, DOW_ALL );
     assert ( d . m_Year == 2024 && d . m_Month == 10 && d . m_Day == 20 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_WORKDAYS );
     assert ( d . m_Year == 2025 && d . m_Month == 2 && d . m_Day == 17 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 4, DOW_WORKDAYS );
     assert ( d . m_Year == 2024 && d . m_Month == 11 && d . m_Day == 4 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_THU );
     assert ( d . m_Year == 2026 && d . m_Month == 9 && d . m_Day == 2 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 2, DOW_THU );
     assert ( d . m_Year == 2025 && d . m_Month == 9 && d . m_Day == 17 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 0, DOW_THU );
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );
     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, 0 );
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );
   // printf("%lld\n", countConnections ( makeDate ( 2000, 1, 28 ), makeDate ( 2011, 10, 22 ), 25, DOW_WEEKEND | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI ));
    //printf("%d\n", countDays( makeDate ( 2000, 1, 28 ), makeDate ( 2011, 10, 22 )));
     assert(countConnections ( makeDate ( 2000, 1, 28 ), makeDate ( 2011, 10, 22 ), 25, DOW_WEEKEND | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI ) ==74702);

    //printf("%lld\n",countConnections(makeDate(2024, 1, 1), makeDate(260340, 12, 31), 5, DOW_MON | DOW_FRI | DOW_SAT));
    assert(countConnections(makeDate(2024, 1, 1), makeDate(260340, 12, 31), 5, DOW_MON | DOW_FRI | DOW_SAT)   ==   175218238);
     d = endDate ( makeDate (2296, 5, 11 ), 527748, 5, DOW_MON | DOW_TUE | DOW_FRI | DOW_SUN );
     assert ( d . m_Year == 2891 && d . m_Month == 4 && d . m_Day == 29 );

     assert(countConnections(makeDate(2024, 1, 1), makeDate(2603405, 12, 31), 5, DOW_MON | DOW_FRI | DOW_SAT)   ==   1764535713);

     d = endDate ( makeDate ( 2024, 12, 35 ), 100, 1, DOW_TUE ); // invalid date
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );

     d = endDate(makeDate(4001, 2, 29), 100, 2, DOW_TUE); // invalid date with leap year
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );

     d = endDate ( makeDate ( 2024, 10, 1 ), -1, 4, DOW_TUE ); // negative number of connections
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );

     d = endDate ( makeDate ( 2024, 10, 1 ), 3, 4, DOW_TUE ); //number of connections is not enough even for the first day from
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );

     d = endDate ( makeDate ( 2000, 1, 1 ), 20, 1, DOW_MON );
     assert ( d . m_Year == 2000 && d . m_Month == 5 && d . m_Day == 21 );

     d = endDate ( makeDate ( 4001, 1, 3 ), 20, 1, DOW_MON );
     assert ( d . m_Year == 4001 && d . m_Month == 5 && d . m_Day == 28 );
     d = endDate ( makeDate ( 4001, 1, 16 ), 20, 1, DOW_FRI );
     assert ( d . m_Year == 4001 && d . m_Month == 6 && d . m_Day == 8 );

     d =endDate ( makeDate ( 4001, 1, 15 ), 5, 6, DOW_FRI );
     assert ( d . m_Year == 4001 && d . m_Month == 1 && d . m_Day == 19 );

     d = endDate ( makeDate (2296, 5, 11 ), 527748, 5, DOW_MON | DOW_TUE | DOW_FRI | DOW_SUN );
     assert ( d . m_Year == 2891 && d . m_Month == 4 && d . m_Day == 29 );

     assert(countConnections ( makeDate ( 2024, 10, 2 ), makeDate ( 2024, 10, 30 ), 1, DOW_WED )== 5);

     d = endDate ( makeDate ( 4001, 1, 25 ), 20, 1, DOW_FRI );
     assert ( d . m_Year == 4001 && d . m_Month == 6 && d . m_Day == 15 );

     d = endDate ( makeDate ( 4001, 1, 18 ), 5, 6, DOW_FRI );
     assert ( d . m_Year == 4001 && d . m_Month == 1 && d . m_Day == 19 );

     d = endDate ( makeDate ( 4001, 0, 30 ), 100, 1, DOW_MON );
     assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );

     d = endDate ( makeDate ( 2000, 1, 1 ), 20, 1, DOW_MON );
     assert ( d . m_Year == 2000 && d . m_Month == 5 && d . m_Day == 21 );

     d = endDate ( makeDate ( 2009, 7, 24 ), 2312, 25, DOW_MON | DOW_TUE | DOW_THU | DOW_FRI );
     assert ( d . m_Year == 2009 && d . m_Month == 12 && d . m_Day == 31 );

     d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_THU );
     assert ( d . m_Year == 2026 && d . m_Month == 9 && d . m_Day == 2 );

     d = endDate ( makeDate ( 2296, 5, 11 ), 527748, 5, DOW_MON | DOW_TUE | DOW_FRI | DOW_SUN );
     assert(d.m_Year == 2891 && d.m_Month == 4 && d.m_Day == 29);

     d = endDate(makeDate(2024,10,28), 3, 3, DOW_MON);
     assert ( d . m_Year == 2024 && d . m_Month == 11 && d . m_Day == 3 );

     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_ALL ) == 31 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 10, DOW_ALL ) == 266 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_WED ) == 5 );
     assert ( countConnections ( makeDate ( 2024, 10, 2 ), makeDate ( 2024, 10, 30 ), 1, DOW_WED ) == 5 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_TUE ) == 10 );
     assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_WED ) == 0 );
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 5, DOW_MON | DOW_FRI | DOW_SAT ) == 7462 );
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 0, DOW_MON | DOW_FRI | DOW_SAT ) == 0 );
     assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 100, 0 ) == 0 );
     assert ( countConnections ( makeDate ( 2024, 10, 10 ), makeDate ( 2024, 10, 9 ), 1, DOW_MON ) == -1 );
     assert ( countConnections ( makeDate ( 2024, 2, 29 ), makeDate ( 2024, 2, 29 ), 1, DOW_ALL ) == 1 );
     assert ( countConnections ( makeDate ( 2023, 2, 29 ), makeDate ( 2023, 2, 29 ), 1, DOW_ALL ) == -1 );
     assert ( countConnections ( makeDate ( 2100, 2, 29 ), makeDate ( 2100, 2, 29 ), 1, DOW_ALL ) == -1 );
     assert ( countConnections ( makeDate ( 2400, 2, 29 ), makeDate ( 2400, 2, 29 ), 1, DOW_ALL ) == 1 );
     assert ( countConnections ( makeDate ( 4000, 2, 29 ), makeDate ( 4000, 2, 29 ), 1, DOW_ALL ) == -1 );

     assert(countConnections ( makeDate ( 2024, 10, 2 ), makeDate ( 2024, 10, 30 ), 1, DOW_WED )== 5);



    assert(countConnections(makeDate(2024, 1, 1), makeDate(2603405, 12, 31), 5, DOW_MON | DOW_FRI | DOW_SAT)   ==   1764535713);

   // printf("%lld\n", countConnections ( makeDate ( 18315, 1, 25 ), makeDate ( 44525, 7, 26 ), 28, DOW_WEEKEND | DOW_MON | DOW_TUE | DOW_THU ));
   // printf("%d\n", countDays(makeDate ( 18315, 1, 25 ), makeDate ( 44525, 7, 26 )));
   assert(countConnections ( makeDate ( 18315, 1, 25 ), makeDate ( 44525, 7, 26 ), 28, DOW_WEEKEND | DOW_MON | DOW_TUE | DOW_THU ) == 147700528);



    d = endDate ( makeDate ( 4001, 1, 24 ), 5, 6, DOW_FRI );
    assert(d.m_Year == 4001 && d.m_Month == 1 && d.m_Day == 26);

    d = endDate ( makeDate ( 2214, 5, 7 ), 1603219748, 5, DOW_MON | DOW_THU | DOW_FRI | DOW_SUN );
    assert(d.m_Year == 1809642 && d.m_Month == 12 && d.m_Day == 31);

    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_ALL );
    assert ( d . m_Year == 2025 && d . m_Month == 1 && d . m_Day == 8 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 6, DOW_ALL );
    assert ( d . m_Year == 2024 && d . m_Month == 10 && d . m_Day == 20 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_WORKDAYS );
    assert ( d . m_Year == 2025 && d . m_Month == 2 && d . m_Day == 17 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 4, DOW_WORKDAYS );
    assert ( d . m_Year == 2024 && d . m_Month == 11 && d . m_Day == 4 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_THU );
    assert ( d . m_Year == 2026 && d . m_Month == 9 && d . m_Day == 2 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 2, DOW_THU );
    assert ( d . m_Year == 2025 && d . m_Month == 9 && d . m_Day == 17 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 0, DOW_THU );
    assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, 0 );
    assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );


    //------------------------------------------------------------------------------------


    d = endDate ( makeDate ( 2024, 12, 35 ), 100, 1, DOW_TUE ); // invalid date
assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );
 return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

