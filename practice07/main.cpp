#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

class CTimeStamp
{
    // TODO
private:
    int year, month, day, hour, minute, second;
public:
    CTimeStamp(int y, int m, int d, int h, int min, int s):
            year(y), month(m), day(d), hour(h), minute(min), second(s){}
    CTimeStamp(const CTimeStamp& ts) : year(ts.year), month(ts.month),
                                       day(ts.day), hour(ts.hour), minute(ts.minute), second(ts.second) {}

    bool operator<(const CTimeStamp& ts) const {
        if (year != ts.year) return year < ts.year;
        if (month != ts.month) return month < ts.month;
        if (day != ts.day) return day < ts.day;
        if (hour != ts.hour) return hour < ts.hour;
        if (minute != ts.minute) return minute < ts.minute;
        return second < ts.second;
    }

    bool operator>(const CTimeStamp& ts) const {
        if (year != ts.year) return year > ts.year;
        if (month != ts.month) return month > ts.month;
        if (day != ts.day) return day > ts.day;
        if (hour != ts.hour) return hour > ts.hour;
        if (minute != ts.minute) return minute > ts.minute;
        return second > ts.second;
    }

    bool operator==(const CTimeStamp& ts) const {
        return !(*this < ts) && !(*this > ts);
    }

    bool operator>=(const CTimeStamp& ts) const {
        return (*this > ts) || (*this == ts);
    }

    bool operator<=(const CTimeStamp& ts) const {
        return (*this < ts) || (*this == ts);
    }
};
class CContact
{
private:
    CTimeStamp timeStamp;
    int number1;
    int number2;
public:
    CContact(CTimeStamp ts, int num1, int num2) :timeStamp(ts), number1(num1), number2(num2) {};
    int getNumber1() const { return number1; }
    int getNumber2() const { return number2; }
    const CTimeStamp& getTimeStamp() const { return timeStamp; }
};
class CEFaceMask
{
    // TODO
private:
    vector<CContact> contacts;
public:
    CEFaceMask() = default;
    ~CEFaceMask() = default;

    CEFaceMask& addContact(const CContact& contact){
        contacts.push_back(contact);
        return *this;
    }

    vector<int> listContacts(int num) const{
        vector<int> result;

        for (const auto& it : contacts) {
            if(it.getNumber1() == num && it.getNumber2() != num){
                bool found = false;
                for(auto it1 : result)
                    if(it1 == it.getNumber2()){
                        found = true;
                        break;
                    }
                if(!found)
                    result.push_back(it.getNumber2());
            }
            else if(it.getNumber2() == num && it.getNumber1() != num){
                bool found = false;
                for(auto it1 : result)
                    if(it1 == it.getNumber1()){
                        found = true;
                        break;
                    }
                if(!found)
                    result.push_back(it.getNumber1());
            }
        }

        return result;
    }

    vector<int> listContacts(int num, CTimeStamp beg, CTimeStamp end) const{
        vector<int> result;

        for (const auto& it : contacts)
            if(it.getTimeStamp() >= beg && it.getTimeStamp() <= end){
                if(it.getNumber1() == num && it.getNumber2() != num){
                    bool found = false;
                    for(auto it1 : result)
                        if(it1 == it.getNumber2()){
                            found = true;
                            break;
                        }
                    if(!found)
                        result.push_back(it.getNumber2());
                }
                else if(it.getNumber2() == num && it.getNumber1() != num){
                    bool found = false;
                    for(auto it1 : result)
                        if(it1 == it.getNumber1()){
                            found = true;
                            break;
                        }
                    if(!found)
                        result.push_back(it.getNumber1());
                }
            }

        return result;
    }

    vector<int> getSuperSpreaders(CTimeStamp beg, CTimeStamp end) const {
        map<int, set<int>> contactMap;

        for (const auto& contact : contacts) {
            const CTimeStamp& ts = contact.getTimeStamp();
            if (ts >= beg && ts <= end) {
                int a = contact.getNumber1();
                int b = contact.getNumber2();
                if (a != b) {
                    contactMap[a].insert(b);
                    contactMap[b].insert(a);
                }
            }
        }

        int maxCount = 0;
        for (const auto& entry : contactMap)
            maxCount = max(maxCount, (int)entry.second.size());


        vector<int> result;
        for (const auto& entry : contactMap)
            if ((int)entry.second.size() == maxCount)
                result.push_back(entry.first);

        sort(result.begin(), result.end());
        return result;
    }
};


#ifndef __PROGTEST__
int main ()
{
    CEFaceMask test;

    test . addContact ( CContact ( CTimeStamp ( 2021, 1, 10, 12, 40, 10 ), 111111111, 222222222 ) );
    test . addContact ( CContact ( CTimeStamp ( 2021, 1, 12, 12, 40, 10 ), 333333333, 222222222 ) )
            . addContact ( CContact ( CTimeStamp ( 2021, 2, 14, 15, 30, 28 ), 222222222, 444444444 ) );
    test . addContact ( CContact ( CTimeStamp ( 2021, 2, 15, 18, 0, 0 ), 555555555, 444444444 ) );
    assert ( test . getSuperSpreaders ( CTimeStamp ( 2021, 1, 1, 0, 0, 0 ), CTimeStamp ( 2022, 1, 1, 0, 0, 0 ) ) == (vector<int> {222222222}) );
    test . addContact ( CContact ( CTimeStamp ( 2021, 3, 20, 18, 0, 0 ), 444444444, 666666666 ) );
    test . addContact ( CContact ( CTimeStamp ( 2021, 3, 25, 0, 0, 0 ), 111111111, 666666666 ) );
    assert ( test . getSuperSpreaders ( CTimeStamp ( 2021, 1, 1, 0, 0, 0 ), CTimeStamp ( 2022, 1, 1, 0, 0, 0 ) ) == (vector<int> {222222222, 444444444}) );
    return 0;
}
#endif /* __PROGTEST__ */
