#ifndef __PROGTEST__
#include <cstring>
using namespace std;

class CLinkedSetTester;
#endif /* __PROGTEST__ */


class CLinkedSet
{
private:
    struct CNode
    {
        CNode * m_Next;
        char * m_Value;

        CNode(const char * value, CNode * next = nullptr)
                : m_Next(next) {
            m_Value = new char[strlen(value) + 1];
            strcpy(m_Value, value);
        }

        ~CNode() {delete [] m_Value; }

        const char * Value () const { return m_Value; }
    };

    CNode * m_Begin;

    void Clear() {
        while (m_Begin) {
            CNode * tmp = m_Begin->m_Next;
            delete m_Begin;
            m_Begin = tmp;
        }
    }

    void CopyFrom(const CLinkedSet & src) {
        if (!src.m_Begin) {
            m_Begin = nullptr;
            return;
        }

        m_Begin = new CNode(src.m_Begin->Value());
        CNode * destCurr = m_Begin;
        CNode * srcCurr = src.m_Begin->m_Next;

        while (srcCurr) {
            destCurr->m_Next = new CNode(srcCurr->Value());
            destCurr = destCurr->m_Next;
            srcCurr = srcCurr->m_Next;
        }
    }

public:
    CLinkedSet(): m_Begin(nullptr) {}

    // copy constructor
    CLinkedSet(const CLinkedSet & other) : m_Begin(nullptr) {
        CopyFrom(other);
    }

    // assignment operator
    CLinkedSet & operator=(const CLinkedSet & other) {
        if (this == &other)
            return *this;

        Clear();
        CopyFrom(other);
        return *this;
    }

    // destructor
    ~CLinkedSet() {
        Clear();
    }

    bool Insert(const char * value) {
        CNode ** curr = &m_Begin;

        while (*curr && strcmp((*curr)->Value(), value) < 0)
            curr = &((*curr)->m_Next);

        if (*curr && strcmp((*curr)->Value(), value) == 0)
            return false;

        *curr = new CNode(value, *curr);
        return true;
    }

    bool Remove(const char * value) {
        CNode ** curr = &m_Begin;

        while (*curr && strcmp((*curr)->Value(), value) < 0)
            curr = &((*curr)->m_Next);

        if (!*curr || strcmp((*curr)->Value(), value) != 0)
            return false;

        CNode * tmp = *curr;
        *curr = (*curr)->m_Next;
        delete tmp;
        return true;
    }

    bool Empty () const {
        return m_Begin == nullptr;
    }

    size_t Size () const {
        size_t count = 0;
        for (CNode * curr = m_Begin; curr; curr = curr->m_Next)
            ++count;
        return count;
    }

    bool Contains(const char * value) const {
        for (CNode * curr = m_Begin; curr; curr = curr->m_Next) {
            int cmp = strcmp(curr->Value(), value);
            if (cmp == 0)
                return true;
            if (cmp > 0)
                return false;
        }
        return false;
    }
    friend class ::CLinkedSetTester;
};

#ifndef __PROGTEST__
#include <cassert>

struct CLinkedSetTester
{
    static void test0()
    {
        CLinkedSet x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        assert( !x0 . Empty() );
    }

    static void test1()
    {
        CLinkedSet x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        CLinkedSet x1 ( x0 );
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
        CLinkedSet x0;
        CLinkedSet x1;
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

};

int main ()
{
    CLinkedSetTester::test0();
    CLinkedSetTester::test1();
    CLinkedSetTester::test2();
    return 0;
}
#endif /* __PROGTEST__ */

