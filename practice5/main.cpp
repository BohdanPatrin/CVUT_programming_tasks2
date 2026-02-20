#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;

class CTimeTester;
#endif /* __PROGTEST__ */

class CTime
{
private:
    int m_Hour;
    int m_Minute;
    int m_Second;

    void validate(int h, int m, int s) {
        if (h < 0 || h >= 24 || m < 0 || m >= 60 || s < 0 || s >= 60)
            throw std::invalid_argument("Invalid time");
    }

    int toSeconds() const { //convert time to seconds
        return m_Hour * 3600 + m_Minute * 60 + m_Second;
    }

    static CTime fromSeconds(int totalSeconds) {
        int h = totalSeconds / 3600;
        int m = (totalSeconds % 3600) / 60;
        int s = totalSeconds % 60;
        return CTime(h, m, s);
    }

public:
    CTime() : m_Hour(0), m_Minute(0), m_Second(0) {}

    CTime(int hour, int minute) {
        validate(hour, minute, 0);
        m_Hour = hour;
        m_Minute = minute;
        m_Second = 0;
    }

    CTime(int hour, int minute, int second) {
        validate(hour, minute, second);
        m_Hour = hour;
        m_Minute = minute;
        m_Second = second;
    }

    CTime operator+(int sec) const {
        CTime tmp = *this;
        tmp = fromSeconds(toSeconds() + sec);
        return tmp;
    }

    CTime& operator+=(int seconds) {
        int total = (toSeconds() + seconds) % 86400;
        if (total < 0) total += 86400;
        *this = CTime::fromSeconds(total);
        return *this;
    }

    CTime operator-(int sec) const {
        CTime tmp = *this;
        tmp = fromSeconds(toSeconds() - sec);
        return tmp;
    }

    CTime& operator-=(int seconds) {
        int total = (toSeconds() - seconds) % 86400;
        if (total < 0) total += 86400;
        *this = CTime::fromSeconds(total);
        return *this;
    }

    CTime operator+(const CTime& other) const {
        int total = (toSeconds() + other.toSeconds()) % 86400;
        return fromSeconds(total);
    }

    int operator-(const CTime& other) const {
        int diff = (toSeconds() - other.toSeconds() + 86400) % 86400;
        return diff;
    }

    CTime& operator++() { return (*this += 1); }
    CTime operator++(int) { CTime tmp = *this; ++(*this); return tmp; }

    CTime& operator--() { return (*this -= 1); }
    CTime operator--(int) { CTime tmp = *this; --(*this); return tmp; }

    bool operator<(const CTime& other) const { return toSeconds() < other.toSeconds(); }
    bool operator<=(const CTime& other) const { return toSeconds() <= other.toSeconds(); }
    bool operator>(const CTime& other) const { return toSeconds() > other.toSeconds(); }
    bool operator>=(const CTime& other) const { return toSeconds() >= other.toSeconds(); }
    bool operator==(const CTime& other) const { return toSeconds() == other.toSeconds(); }
    bool operator!=(const CTime& other) const { return toSeconds() != other.toSeconds(); }

    friend ostream& operator<<(ostream& os, const CTime& t) {
        os << setw(2) << setfill(' ') << t.m_Hour << ":"
           << setw(2) << setfill('0') << t.m_Minute << ":"
           << setw(2) << setfill('0') << t.m_Second;
        return os;
    }

    friend CTime operator+(int sec, const CTime& t){
        return t + sec; // Leverage member operator+
    }
    friend CTime operator-(int sec, const CTime& t){
        int newSeconds = ((sec - t.toSeconds()) % 86400 + 86400) % 86400;
        CTime result;
        result += newSeconds;
        return result;
    }

    friend class ::CTimeTester;
};

#ifndef __PROGTEST__
struct CTimeTester
{
    static bool test ( const CTime & time, int hour, int minute, int second )
    {
        return time.m_Hour == hour 
            && time.m_Minute == minute
            && time.m_Second == second;
    }
};

#include <cassert>
#include <sstream>

int main ()
{
    CTime a { 12, 30 };
    assert( CTimeTester::test( a, 12, 30, 0 ) );

    CTime b { 13, 30 };
    assert( CTimeTester::test( b, 13, 30, 0 ) );

    assert( b - a == 3600 );

    assert( CTimeTester::test( a + 60, 12, 31, 0 ) );
    assert( CTimeTester::test( a - 60, 12, 29, 0 ) );

    assert( a < b );
    assert( a <= b );
    assert( a != b );
    assert( !( a > b ) );
    assert( !( a >= b ) );
    assert( !( a == b ) );

    while ( ++a != b );
    assert( a == b );

    std::ostringstream output;
    assert( static_cast<bool>( output << a ) );
    assert( output.str() == "13:30:00" );

    assert( a++ == b++ );
    assert( a == b );

    assert( --a == --b );
    assert( a == b );

    assert( a-- == b-- );
    assert( a == b );

    return 0;
}
#endif /* __PROGTEST__ */