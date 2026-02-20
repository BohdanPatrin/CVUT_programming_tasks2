#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

class CFileSystemItem { //parent class
public:
    virtual ~CFileSystemItem() = default;
    virtual size_t Size() const = 0;
    virtual CFileSystemItem * Clone() const = 0;
    virtual void Print(std::ostream & os, const std::string & name) const = 0;
};

class CFile : public CFileSystemItem {
public:
    CFile(const std::string & hash, size_t size): m_Hash(hash), m_Size(size) {}

    size_t Size() const override {
        return m_Size;
    }

    CFile & Change(const std::string & hash, size_t size) {
        m_Hash = hash;
        m_Size = size;
        return *this;
    }

    CFileSystemItem * Clone() const override {
        return new CFile(*this);
    }

    void Print(std::ostream & os, const std::string & name) const override {
        os << m_Size << "\t" << name << " " << m_Hash << "\n";
    }

private:
    std::string m_Hash;
    size_t m_Size;
};

class CLink : public CFileSystemItem {
public:
    CLink(const std::string & path) : m_Path(path) {}

    size_t Size() const override {
        return m_Path.size() + 1;
    }

    CLink & Change(const std::string & path) {
        m_Path = path;
        return *this;
    }

    CFileSystemItem * Clone() const override {
        return new CLink(*this);
    }

    void Print(std::ostream & os, const std::string & name) const override {
        os << Size() << "\t" << name << " -> " << m_Path << "\n";
    }

private:
    std::string m_Path;
};

class CDirectory : public CFileSystemItem {
public:
    CDirectory() = default;
    CDirectory(const CDirectory & src) {
        for (const auto & [name, item] : src.m_Items)
            m_Items[name] = std::unique_ptr<CFileSystemItem>(item->Clone());
    }

    CDirectory & operator=(const CDirectory & src) {
        if (this == &src) return *this;
        m_Items.clear();
        for (const auto & [name, item] : src.m_Items)
            m_Items[name] = std::unique_ptr<CFileSystemItem>(item->Clone());

        return *this;
    }

    size_t Size() const override {
        size_t total = 0;
        for (const auto & [name, item] : m_Items)
            total += name.size() + item->Size();

        return total;
    }

    CDirectory & Change(const std::string & name, const CFileSystemItem & item) {
        m_Items[name] = std::unique_ptr<CFileSystemItem>(item.Clone());
        return *this;
    }

    CDirectory & Change(const std::string & name, std::nullptr_t) {
        m_Items.erase(name);
        return *this;
    }

    CFileSystemItem & Get(const std::string & name) {
        auto it = m_Items.find(name);
        if (it == m_Items.end()) throw std::out_of_range("File not found");
        return *(it->second);
    }

    const CFileSystemItem & Get(const std::string & name) const {
        auto it = m_Items.find(name);
        if (it == m_Items.end()) throw std::out_of_range("File not found");
        return *(it->second);
    }

    void Print(std::ostream & os, const std::string & name) const override {
        os << Size() << "\t" << name << "/\n";
    }

    friend std::ostream & operator<<(std::ostream & os, const CDirectory & dir) {
        std::map<std::string, const CFileSystemItem *> sorted;
        for (const auto & [name, item] : dir.m_Items)
            sorted[name] = item.get();

        for (const auto & [name, item] : sorted)
            item->Print(os, name);

        return os;
    }

    CFileSystemItem * Clone() const override {
        return new CDirectory(*this);
    }

private:
    std::map<std::string, std::unique_ptr<CFileSystemItem>> m_Items;
};

#ifndef __PROGTEST__
int main ()
{
    CDirectory root;
    stringstream sout;

    root.Change("file.txt", CFile("jhwadkhawkdhajwdhawhdaw=", 1623))
            .Change("file.ln", CLink("").Change("file.txt"))
            .Change("folder", CDirectory()
                    .Change("fileA.txt", CFile("", 0).Change("skjdajdakljdljadkjwaljdlaw=", 1713))
                    .Change("fileB.txt", CFile("kadwjkwajdwhoiwhduqwdqwuhd=", 71313))
                    .Change("fileC.txt", CFile("aihdqhdqudqdiuwqhdquwdqhdi=", 8193))
            );

    sout.str( "" );
    sout << root;
    assert( sout.str() ==
            "9\tfile.ln -> file.txt\n"
            "1623\tfile.txt jhwadkhawkdhajwdhawhdaw=\n"
            "81246\tfolder/\n" );
    assert( root.Size() == 82899 );

    string filename = "folder";
    const CDirectory & inner = dynamic_cast<const CDirectory &>( root.Get( filename ) );

    sout.str( "" );
    sout << inner;
    assert( sout.str() ==
            "1713\tfileA.txt skjdajdakljdljadkjwaljdlaw=\n"
            "71313\tfileB.txt kadwjkwajdwhoiwhduqwdqwuhd=\n"
            "8193\tfileC.txt aihdqhdqudqdiuwqhdquwdqhdi=\n" );
    assert( inner.Size() == 81246 );

    return 0;
}
#endif /* __PROGTEST__ */

