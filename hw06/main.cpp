#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST__ */

//parent class
class CCell {
public:
    virtual ~CCell() {}
    virtual std::vector<std::string> display() const = 0; //outputs the content of cell as the vector of strings
    virtual std::shared_ptr<CCell> clone() const = 0; //returns the deep copy of the cell
    virtual bool operator==(const CCell& other) const = 0;
    virtual bool operator!=(const CCell & other) const {
        return !(*this == other);
    }
};

class CText : public CCell
{
public:
    enum AlignType {
        ALIGN_LEFT,
        ALIGN_RIGHT
    };

    bool isLeft(){ //needed for padding
        return m_Align == ALIGN_LEFT;
    }

    CText(const std::string& text, AlignType align): m_Text(text), m_Align(align){};

    void setText(const std::string& text){
        m_Text = text;
    }

    std::vector<std::string> display() const override{
        std::vector<std::string> res;
        std::istringstream ss(m_Text);
        std::string line;
        while (std::getline(ss, line))
            res.push_back(line);
        return res;
    }

    std::shared_ptr<CCell> clone() const override{
        return std::make_shared<CText>(*this);
    }

    bool operator==(const CCell& other) const override{
        const CText* p = dynamic_cast<const CText*>(&other);
        if (!p) return false;
        return this->m_Text == p->m_Text && this->m_Align == p->m_Align;
    }
private:
    std::string m_Text;
    AlignType m_Align;
};

class CEmpty: public CCell
{
public:
    CEmpty() = default;
    ~CEmpty() override = default;

    std::vector<std::string> display() const override {
        return {""};
    }

    std::shared_ptr<CCell> clone() const override {
        return std::make_shared<CEmpty>(*this);
    }

    bool operator==(const CCell& other) const override {
        return dynamic_cast<const CEmpty*>(&other) != nullptr;
    }
};

class CImage : public CCell
{
public:
    CImage() = default;

    CImage& addRow(const std::string& row){
        m_Image.push_back(row);
        return *this;
    }

    std::vector<std::string> display() const override{
        return m_Image;
    }

    std::shared_ptr<CCell> clone() const override{
        return std::make_shared<CImage>(*this);
    }

    bool operator==(const CCell& other) const override{
        const CImage* p = dynamic_cast<const CImage*>(&other);
        if (!p) return false;
        if(m_Image.size() != p->m_Image.size()) return false;
        for(size_t i = 0; i < m_Image.size(); i++)
            if(m_Image[i] != p->m_Image[i]) return false;

        return true;
    }

private:
    std::vector<std::string> m_Image;
};

class CTable :public CCell
{
public:
    friend std::ostream& operator<<(std::ostream& os, const CTable& table);

    CTable(int rows, int cols): m_Rows(rows), m_Cols(cols),
    m_Cells(rows, std::vector<std::shared_ptr<CCell>>(cols)) {
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c)
                m_Cells[r][c] = std::make_shared<CEmpty>();
    }

    CTable(const CTable& other)
            : m_Rows(other.m_Rows), m_Cols(other.m_Cols)
    {
        m_Cells.resize(m_Rows, std::vector<std::shared_ptr<CCell>>(m_Cols));
        for (int r = 0; r < m_Rows; ++r)
            for (int c = 0; c < m_Cols; ++c)
                m_Cells[r][c] = other.m_Cells[r][c]->clone();
    }

    CTable& operator=(const CTable& other) {
        if (this == &other)
            return *this;

        m_Rows = other.m_Rows;
        m_Cols = other.m_Cols;
        m_Cells.resize(m_Rows, std::vector<std::shared_ptr<CCell>>(m_Cols));

        for (int r = 0; r < m_Rows; ++r)
            for (int c = 0; c < m_Cols; ++c)
                m_Cells[r][c] = other.m_Cells[r][c]->clone();

        return *this;
    }

    CCell& getCell(int row, int col) const{
        return *m_Cells[row][col].get();
    }

    void setCell(int row, int col, const CCell& newContent){
        m_Cells[row][col] = newContent.clone();
    }

    std::vector<std::string> display() const override{
        //using the operator << for generating result - vector of strings
        std::ostringstream oss;
        oss << *this;

        std::vector<std::string> result;
        std::string line;
        std::istringstream iss(oss.str());

        while (std::getline(iss, line)) {
            result.push_back(line);
        }

        return result;
    }

    std::shared_ptr<CCell> clone() const override{
        auto newTable = std::make_shared<CTable>(m_Rows, m_Cols);
        for (int i = 0; i < m_Rows; ++i)
            for (int j = 0; j < m_Cols; ++j)
                newTable->setCell(i, j, *((m_Cells[i][j]).get())->clone());
        return newTable;
    }

    bool operator==(const CCell& other) const override{
        const CTable* p = dynamic_cast<const CTable*>(&other);
        if (!p) return false;
        if (m_Rows != p->m_Rows || m_Cols != p->m_Cols) return false;

        for (int i = 0; i < m_Rows; i++)
            for (int j = 0; j < m_Cols; j++)
                if (!(*m_Cells[i][j] == *p->m_Cells[i][j]))
                    return false;

        return true;
    }

