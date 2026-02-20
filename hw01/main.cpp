#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */

struct Account{ //structure of accounts inthe register
    std::string name = "";
    std::string addr = "";
    std::string id = "";
    int income = 0;
    int expense = 0;
};


class CIterator
{
public:
    CIterator( std::vector<Account*>  db, size_t pos){
        m_db = std::move(db);
        m_pos = pos;

    }

    ~CIterator() = default;

    bool atEnd() const {
        return m_pos >= m_db.size();
    }

    void next() {
        if (!atEnd()) ++m_pos;
    }

    const std::string & name() const {
        return (m_db)[m_pos]->name;
    }

    const std::string & addr() const {
        return (m_db)[m_pos]->addr;
    }

    const std::string & account() const {
        return (m_db)[m_pos]->id;
    }
private:
    // todo
    std::vector<Account*> m_db;
    size_t m_pos; // current index

};

class CTaxRegister
{
private:
    //two arrays in which accounts are sorted by name and id
    std::vector<Account*> namesorted;
    std::vector<Account*> idsorted;

    //binary search functions for register class methods
    size_t findAcc(const std::string name, const std::string addr) const{
        auto it = std::lower_bound(namesorted.begin(), namesorted.end(), std::make_pair(name, addr),
                                   [](const Account* acc, const std::pair<std::string, std::string>& key) {
                                       return std::pair(acc->name, acc->addr) < key;
                                   });

        if (it == namesorted.end() || (*it)->name != name || (*it)->addr != addr) return -1;
        return std::distance(namesorted.begin(), it);
    }

    size_t findAcc(const std::string& id) const{
        auto it = std::lower_bound(idsorted.begin(), idsorted.end(), id,
                                   [](const Account* acc, const std::string& id) {
                                       return acc->id < id;
                                   });

        if (it == idsorted.end() || (*it)->id != id) return -1;
        return std::distance(idsorted.begin(), it);
    }

public:
    CTaxRegister() = default;

    ~CTaxRegister() {
        for(Account* it : namesorted)
            delete it;

        namesorted.clear();
        idsorted.clear();
    }

    bool      birth      ( const std::string    & name,
                           const std::string    & addr,
                           const std::string    & account ){
        if ((int)findAcc(account) != -1 || (int)findAcc(name, addr) != -1)
            return false; // account already exists

        Account* newAcc = new Account{name, addr, account};


        auto itName = std::lower_bound(namesorted.begin(), namesorted.end(), newAcc,
                                       [](const Account* a, const Account* b) { return std::tie(a->name, a->addr) < std::tie(b->name, b->addr); });
        namesorted.insert(itName, newAcc);

        auto itID = std::lower_bound(idsorted.begin(), idsorted.end(), newAcc,
                                     [](const Account* a, const Account* b) { return a->id < b->id; });
        idsorted.insert(itID, newAcc);

        return true;

    };
    bool      death      ( const std::string    & name,
                           const std::string    & addr ){
        size_t posName = findAcc(name, addr);
        if ((int)posName == -1) return false;

        Account* acc = namesorted[posName];
        namesorted.erase(namesorted.begin() + posName);

        size_t posID = findAcc(acc->id);
        if ((int)posID != -1)
            idsorted.erase(idsorted.begin() + posID);
        delete acc;

        return true;

    };

    bool      income     ( const std::string    & account,
                           int                    amount ){
        auto it = std::lower_bound(idsorted.begin(), idsorted.end(), account,
                                   [](const Account* acc, const std::string &id) {
                                       return acc->id < id;
                                   });

        if (it == idsorted.end() || (*it)->id != account)
            return false;

        (*it)->income += amount;
        return true;
    };
    bool      income     ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ){
        auto it = std::lower_bound(namesorted.begin(), namesorted.end(), std::make_pair(name, addr),
                                   [](const Account* acc, const std::pair<std::string, std::string> &key) {
                                       return std::make_pair(acc->name, acc->addr) < key;
                                   });

        if (it == namesorted.end() || (*it)->name != name || (*it)->addr != addr)
            return false;

        (*it)->income += amount;
        return true;
    };
    bool      expense    ( const std::string    & account,
                           int                    amount ){
        auto it = std::lower_bound(idsorted.begin(), idsorted.end(), account,
                                   [](const Account* acc, const std::string &id) {
                                       return acc->id < id;
                                   });

        if (it == idsorted.end() || (*it)->id != account)
            return false;

        (*it)->expense += amount;
        return true;
    };

    bool      expense    ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ){
        auto it = std::lower_bound(namesorted.begin(), namesorted.end(), std::make_pair(name, addr),
                                   [](const Account* acc, const std::pair<std::string, std::string> &key) {
                                       return std::make_pair(acc->name, acc->addr) < key;
                                   });

        if (it == namesorted.end() || (*it)->name != name || (*it)->addr != addr)
            return false;

        (*it)->expense += amount;
        return true;
    };
    bool      audit      ( const std::string    & name,
                           const std::string    & addr,
                           std::string          & account,
                           int                  & sumIncome,
                           int                  & sumExpense ) const{
        auto it = std::lower_bound(namesorted.begin(), namesorted.end(), std::make_pair(name, addr),
                                   [](const Account* acc, const std::pair<std::string, std::string> &key) {
                                       return std::make_pair(acc->name, acc->addr) < key;
                                   });

        if (it == namesorted.end() || (*it)->name != name || (*it)->addr != addr)
            return false;

        account = (*it)->id;
        sumIncome = (*it)->income;
        sumExpense = (*it)->expense;
        return true;
    };
    CIterator listByName () const{
        return CIterator(namesorted, 0);
    }

};

#ifndef __PROGTEST__
int main ()
{
    std::string acct;
    int    sumIncome, sumExpense;
    CTaxRegister b0;
    assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
    assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
    assert ( b0 . income ( "Xuj5#94", 1000 ) );
    assert ( b0 . income ( "634oddT", 2000 ) );
    assert ( b0 . income ( "123/456/789", 3000 ) );
    assert ( b0 . income ( "634oddT", 4000 ) );
    assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
    assert ( b0 . expense ( "Xuj5#94", 1300 ) );
    assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "123/456/789" );
    assert ( sumIncome == 3000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 1000 );
    assert ( sumExpense == 4300 );
    assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "634oddT" );
    assert ( sumIncome == 8000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Z343rwZ" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 500 );
    CIterator it = b0 . listByName ();
    assert ( ! it . atEnd ()
             && it . name () == "Jane Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "Xuj5#94" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Main Street 17"
             && it . account () == "Z343rwZ" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Oak Road 23"
             && it . account () == "123/456/789" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "Peter Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "634oddT" );
    it . next ();
    assert ( it . atEnd () );

    assert ( b0 . death ( "John Smith", "Main Street 17" ) );
    CTaxRegister b1;
    assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( !b1 . income ( "634oddT", 4000 ) );
    assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
    assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
    assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
    assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
    assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 0 );
    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
