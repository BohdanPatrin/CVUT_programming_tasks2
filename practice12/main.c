#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct TItem
{
    struct TItem * m_Next;
    struct TItem * m_Prev;
    int      m_Val;
} TITEM;

typedef struct TData
{
    TITEM * m_First;
    TITEM * m_Last;
} TDATA;

#endif /* __PROGTEST__ */

void insertStart ( TDATA * l, int x )
{
    /* TODO */
    TITEM *newItem = (TITEM *)malloc(sizeof(TITEM));
    newItem->m_Val = x;
    newItem->m_Next = l->m_First;
    newItem->m_Prev = NULL;
    if (l->m_First) l->m_First->m_Prev = newItem;

    l->m_First = newItem;

    if (!l->m_Last) l->m_Last = newItem; //if the list is empty
}
void insertEnd   ( TDATA * l, int x )
{
    /* TODO */
    TITEM *newItem = (TITEM *)malloc(sizeof(TITEM));
    newItem->m_Val = x;
    newItem->m_Next = NULL;
    newItem->m_Prev = l->m_Last;
    if (l->m_Last) l->m_Last->m_Next = newItem;

    l->m_Last = newItem;

    if (!l->m_First) l->m_First = newItem; //if the list is empty
}
int  removeMax   ( TDATA * l )
{
    /* TODO */
    if (!l->m_First) return 0; //if the list is empty

    int maxVal = l->m_First->m_Val;
    TITEM *curr = l->m_First;

    while (curr) { //find maximum value
        if (curr->m_Val > maxVal) maxVal = curr->m_Val;
        curr = curr->m_Next;
    }

    curr = l->m_First;
    int count = 0;

    while (curr) { //go through list again and remove all values with maximum value
        if (curr->m_Val == maxVal) {
            TITEM *toDelete = curr;

            if (curr->m_Prev) curr->m_Prev->m_Next = curr->m_Next;
            else l->m_First = curr->m_Next; //update the head

            if (curr->m_Next) curr->m_Next->m_Prev = curr->m_Prev;
            else l->m_Last = curr->m_Prev; //update the tail

            curr = curr->m_Next;
            free(toDelete);
            count++;
        }
        else curr = curr->m_Next;
    }
    return count;
}
void destroyAll  ( TDATA * l )
{
    /* TODO */
    TITEM *curr = l->m_First;
    while (curr) {
        TITEM *toDelete = curr;
        curr = curr->m_Next;
        free(toDelete);
    }
    l->m_First = NULL;
    l->m_Last = NULL;
}

#ifndef __PROGTEST__
int main ( void )
{
    TDATA a;
    a . m_First = a . m_Last = NULL;
    insertEnd ( &a, 1 );
    insertEnd ( &a, 2 );
    insertEnd ( &a, 1 );
    insertEnd ( &a, 3 );
    insertEnd ( &a, 4 );
    insertEnd ( &a, 2 );
    insertEnd ( &a, 5 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 1
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Val == 3
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 4
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 5
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
    assert ( removeMax ( &a ) == 1 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 1
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Val == 3
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 4
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
    destroyAll ( &a );

    a . m_First = a . m_Last = NULL;
    insertEnd ( &a, 1 );
    insertEnd ( &a, 2 );
    insertEnd ( &a, 3 );
    insertEnd ( &a, 0 );
    insertEnd ( &a, 2 );
    insertEnd ( &a, 3 );
    insertStart ( &a, 1 );
    insertStart ( &a, 2 );
    insertStart ( &a, 3 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 3
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 3
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 0
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 3
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
    assert ( removeMax ( &a ) == 3 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 2
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 0
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Val == 2
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next );
    assert ( removeMax ( &a ) == 3 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 1
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == 1
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == 0
             && a . m_First -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next );
    assert ( removeMax ( &a ) == 2 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == 0
             && a . m_First -> m_Next == NULL
             && a . m_First -> m_Prev == NULL
             && a . m_Last == a . m_First );
    destroyAll ( &a );

    a . m_First = a . m_Last = NULL;
    insertEnd ( &a, -1 );
    insertEnd ( &a, -2 );
    insertEnd ( &a, -10000 );
    insertEnd ( &a, -1 );
    insertEnd ( &a, -300 );
    assert ( removeMax ( &a ) == 2 );
    assert ( a . m_First != NULL
             && a . m_First -> m_Val == -2
             && a . m_First -> m_Next != NULL
             && a . m_First -> m_Prev == NULL
             && a . m_First -> m_Next -> m_Val == -10000
             && a . m_First -> m_Next -> m_Next != NULL
             && a . m_First -> m_Next -> m_Prev == a . m_First
             && a . m_First -> m_Next -> m_Next -> m_Val == -300
             && a . m_First -> m_Next -> m_Next -> m_Next == NULL
             && a . m_First -> m_Next -> m_Next -> m_Prev == a . m_First -> m_Next
             && a . m_Last == a . m_First -> m_Next -> m_Next );
    destroyAll ( &a );

    return 0;
}
#endif /* __PROGTEST__ */
