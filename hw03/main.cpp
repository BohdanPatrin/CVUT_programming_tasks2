#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

struct Date { //struct to store dates
    int year, month, day;

    Date(const char* dateStr) {
        sscanf(dateStr, "%d-%d-%d", &year, &month, &day);
    }

    Date() : year(0), month(0), day(0) {}

    ~Date() = default;

    bool operator==(const Date& other) const {
        return day == other.day && month == other.month && year == other.year;
    }

    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        //handling leading zeroes
        if(date.year < 1000){
            if(date.year < 100){
                if(date.year < 10)
                    os<<'0';
                os <<'0';
            }
            os <<'0';
        }
        os << date.year << '-';

        if (date.month < 10) os << '0';
        os << date.month << '-';

        if (date.day < 10) os << '0';
        os << date.day;

        return os;
    }

};

class CString { //class implementing a C string
private:
    char* data;
    size_t length;

public:
    CString(const char* str = "") {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }

    CString(const CString& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
    }

    ~CString() {
        delete[] data;
    }

    const char* get_data() const {
        return data;
    }

    CString& operator=(const CString& other) {
        if (this != &other) {
            delete[] data;
            length = other.length;
            data = new char[length + 1];
            strcpy(data, other.data);
        }
        return *this;
    }

    int strcmp(const CString& other) const { //my implementation of strcmp
        size_t i = 0;
        while (data[i] != '\0' && other.data[i] != '\0') {
            if (data[i] < other.data[i]) return -1;
            if (data[i] > other.data[i]) return 1;
            i++;
        }

        if (data[i] == '\0' && other.data[i] == '\0') return 0;
        return (data[i] == '\0') ? -1 : 1;
    }

    bool operator==(const CString& other) const {
        return strcmp( other.data) == 0;
    }

    bool operator!=(const char* other) const {
        return strcmp( other) != 0;
    }

    char& operator[](size_t index) {
        return data[index];
    }

    const char& operator[](size_t index) const {
        return data[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const CString& str) { //output operator
        return os << str.data;
    }
};

template <typename T>

class CVector { //class implementing a vector
private:
    T* data;
    size_t size;
    size_t capacity;

public:
    CVector() : data(nullptr), size(0), capacity(0) {}

    void resize(size_t new_capacity) { //needed when size >= capacity
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size; ++i)
            new_data[i] = data[i];

        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

    void clear() { //needed for destructor
        for (size_t i = 0; i < size; ++i)
            data[i].~T();

        size = 0;
    }

    ~CVector() {
        clear();
        delete[] data;
    }

    void push_back(const T& value) { //inserting new element at the end
        if (size == capacity)
            resize(capacity == 0 ? 1 : capacity * 2);

        data[size++] = value;
    }

    void insert(size_t index, const T& value) { //inserting new element at specific position
        if (size == capacity) resize(capacity == 0 ? 1 : capacity * 2);

        for (size_t i = size; i > index; --i) { //and removing other elements
            new(data + i) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }

        new(data + index) T(value);
        ++size;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const { //needed if we only read the element
        return data[index];
    }

    int get_size() const {
        return size;
    }

    //my implementation of lower bound - needed for fast finding the element
    template <typename Compare>
    int lower_bound(const T& value, Compare cmp) const {
        int left = 0, right = size;

        while (left < right) {
            int mid = left + (right - left) / 2;
            if (cmp(data[mid], value)) left = mid + 1;
            else right = mid;
        }
        return left;
    }

};

class Domicile{ //class representing domicile
private:
    CString street, city;
    Date date;

public:
    Domicile(const char date[], const char street[], const char city[]): street(street), city(city), date(date){};

    Domicile(const Domicile& other): street(other.street), city(other.city), date(other.date){};

    Domicile() = default;
    ~Domicile() = default;

    friend std::ostream& operator<<(std::ostream& os, const Domicile* house){
        os << house->date<< ' ' << house->street << ' ' << house->city;
        return os;
    }

