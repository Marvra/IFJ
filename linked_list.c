/**
 * @file lexer.c
 * @author Jaroslav Doktor
 * @brief  source file for dll implementation of linked list used in expression parsing
 * @todo
 */

#include "linked_list.h"

/**
 * @brief Initialize linked list
 * @return pointer to linked list
 */
DLList* DLLInit() 
{
    DLList* list = malloc(sizeof(DLList) * 1);

	list->firstElement = NULL;
    list->lastElement = NULL;
    list->currentLength = 0;

    return list;
}

/**
 * @brief clear linked list
 */
void DLLDispose( DLList *list )
{
	DLLElementPtr tempElement;
    while (list->firstElement != NULL)
    {
        tempElement = list->firstElement;
        list->firstElement = list->firstElement->nextElement;
        free(tempElement);
    }
    list->firstElement = NULL;
    list->lastElement = NULL;
    list->currentLength = 0;
}

/**
 * @brief Insert new element at the end of the list
 */
int DLLInsertLast( DLList *list, precTableTerm_t termType)
{
	DLLElementPtr newElement = malloc(sizeof(DLLElementPtr) * 1);
    if (newElement == NULL)
    {
        return -1;
    }
    else
    {
        newElement->data = malloc(sizeof(DLLData) * 1);
        newElement->data->termType = termType;
        newElement->nextElement = NULL;
        newElement->previousElement = list->lastElement;

        if (list->firstElement == NULL) 
        {
            list->firstElement = newElement;
        }
        if (list->lastElement != NULL) 
        {
            list->lastElement->nextElement = newElement;
        }
        
        list->lastElement = newElement;
        list->currentLength += 1;
    }
    return 0;
}

/**
 * @brief Delete last element from the list
 */
DLLData DLLDeleteLast( DLList *list ) {
    if(list->lastElement == NULL)
    {
        return NULL;
    }

    DLLData data = list->lastElement->data;

	if (list->lastElement != NULL)
    {
        if (list->firstElement == list->lastElement)
        {
            DLLElementPtr tempElement;
            tempElement = list->lastElement;
            list->firstElement = NULL;
            list->lastElement = NULL;
            free(tempElement);
            list->currentLength -= 1;
        } 
        else
        {
            DLLElementPtr tempElement;
            tempElement = list->lastElement;
            list->lastElement = list->lastElement->previousElement;
            list->lastElement->nextElement = NULL;
            free(tempElement);
            list->currentLength -= 1;
        }
    }
    return data;
}