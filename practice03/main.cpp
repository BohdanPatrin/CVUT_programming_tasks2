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

struct Contact {
    string firstName;
    string lastName;
    string phone;
};

bool isValidPhoneNumber(const string &phone) {
    if (phone.size() != 9 || phone[0] == '0') return false;
    return all_of(phone.begin(), phone.end(), ::isdigit);
}

bool report(const string &fileName, ostream &out) {
    ifstream file(fileName);
    if (!file) return false;

    vector<Contact> contacts;
    string line;
    bool emptyLineFound = false;

    while (getline(file, line)) { //read contacts until we get empty line
        if (line.empty()) {
            emptyLineFound = true;
            break;
        }
        //read contact info from line
        istringstream iss(line);
        Contact contact;
        if (!(iss >> contact.firstName >> contact.lastName >> contact.phone)) return false;

        string extra;
        if (iss >> extra) return false; //check extra fields after phone number
        if (!isValidPhoneNumber(contact.phone)) return false;

        contacts.push_back(contact);
    }

    if (!emptyLineFound) return false;

    vector<string> queries;
    while (getline(file, line)) {
        if (!line.empty()) queries.push_back(line);
    }
    //process each search query
    for (const string &query : queries) {
        vector<Contact> results;
        for (const Contact &c : contacts) {
            if (c.firstName == query || c.lastName == query) {
                results.push_back(c);
            }
        }

        for (const Contact &c : results) {
            out << c.firstName << " " << c.lastName << " " << c.phone << "\n";
        }
        out << "-> " << results.size() << "\n";
    }

    return true;
}


#ifndef __PROGTEST__
int main ()
{
    ostringstream oss;
    oss . str ( "" );
    assert ( report( "tests/test0_in.txt", oss ) == true );
    assert ( oss . str () ==
             "John Christescu 258452362\n"
             "John Harmson 861647702\n"
             "-> 2\n"
             "-> 0\n"
             "Josh Dakhov 264112084\n"
             "Dakhov Speechley 865216101\n"
             "-> 2\n"
             "John Harmson 861647702\n"
             "-> 1\n" );
    oss . str ( "" );
    assert ( report( "tests/test1_in.txt", oss ) == false );
    return 0;
}
#endif /* __PROGTEST__ */
