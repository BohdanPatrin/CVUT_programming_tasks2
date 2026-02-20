#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <algorithm>
#include <functional>

#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cctype>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */


template <typename Type>
class CSet {
private:
    struct Node {
        Type data;
        Node* next;

        Node(const Type& d, Node* n = nullptr)
                : data(d), next(n) {}
    };

    Node* head;
    size_t count;

    void clear() {
        while (head) {
            Node* tmp = head->next;
            delete head;
            head = tmp;
        }
        count = 0;
    }

    void copyFrom(const CSet& other) {
        if (other.head == nullptr) return;
        head = new Node(other.head->data);
        Node* curr = head;
        Node* src = other.head->next;

        while (src) {
            curr->next = new Node(src->data);
            curr = curr->next;
            src = src->next;
        }

        count = other.count;
    }

public:
    CSet() : head(nullptr), count(0) {}

    CSet(const CSet& other) : head(nullptr), count(0) {
        copyFrom(other);
    }

    CSet& operator=(const CSet& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    ~CSet() {
        clear();
    }

    bool Insert(const Type& value) {
        if (!head || value < head->data) {
            head = new Node(value, head);
            ++count;
            return true;
        }

        Node* prev = nullptr;
        Node* curr = head;
        while (curr && curr->data < value) {
            prev = curr;
            curr = curr->next;
        }

        if (curr && !(value < curr->data))
            return false;  //already present

        prev->next = new Node(value, curr);
        ++count;
        return true;
    }

    bool Remove(const Type& value) {
        Node* prev = nullptr;
        Node* curr = head;

        while (curr && curr->data < value) {
            prev = curr;
            curr = curr->next;
        }

        if (!curr || (value < curr->data))
            return false;  //not found


        if (prev) prev->next = curr->next;
        else head = curr->next;

        delete curr;
        --count;
        return true;
    }

    bool Contains(const Type& value) const {
        Node* curr = head;
        while (curr && curr->data < value)
            curr = curr->next;

        return curr && !(value < curr->data);
    }

    size_t Size() const {
        return count;
    }
};

#ifndef __PROGTEST__
#include <cassert>

struct CSetTester
{
    static void test0()
    {
        CSet<string> x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
    }

    static void test1()
    {
        CSet<string> x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        CSet<string> x1 ( x0 );
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    }

    static void test2()
    {
        CSet<string> x0;
        CSet<string> x1;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        x1 = x0;
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    }

    static void test4()
    {
        CSet<int> x0;
        assert( x0 . Insert( 4 ) );
        assert( x0 . Insert( 8 ) );
        assert( x0 . Insert( 1 ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( 4 ) );
        assert( !x0 . Contains( 5 ) );
        assert( !x0 . Remove( 5 ) );
        assert( x0 . Remove( 4 ) );
    }

};

int main ()
{
    CSetTester::test0();
    CSetTester::test1();
    CSetTester::test2();
    CSetTester::test4();
    return 0;
}
#endif /* __PROGTEST__ */