private:
    int m_Rows;
    int m_Cols;
    std::vector<std::vector<std::shared_ptr<CCell>>> m_Cells; //2D vector of smart pointers to cells
};

std::ostream& operator<<(std::ostream& os, const CTable& table){
    //needed in case the row/column fully consists of CEmpty
    std::vector<bool> empty_row(table.m_Rows, true);
    std::vector<bool> empty_col(table.m_Cols, true);

    //storing the content of each cell and computing max height/width
    std::vector<std::vector<std::vector<std::string>>> displayedCells(table.m_Rows, std::vector<std::vector<std::string>>(table.m_Cols));
    std::vector<int> max_width(table.m_Cols, 0);
    std::vector<int> max_height(table.m_Rows, 0);
    for (int r = 0; r < table.m_Rows; ++r)
        for (int c = 0; c < table.m_Cols; ++c) {
            auto& cell = table.m_Cells[r][c];
            displayedCells[r][c] = cell->display();

            if (!std::dynamic_pointer_cast<CEmpty>(cell)) {
                empty_row[r] = false;
                empty_col[c] = false;
            }
        }

    for (int r = 0; r < table.m_Rows; ++r) {
        if (empty_row[r]) {
            max_height[r] = 0;
        } else {
            for (int c = 0; c < table.m_Cols; ++c)
                if ((int)displayedCells[r][c].size() > max_height[r])
                    max_height[r] = (int)displayedCells[r][c].size();
        }
    }

    for (int c = 0; c < table.m_Cols; ++c) {
        if (empty_col[c]) {
            max_width[c] = 0;
        } else {
            for (int r = 0; r < table.m_Rows; ++r)
                for (const std::string& line : displayedCells[r][c])
                    if ((int)line.length() > max_width[c])
                        max_width[c] = (int)line.length();
        }
    }

    //padding all cells
    std::vector<std::vector<std::vector<std::string>>> paddedCells(table.m_Rows, std::vector<std::vector<std::string>>(table.m_Cols));
    for (int r = 0; r < table.m_Rows; ++r)
        for (int c = 0; c < table.m_Cols; ++c) {
            auto& cell = table.m_Cells[r][c];
            const auto& content = displayedCells[r][c];
            int width = max_width[c];
            int height = max_height[r];

            std::vector<std::string> padded(height, std::string(width, ' '));

            if (auto text = std::dynamic_pointer_cast<CText>(cell))
                for (size_t i = 0; i < content.size(); ++i) {
                    const std::string& line = content[i];
                    if (text->isLeft())
                        padded[i].replace(0, line.size(), line);
                    else
                        padded[i].replace(width - line.size(), line.size(), line);
                }
            else if (std::dynamic_pointer_cast<CImage>(cell)) {
                //if the extra space is not even, the image will be placed one character to the left/up
                int vExtra = height - (int)content.size();
                int hExtra = width  - (int)(content.empty() ? 0 : content[0].size());

                int vOffset = vExtra / 2;
                int hOffset = hExtra / 2;

                for (size_t i = 0; i < content.size(); ++i) {
                    padded[vOffset + i].replace(hOffset, content[i].size(), content[i]);
                }
            }
            else if(!empty_row[r])//CEmpty or CTable
                for (size_t i = 0; i < content.size(); ++i)
                    padded[i].replace(0, content[i].size(), content[i]);

            paddedCells[r][c] = std::move(padded);
        }


    //printing whole table
    for(int i = 0; i < table.m_Rows; i++){
        //print the upper border
        for(int k = 0; k < table.m_Cols; k++){
            os<<'+';
            for(int m = 0; m < max_width[k]; m++) os << '-';
        }
        os <<"+\n";

        if (max_height[i] == 0) //if row consists of CEmpty - skip it
            continue;

        //print content of cell
        for (int h = 0; h < max_height[i]; h++)
           for(int j = 0; j < table.m_Cols; j++) {
                     os << '|' << paddedCells[i][j][h];

                     if(j == table.m_Cols - 1) os << "|\n";
        }

    }

    //print the last line
    for(int k = 0; k < table.m_Cols; k++){
        os<<'+';
        for(int m = 0; m < max_width[k]; m++) os << '-';
    }
    os <<"+\n";

    return os;
}