    Date get_date() const{
        return date;
    }
};

class Person{ //class representing person
private:
    CString id, name,  surname;
    CVector<Domicile*> houses; // vector of domiciles which we keep always sorted by date

public:
    Person(const char id[], const char name[], const char surname[], const char date[],
           const char street[],const char city[]): id(id), name(name),surname(surname){
        houses.push_back(new Domicile(date, street, city));
    };

    Person(const Person& other) : id(other.id), name(other.name), surname(other.surname) {
        houses.resize(other.houses.get_size());
        for (int i = 0; i < other.houses.get_size(); ++i)
            houses.push_back( new Domicile(*other.houses[i]));

    }

    Person& operator=(const Person& other) {
        if (this == &other) return *this;

        for (int i = 0; i < houses.get_size(); ++i)
            delete houses[i];

        houses.clear();

        id = other.id;
        name = other.name;
        surname = other.surname;

        houses.resize(other.houses.get_size());
        for (int i = 0; i < other.houses.get_size(); ++i)
            houses[i] = new Domicile(*other.houses[i]);

        return *this;
    }

    Person() = default;

    ~Person(){
        for (int i = 0; i < houses.get_size(); ++i)
            delete houses[i];
    }

    bool addHome(const char date[],
                 const char street[],const char city[]){
        auto * tmp = new Domicile(date, street, city);
        //finding the position where to insert the house (domicile)
        int pos = houses.lower_bound(tmp, [](const Domicile* a, const Domicile* b)
        {if (a->get_date().year != b->get_date().year) return a->get_date().year < b->get_date().year;
            if (a->get_date().month != b->get_date().month) return a->get_date().month < b->get_date().month;
            return a->get_date().day < b->get_date().day;});
        //if the house is already in the vector - return false
        if(pos != houses.get_size() && houses[pos]->get_date() == Date(date)){
            delete tmp;
            return false;
        }

        houses.insert(pos, tmp);
        return true;
    }

    void print( std::ostream & os) const{
        os << id << ' ' << name << ' ' << surname << '\n';
        for(int i = 0; i < houses.get_size(); i++)
            os << houses[i] << '\n';

    };

    CString get_id() const{
        return id;
    }
};

class CRegister
{
public:
    CRegister(){
        people = new CVector<Person*>();
        refCount = new int(1);
    }

    CRegister(const CRegister& other) {
        people = other.people;
        refCount = other.refCount;
        (*refCount)++;
    }

    CRegister& operator=(const CRegister& other){
        if (this == &other) return *this;


        if (--(*refCount) == 0) {
            for (int i = 0; i < people->get_size(); ++i)
                delete (*people)[i];
            delete people;
            delete refCount;
        }

        people = other.people;
        refCount = other.refCount;
        (*refCount)++;

        return *this;
    }

    ~CRegister() {
        if (--(*refCount) == 0) {
            for (int i = 0; i < people->get_size(); ++i)
                delete (*people)[i];
            delete people;
            delete refCount;
        }
    }

    void ensureUnique() { //ensuring that the register is unique (needed for copy-on-write)
        if (*refCount > 1) { //if there are other registers with the same database
            CVector<Person*>* newPeople = new CVector<Person*>(); //we make deep copy of it
            for (int i = 0; i < people->get_size(); ++i)
                newPeople->push_back(new Person(*(*people)[i]));

            (*refCount)--;
            people = newPeople;
            refCount = new int(1); //and make refCount = 1
        }
    }

    bool add       ( const char     id[],
                     const char     name[],
                     const char     surname[],
                     const char     date[],
                     const char     street[],
                     const char     city[] ){
        ensureUnique();
        auto *tmp = new Person(id,name, surname, date, street, city);
        if(!people->get_size()){ //if register is empty - we just push back the person
            people->push_back(tmp);
            return true;
        }
        //finding the suitable position for the person
        int pos = people->lower_bound(tmp, [](const Person* a, const Person* b)
        {return strcmp(a->get_id().get_data(), b->get_id().get_data()) < 0;});
        if(pos < people->get_size() && (*people)[pos]->get_id() == id){ //if register already has this person
            delete tmp;
            return false; //we return false
        }
        //in other case - we insert it in the suitable position
        people->insert(pos, tmp);
        return true;
    }

