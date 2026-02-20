#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

constexpr int PHONE_DIGITS = 10;

typedef struct TNode
{
    char                               * m_Name;
    struct TNode                       * m_Child[PHONE_DIGITS];
} TNODE;

typedef struct
{
    TNODE                              * m_Root;
    int                                  m_Size;
} TPHONEBOOK;

#endif /* __PROGTEST__ */
bool isNumeric(const char *str) { //check string for having only numbers
    if (str == NULL || *str == '\0') return false; //empty string

    for (int i = 0; str[i] != '\0'; i++) // check for each character
        if (!isdigit((unsigned char)str[i]))
            return false;

    return true;
}

void freeNode(TNODE *node) { //delete node and all its children recursively
    if(!node) return; //base case

    for (int i = 0; i < PHONE_DIGITS; i++) //recursively free all children
        if (node->m_Child[i]) {
            freeNode(node->m_Child[i]);
            free(node->m_Child[i]);
            node->m_Child[i] = nullptr;
        }

    if (node->m_Name) {
        free(node->m_Name);
        node->m_Name = nullptr;
    }
}

bool delNode(TNODE *node, const char *phone, int depth) { //function to delete node and its parents if needed (but not children)
    if (!node) return false; // node doesn't exist, return false

    if (phone[depth] == '\0') {
        if (node->m_Name) {
            free(node->m_Name);
            node->m_Name = nullptr;
            return true;
        }
        else return false; // node doesn't have name - such number doesn't exist
    }

    int digit = phone[depth] - '0';

    //go to the next node
    if (!delNode(node->m_Child[digit], phone, depth + 1)) return false;

   //check node for having children
    TNODE *child = node->m_Child[digit];
    bool hasChildren = false;
    for (int i = 0; i < PHONE_DIGITS; i++)
        if (child->m_Child[i]) {
            hasChildren = true;
            break;
        }

    if (!hasChildren && !child->m_Name) { //if there are no valid children, erase them
        free(child);
        node->m_Child[digit] = nullptr;
    }

    return true;
}

// implementation of given functions

const char * findPhone ( TPHONEBOOK * book,
                         const char * phone )
{
    // if book is empty
    if(!book->m_Root) return nullptr;

    char* res = nullptr;
    struct TNode *current = book->m_Root;
    int i = 0;

    // fo through each digit in phone number until we get to the end
    while (phone[i]) {
        int digit = phone[i] - '0';
        if (!current->m_Child[digit]) break; // if the child for this digit doesn't exist, break the loop

        current = current->m_Child[digit]; // move to the child node
        if(current->m_Name) res = current->m_Name; //save the last name which has the same digits in the beginning
        i++;
    }

    return res;
}

bool         addPhone  ( TPHONEBOOK * book,
                         const char * phone,
                         const char * name )
{
    if (!isNumeric(phone) || !book) return false;

    // initialize the root of book as a placeholder if it doesn't exist
    if (!book->m_Root) book->m_Root = (struct TNode *)calloc(1, sizeof(struct TNode));

    struct TNode *current = book->m_Root;
    int i = 0;
    while (phone[i]) { // go through each digit in phone number until we get to the end
        int digit = phone[i] - '0';
        if (!current->m_Child[digit])// if the child for this digit doesn't exist, create it
            current->m_Child[digit] = (struct TNode *)calloc(1, sizeof(struct TNode));

        current = current->m_Child[digit]; // move to the child node
        i++;
    }

    if (current->m_Name) free(current->m_Name); //if it has name, we just free it, and later copy name (parameter) to it
    else book->m_Size++; // if the node has no name, it didn't count in the book size, so we increment it

    current->m_Name = strdup(name);
    return true;
}

bool delPhone(TPHONEBOOK *book, const char *phone) {
    if (!isNumeric(phone) || !book || !book->m_Root) return false;

    if (!delNode(book->m_Root, phone, 0)) return false; // call recursive function to delete node (but not its children)

    book->m_Size--;
    //check for case if it was the last number in book
    for(int i = 0; i < PHONE_DIGITS; i++)
        if(book->m_Root->m_Child[i])
            return true;

    free(book->m_Root);
    book->m_Root = nullptr;

    return true;
}


void         delBook   ( TPHONEBOOK * book )
{
    if(!book || !book->m_Root) return;

    freeNode(book->m_Root);
    book->m_Root = nullptr;
    book->m_Size = 0;
}

#ifndef __PROGTEST__
int main ()
{
    TPHONEBOOK b = { nullptr, 0 };
    char tmpStr[100];
    const char * name;
    assert ( addPhone ( &b, "420", "Czech Republic" ) );
    assert ( addPhone ( &b, "42022435", "Czech Republic CVUT" ) );
    assert ( addPhone ( &b, "421", "Slovak Republic" ) );
    assert ( addPhone ( &b, "44", "Great Britain" ) );
    strncpy ( tmpStr, "USA", sizeof ( tmpStr ) - 1 );
    assert ( addPhone ( &b, "1", tmpStr ) );
    strncpy ( tmpStr, "Guam", sizeof ( tmpStr ) - 1 );
    assert ( addPhone ( &b, "1671", tmpStr ) );
    assert ( addPhone ( &b, "44", "United Kingdom" ) );
    assert ( b . m_Size == 6 );
    assert ( ! b . m_Root -> m_Name );
    assert ( ! b . m_Root -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Name, "USA" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Name );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Name );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Name, "Guam" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Name );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Name, "Czech Republic" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[4] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Name, "Czech Republic CVUT" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[9] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Name, "Slovak Republic" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[3] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[4] -> m_Name, "United Kingdom" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[9] );
    name = findPhone ( &b, "420800123456" );  assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "420224351111" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    name = findPhone ( &b, "42022435" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    name = findPhone ( &b, "4202243" );  assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "420224343258985224" ); assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "42" );  assert ( !name );
    name = findPhone ( &b, "422" );  assert ( !name );
    name = findPhone ( &b, "4422" );  assert ( !strcmp ( name, "United Kingdom" ) );
    name = findPhone ( &b, "16713425245763" );  assert ( !strcmp ( name, "Guam" ) );
    name = findPhone ( &b, "123456789123456789" );  assert ( !strcmp ( name, "USA" ) );
    assert ( delPhone ( &b, "420" ) );
    assert ( delPhone ( &b, "421" ) );
    assert ( delPhone ( &b, "44" ) );
    assert ( delPhone ( &b, "1671" ) );
    assert ( !delPhone ( &b, "1672" ) );
    assert ( !delPhone ( &b, "1671" ) );
    name = findPhone ( &b, "16713425245763" );  assert ( !strcmp ( name, "USA" ) );
    name = findPhone ( &b, "4422" );  assert ( !name );
    name = findPhone ( &b, "420800123456" );  assert ( !name );
    name = findPhone ( &b, "420224351111" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    assert ( b . m_Size == 2 );
    assert ( ! b . m_Root -> m_Name );
    assert ( ! b . m_Root -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Name, "USA" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[4] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Name, "Czech Republic CVUT" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[9] );
    assert ( delPhone ( &b, "1" ) );
    assert ( delPhone ( &b, "42022435" ) );
    assert ( !addPhone ( &b, "12345XYZ", "test" ) );
    assert ( b . m_Size == 0 );
    assert ( ! b . m_Root );

    delBook ( &b );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
