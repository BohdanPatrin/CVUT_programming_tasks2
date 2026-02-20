#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
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
};

#ifndef __PROGTEST__
int main()
{
    CEFaceMask test;

    test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 123456789, 111222333))
            .addContact(CContact(CTimeStamp(2021, 2, 5, 15, 30, 28), 999888777, 555000222));
    test.addContact(CContact(CTimeStamp(2021, 2, 21, 18, 0, 0), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 5, 18, 0, 0), 123456789, 456456456));
    test.addContact(CContact(CTimeStamp(2021, 2, 1, 0, 0, 0), 123456789, 123456789));
    assert(test.listContacts(123456789) == (vector<int> {999888777, 111222333, 456456456}));
    assert(test.listContacts(999888777) == (vector<int> {123456789, 555000222}));
    assert(test.listContacts(191919191) == (vector<int> {}));
    assert(test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 0), CTimeStamp(2021, 2, 21, 18, 0, 0)) == (vector<int> {999888777, 111222333, 456456456}));
    assert(test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 1), CTimeStamp(2021, 2, 21, 17, 59, 59)) == (vector<int> {999888777, 111222333}));
    assert(test.listContacts(123456789, CTimeStamp(2021, 1, 10, 12, 41, 9), CTimeStamp(2021, 2, 21, 17, 59, 59)) == (vector<int> {111222333}));
    return 0;
}
#endif /* __PROGTEST__ */