    bool resettle  ( const char     id[],
                     const char     date[],
                     const char     street[],
                     const char     city[] ){
        ensureUnique();
        auto *tmp = new Person(id,"", "", date, street, city);
        int pos = people->lower_bound(tmp, [](const Person* a, const Person* b)
        {return strcmp(a->get_id().get_data(), b->get_id().get_data()) < 0;}); //finding the person by lowerbound
        delete tmp;
        if(pos >= people->get_size() || (*people)[pos]->get_id() != id) return false; //if we haven't found it - return false

        return (*people)[pos]->addHome(date, street, city);
    }

    bool print     ( std::ostream & os,
                     const char     id[] ) const{
        Person* tmp = new Person(id, "", "", "", "", "");
        int pos = people->lower_bound(tmp, [](const Person* a, const Person* b)
        {return strcmp(a->get_id().get_data(), b->get_id().get_data()) < 0;});//finding the person by lowerbound
        delete tmp;

        if(pos >= people->get_size() || (*people)[pos]->get_id() != id) return false; //if we haven't found it - return false

        (*people)[pos]->print(os);
        return true;
    }
private:
    // todo
    CVector<Person*>* people; //vector of pointers for memory efficiency which we keep always sorted by ids
    int* refCount; //shows how many registers has the same data (needed for copy-on-write)

};

#ifndef __PROGTEST__
int main ()
{
    char   lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
    std::ostringstream oss;
    CRegister  a;
    assert ( a . add ( "123456/7890", "John", "Smith", "2000-01-01", "Main street", "Seattle" ) == true );
    assert ( a . add ( "987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street", "Sacramento" ) == true );
    assert ( a . resettle ( "123456/7890", "2003-05-12", "Elm street", "Atlanta" ) == true );
    assert ( a . resettle ( "123456/7890", "2002-12-05", "Sunset boulevard", "Los Angeles" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "123456/7890" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
)###" ) );
    CRegister b ( a );
    assert ( b . resettle ( "987654/3210", "2008-04-12", "Elm street", "Cinccinati" ) == true );
    assert ( a . resettle ( "987654/3210", "2007-02-11", "Elm street", "Indianapolis" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2007-02-11 Elm street Indianapolis
)###" ) );
    oss . str ( "" );
    assert ( b . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
    a = b;
    assert ( a . resettle ( "987654/3210", "2011-05-05", "Elm street", "Salt Lake City" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
2011-05-05 Elm street Salt Lake City
)###" ) );
    oss . str ( "" );
    assert ( b . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
    assert ( b . add ( "987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road", "London" ) == false );
    assert ( a . resettle ( "987654/3210", "2001-02-03", "Second street", "Milwaukee" ) == false );
    oss . str ( "" );
    assert ( a . print ( oss, "666666/6666" ) == false );

    CRegister  c;
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lName, "John", sizeof ( lName ) );
    strncpy ( lSurname, "Smith", sizeof ( lSurname ) );
    strncpy ( lDate, "2000-01-01", sizeof ( lDate) );
    strncpy ( lStreet, "Main street", sizeof ( lStreet ) );
    strncpy ( lCity, "Seattle", sizeof ( lCity ) );
    assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "987654/3210", sizeof ( lID ) );
    strncpy ( lName, "Freddy", sizeof ( lName ) );
    strncpy ( lSurname, "Kruger", sizeof ( lSurname ) );
    strncpy ( lDate, "2001-02-03", sizeof ( lDate) );
    strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
    strncpy ( lCity, "Sacramento", sizeof ( lCity ) );
    assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lDate, "2003-05-12", sizeof ( lDate) );
    strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
    strncpy ( lCity, "Atlanta", sizeof ( lCity ) );
    assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lDate, "2002-12-05", sizeof ( lDate) );
    strncpy ( lStreet, "Sunset boulevard", sizeof ( lStreet ) );
    strncpy ( lCity, "Los Angeles", sizeof ( lCity ) );
    assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
    oss . str ( "" );
    assert ( c . print ( oss, "123456/7890" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