#ifndef __PROGTEST__
int main ()
{
    std::ostringstream oss;
    CTable t0 ( 3, 2 );
    t0 . setCell ( 0, 0, CText ( "Hello,\n"
                                 "Hello Kitty", CText::ALIGN_LEFT ) );
    t0 . setCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
    t0 . setCell ( 2, 0, CText ( "Bye,\n"
                                 "Hello Kitty", CText::ALIGN_RIGHT ) );
    t0 . setCell ( 1, 1, CImage ()
            . addRow ( "###                   " )
            . addRow ( "#  #                  " )
            . addRow ( "#  # # ##   ###    ###" )
            . addRow ( "###  ##    #   #  #  #" )
            . addRow ( "#    #     #   #  #  #" )
            . addRow ( "#    #     #   #  #  #" )
            . addRow ( "#    #      ###    ###" )
            . addRow ( "                     #" )
            . addRow ( "                   ## " )
            . addRow ( "                      " )
            . addRow ( " #    ###   ###   #   " )
            . addRow ( "###  #   # #     ###  " )
            . addRow ( " #   #####  ###   #   " )
            . addRow ( " #   #         #  #   " )
            . addRow ( "  ##  ###   ###    ## " ) );
    t0 . setCell ( 2, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+--------------------------+----------------------+\n"
             "|Hello,                    |                      |\n"
             "|Hello Kitty               |                      |\n"
             "+--------------------------+----------------------+\n"
             "|Lorem ipsum dolor sit amet|###                   |\n"
             "|                          |#  #                  |\n"
             "|                          |#  # # ##   ###    ###|\n"
             "|                          |###  ##    #   #  #  #|\n"
             "|                          |#    #     #   #  #  #|\n"
             "|                          |#    #     #   #  #  #|\n"
             "|                          |#    #      ###    ###|\n"
             "|                          |                     #|\n"
             "|                          |                   ## |\n"
             "|                          |                      |\n"
             "|                          | #    ###   ###   #   |\n"
             "|                          |###  #   # #     ###  |\n"
             "|                          | #   #####  ###   #   |\n"
             "|                          | #   #         #  #   |\n"
             "|                          |  ##  ###   ###    ## |\n"
             "+--------------------------+----------------------+\n"
             "|                      Bye,|                      |\n"
             "|               Hello Kitty|                      |\n"
             "+--------------------------+----------------------+\n" );
    t0 . setCell ( 0, 1, t0 . getCell ( 1, 1 ) );
    t0 . setCell ( 2, 1, CImage ()
            . addRow ( "*****   *      *  *      ******* ******  *" )
            . addRow ( "*    *  *      *  *      *            *  *" )
            . addRow ( "*    *  *      *  *      *           *   *" )
            . addRow ( "*    *  *      *  *      *****      *    *" )
            . addRow ( "****    *      *  *      *         *     *" )
            . addRow ( "*  *    *      *  *      *        *       " )
            . addRow ( "*   *   *      *  *      *       *       *" )
            . addRow ( "*    *    *****   ****** ******* ******  *" ) );
    dynamic_cast<CText &> ( t0 . getCell ( 1, 0 ) ) . setText ( "Lorem ipsum dolor sit amet,\n"
                                                                "consectetur adipiscing\n"
                                                                "elit. Curabitur scelerisque\n"
                                                                "lorem vitae lectus cursus,\n"
                                                                "vitae porta ante placerat. Class aptent taciti\n"
                                                                "sociosqu ad litora\n"
                                                                "torquent per\n"
                                                                "conubia nostra,\n"
                                                                "per inceptos himenaeos.\n"
                                                                "\n"
                                                                "Donec tincidunt augue\n"
                                                                "sit amet metus\n"
                                                                "pretium volutpat.\n"
                                                                "Donec faucibus,\n"
                                                                "ante sit amet\n"
                                                                "luctus posuere,\n"
                                                                "mauris tellus" );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    CTable t1 ( t0 );
    t1 . setCell ( 1, 0, CEmpty () );
    t1 . setCell ( 1, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+-----------+------------------------------------------+\n"
             "|Hello,     |          ###                             |\n"
             "|Hello Kitty|          #  #                            |\n"
             "|           |          #  # # ##   ###    ###          |\n"
             "|           |          ###  ##    #   #  #  #          |\n"
             "|           |          #    #     #   #  #  #          |\n"
             "|           |          #    #     #   #  #  #          |\n"
             "|           |          #    #      ###    ###          |\n"
             "|           |                               #          |\n"
             "|           |                             ##           |\n"
             "|           |                                          |\n"
             "|           |           #    ###   ###   #             |\n"
             "|           |          ###  #   # #     ###            |\n"
             "|           |           #   #####  ###   #             |\n"
             "|           |           #   #         #  #             |\n"
             "|           |            ##  ###   ###    ##           |\n"
             "+-----------+------------------------------------------+\n"
             "+-----------+------------------------------------------+\n"
             "|       Bye,|*****   *      *  *      ******* ******  *|\n"
             "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|           |*    *  *      *  *      *           *   *|\n"
             "|           |*    *  *      *  *      *****      *    *|\n"
             "|           |****    *      *  *      *         *     *|\n"
             "|           |*  *    *      *  *      *        *       |\n"
             "|           |*   *   *      *  *      *       *       *|\n"
             "|           |*    *    *****   ****** ******* ******  *|\n"
             "+-----------+------------------------------------------+\n" );
    t1 = t0;
    t1 . setCell ( 0, 0, CEmpty () );
    t1 . setCell ( 1, 1, CImage ()
            . addRow ( "  ********                    " )
            . addRow ( " **********                   " )
            . addRow ( "**        **                  " )
            . addRow ( "**             **        **   " )
            . addRow ( "**             **        **   " )
            . addRow ( "***         ********  ********" )
            . addRow ( "****        ********  ********" )
            . addRow ( "****           **        **   " )
            . addRow ( "****           **        **   " )
            . addRow ( "****      **                  " )
            . addRow ( " **********                   " )
            . addRow ( "  ********                    " ) );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                              |          ###                             |\n"
             "|                                              |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |                                          |\n"
             "|elit. Curabitur scelerisque                   |        ********                          |\n"
             "|lorem vitae lectus cursus,                    |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
             "|sociosqu ad litora                            |      **             **        **         |\n"
             "|torquent per                                  |      **             **        **         |\n"
             "|conubia nostra,                               |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
             "|                                              |      ****           **        **         |\n"
             "|Donec tincidunt augue                         |      ****           **        **         |\n"
             "|sit amet metus                                |      ****      **                        |\n"
             "|pretium volutpat.                             |       **********                         |\n"
             "|Donec faucibus,                               |        ********                          |\n"
             "|ante sit amet                                 |                                          |\n"
             "|luctus posuere,                               |                                          |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    CTable t2 ( 2, 2 );
    t2 . setCell ( 0, 0, CText ( "OOP", CText::ALIGN_LEFT ) );
    t2 . setCell ( 0, 1, CText ( "Encapsulation", CText::ALIGN_LEFT ) );
    t2 . setCell ( 1, 0, CText ( "Polymorphism", CText::ALIGN_LEFT ) );
    t2 . setCell ( 1, 1, CText ( "Inheritance", CText::ALIGN_LEFT ) );
    oss . str ("");
    oss . clear ();
    oss << t2;
    assert ( oss . str () ==
             "+------------+-------------+\n"
             "|OOP         |Encapsulation|\n"
             "+------------+-------------+\n"
             "|Polymorphism|Inheritance  |\n"
             "+------------+-------------+\n" );
    t1 . setCell ( 0, 0, t2 );
    dynamic_cast<CText &> ( t2 . getCell ( 0, 0 ) ) . setText ( "Object Oriented Programming" );
    oss . str ("");
    oss . clear ();
    oss << t2;
    assert ( oss . str () ==
             "+---------------------------+-------------+\n"
             "|Object Oriented Programming|Encapsulation|\n"
             "+---------------------------+-------------+\n"
             "|Polymorphism               |Inheritance  |\n"
             "+---------------------------+-------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|+------------+-------------+                  |          ###                             |\n"
             "||OOP         |Encapsulation|                  |          #  #                            |\n"
             "|+------------+-------------+                  |          #  # # ##   ###    ###          |\n"
             "||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          |\n"
             "|+------------+-------------+                  |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |                                          |\n"
             "|elit. Curabitur scelerisque                   |        ********                          |\n"
             "|lorem vitae lectus cursus,                    |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
             "|sociosqu ad litora                            |      **             **        **         |\n"
             "|torquent per                                  |      **             **        **         |\n"
             "|conubia nostra,                               |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
             "|                                              |      ****           **        **         |\n"
             "|Donec tincidunt augue                         |      ****           **        **         |\n"
             "|sit amet metus                                |      ****      **                        |\n"
             "|pretium volutpat.                             |       **********                         |\n"
             "|Donec faucibus,                               |        ********                          |\n"
             "|ante sit amet                                 |                                          |\n"
             "|luctus posuere,                               |                                          |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    assert ( t0 != t1 );
    assert ( !( t0 == t1 ) );
    assert ( t0 . getCell ( 1, 1 ) == t0 . getCell ( 0, 1 ) );
    assert ( ! ( t0 . getCell ( 1, 1 ) != t0 . getCell ( 0, 1 ) ) );
    assert ( t0 . getCell ( 0, 0 ) != t0 . getCell ( 0, 1 ) );
    assert ( ! ( t0 . getCell ( 0, 0 ) == t0 . getCell ( 0, 1 ) ) );
    t1 . setCell ( 0, 0, t1 );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "||+------------+-------------+                  |          ###                             ||                                          |\n"
             "|||OOP         |Encapsulation|                  |          #  #                            ||                                          |\n"
             "||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          |\n"
             "|||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          |\n"
             "||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          |\n"
             "||                                              |          #    #     #   #  #  #          ||                                          |\n"
             "||                                              |          #    #      ###    ###          ||                                          |\n"
             "||                                              |                               #          ||                                          |\n"
             "||                                              |                             ##           ||                                          |\n"
             "||                                              |                                          ||                                          |\n"
             "||                                              |           #    ###   ###   #             ||                                          |\n"
             "||                                              |          ###  #   # #     ###            ||                                          |\n"
             "||                                              |           #   #####  ###   #             ||                                          |\n"
             "||                                              |           #   #         #  #             ||          ###                             |\n"
             "||                                              |            ##  ###   ###    ##           ||          #  #                            |\n"
             "|+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          |\n"
             "||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          |\n"
             "||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          |\n"
             "||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          |\n"
             "||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          |\n"
             "||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          |\n"
             "||sociosqu ad litora                            |      **             **        **         ||                             ##           |\n"
             "||torquent per                                  |      **             **        **         ||                                          |\n"
             "||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             |\n"
             "||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            |\n"
             "||                                              |      ****           **        **         ||           #   #####  ###   #             |\n"
             "||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             |\n"
             "||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           |\n"
             "||pretium volutpat.                             |       **********                         ||                                          |\n"
             "||Donec faucibus,                               |        ********                          ||                                          |\n"
             "||ante sit amet                                 |                                          ||                                          |\n"
             "||luctus posuere,                               |                                          ||                                          |\n"
             "||mauris tellus                                 |                                          ||                                          |\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "||                                          Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
             "||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
             "||                                              |*    *  *      *  *      *           *   *||                                          |\n"
             "||                                              |*    *  *      *  *      *****      *    *||                                          |\n"
             "||                                              |****    *      *  *      *         *     *||                                          |\n"
             "||                                              |*  *    *      *  *      *        *       ||                                          |\n"
             "||                                              |*   *   *      *  *      *       *       *||                                          |\n"
             "||                                              |*    *    *****   ****** ******* ******  *||                                          |\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                                                                |                                          |\n"
             "|consectetur adipiscing                                                                     |                                          |\n"
             "|elit. Curabitur scelerisque                                                                |        ********                          |\n"
             "|lorem vitae lectus cursus,                                                                 |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti                                             |      **        **                        |\n"
             "|sociosqu ad litora                                                                         |      **             **        **         |\n"
             "|torquent per                                                                               |      **             **        **         |\n"
             "|conubia nostra,                                                                            |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                                                                    |      ****        ********  ********      |\n"
             "|                                                                                           |      ****           **        **         |\n"
             "|Donec tincidunt augue                                                                      |      ****           **        **         |\n"
             "|sit amet metus                                                                             |      ****      **                        |\n"
             "|pretium volutpat.                                                                          |       **********                         |\n"
             "|Donec faucibus,                                                                            |        ********                          |\n"
             "|ante sit amet                                                                              |                                          |\n"
             "|luctus posuere,                                                                            |                                          |\n"
             "|mauris tellus                                                                              |                                          |\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|                                                                                       Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                                                                Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                                                                           |*    *  *      *  *      *           *   *|\n"
             "|                                                                                           |*    *  *      *  *      *****      *    *|\n"
             "|                                                                                           |****    *      *  *      *         *     *|\n"
             "|                                                                                           |*  *    *      *  *      *        *       |\n"
             "|                                                                                           |*   *   *      *  *      *       *       *|\n"
             "|                                                                                           |*    *    *****   ****** ******* ******  *|\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n" );
    t1 . setCell ( 0, 0, t1 );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
             "|||+------------+-------------+                  |          ###                             ||                                          ||                                          |\n"
             "||||OOP         |Encapsulation|                  |          #  #                            ||                                          ||                                          |\n"
             "|||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          ||                                          |\n"
             "||||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          ||                                          |\n"
             "|||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          ||                                          |\n"
             "|||                                              |          #    #     #   #  #  #          ||                                          ||                                          |\n"
             "|||                                              |          #    #      ###    ###          ||                                          ||                                          |\n"
             "|||                                              |                               #          ||                                          ||                                          |\n"
             "|||                                              |                             ##           ||                                          ||                                          |\n"
             "|||                                              |                                          ||                                          ||                                          |\n"
             "|||                                              |           #    ###   ###   #             ||                                          ||                                          |\n"
             "|||                                              |          ###  #   # #     ###            ||                                          ||                                          |\n"
             "|||                                              |           #   #####  ###   #             ||                                          ||                                          |\n"
             "|||                                              |           #   #         #  #             ||          ###                             ||                                          |\n"
             "|||                                              |            ##  ###   ###    ##           ||          #  #                            ||                                          |\n"
             "||+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          ||                                          |\n"
             "|||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          ||                                          |\n"
             "|||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          ||                                          |\n"
             "|||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          ||                                          |\n"
             "|||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          ||                                          |\n"
             "|||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          ||                                          |\n"
             "|||sociosqu ad litora                            |      **             **        **         ||                             ##           ||                                          |\n"
             "|||torquent per                                  |      **             **        **         ||                                          ||                                          |\n"
             "|||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             ||                                          |\n"
             "|||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            ||                                          |\n"
             "|||                                              |      ****           **        **         ||           #   #####  ###   #             ||                                          |\n"
             "|||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             ||                                          |\n"
             "|||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           ||          ###                             |\n"
             "|||pretium volutpat.                             |       **********                         ||                                          ||          #  #                            |\n"
             "|||Donec faucibus,                               |        ********                          ||                                          ||          #  # # ##   ###    ###          |\n"
             "|||ante sit amet                                 |                                          ||                                          ||          ###  ##    #   #  #  #          |\n"
             "|||luctus posuere,                               |                                          ||                                          ||          #    #     #   #  #  #          |\n"
             "|||mauris tellus                                 |                                          ||                                          ||          #    #     #   #  #  #          |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||          #    #      ###    ###          |\n"
             "|||                                          Bye,|*****   *      *  *      ******* ******  *||                                          ||                               #          |\n"
             "|||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          ||                             ##           |\n"
             "|||                                              |*    *  *      *  *      *           *   *||                                          ||                                          |\n"
             "|||                                              |*    *  *      *  *      *****      *    *||                                          ||           #    ###   ###   #             |\n"
             "|||                                              |****    *      *  *      *         *     *||                                          ||          ###  #   # #     ###            |\n"
             "|||                                              |*  *    *      *  *      *        *       ||                                          ||           #   #####  ###   #             |\n"
             "|||                                              |*   *   *      *  *      *       *       *||                                          ||           #   #         #  #             |\n"
             "|||                                              |*    *    *****   ****** ******* ******  *||                                          ||            ##  ###   ###    ##           |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||Lorem ipsum dolor sit amet,                                                                |                                          ||                                          |\n"
             "||consectetur adipiscing                                                                     |                                          ||                                          |\n"
             "||elit. Curabitur scelerisque                                                                |        ********                          ||                                          |\n"
             "||lorem vitae lectus cursus,                                                                 |       **********                         ||                                          |\n"
             "||vitae porta ante placerat. Class aptent taciti                                             |      **        **                        ||                                          |\n"
             "||sociosqu ad litora                                                                         |      **             **        **         ||                                          |\n"
             "||torquent per                                                                               |      **             **        **         ||                                          |\n"
             "||conubia nostra,                                                                            |      ***         ********  ********      ||                                          |\n"
             "||per inceptos himenaeos.                                                                    |      ****        ********  ********      ||                                          |\n"
             "||                                                                                           |      ****           **        **         ||                                          |\n"
             "||Donec tincidunt augue                                                                      |      ****           **        **         ||                                          |\n"
             "||sit amet metus                                                                             |      ****      **                        ||                                          |\n"
             "||pretium volutpat.                                                                          |       **********                         ||                                          |\n"
             "||Donec faucibus,                                                                            |        ********                          ||                                          |\n"
             "||ante sit amet                                                                              |                                          ||                                          |\n"
             "||luctus posuere,                                                                            |                                          ||                                          |\n"
             "||mauris tellus                                                                              |                                          ||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||                                                                                       Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
             "||                                                                                Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
             "||                                                                                           |*    *  *      *  *      *           *   *||                                          |\n"
             "||                                                                                           |*    *  *      *  *      *****      *    *||                                          |\n"
             "||                                                                                           |****    *      *  *      *         *     *||                                          |\n"
             "||                                                                                           |*  *    *      *  *      *        *       ||                                          |\n"
             "||                                                                                           |*   *   *      *  *      *       *       *||                                          |\n"
             "||                                                                                           |*    *    *****   ****** ******* ******  *||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                                                                                                             |                                          |\n"
             "|consectetur adipiscing                                                                                                                  |                                          |\n"
             "|elit. Curabitur scelerisque                                                                                                             |        ********                          |\n"
             "|lorem vitae lectus cursus,                                                                                                              |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti                                                                                          |      **        **                        |\n"
             "|sociosqu ad litora                                                                                                                      |      **             **        **         |\n"
             "|torquent per                                                                                                                            |      **             **        **         |\n"
             "|conubia nostra,                                                                                                                         |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                                                                                                                 |      ****        ********  ********      |\n"
             "|                                                                                                                                        |      ****           **        **         |\n"
             "|Donec tincidunt augue                                                                                                                   |      ****           **        **         |\n"
             "|sit amet metus                                                                                                                          |      ****      **                        |\n"
             "|pretium volutpat.                                                                                                                       |       **********                         |\n"
             "|Donec faucibus,                                                                                                                         |        ********                          |\n"
             "|ante sit amet                                                                                                                           |                                          |\n"
             "|luctus posuere,                                                                                                                         |                                          |\n"
             "|mauris tellus                                                                                                                           |                                          |\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|                                                                                                                                    Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                                                                                                             Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                                                                                                                        |*    *  *      *  *      *           *   *|\n"
             "|                                                                                                                                        |*    *  *      *  *      *****      *    *|\n"
             "|                                                                                                                                        |****    *      *  *      *         *     *|\n"
             "|                                                                                                                                        |*  *    *      *  *      *        *       |\n"
             "|                                                                                                                                        |*   *   *      *  *      *       *       *|\n"
             "|                                                                                                                                        |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n" );




    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

