#include "scanll.h"

SCAN* scan_create ()
{
    SCAN* new_scan = (SCAN*) malloc (1*sizeof(SCAN));
    new_scan->head = NULL;
    new_scan->tail = NULL;
    new_scan->length = 0;
    new_scan->curr_id = 0;
    return new_scan;
}

void scan_add (SCAN* inscan, float inph)
{
    if (inscan->head) {
        SCAN_NODE* new_node = (SCAN_NODE*) malloc (1*sizeof(SCAN_NODE));
        new_node->id = inscan->curr_id;
        new_node->ph = inph;
        new_node->next = NULL;
        inscan->tail->next = new_node;
        inscan->tail = new_node;
        inscan->curr_id++;
        inscan->length++;
    } else {
        SCAN_NODE* new_node = (SCAN_NODE*) malloc (1*sizeof(SCAN_NODE));
        new_node->id = inscan->length;
        new_node->ph = inph;
        inscan->head = new_node;
        inscan->tail = new_node;
        inscan->curr_id++;
        inscan->length++;
    }
}

void scan_remove_id (SCAN* inscan, int id)
{
    SCAN_NODE* curr = inscan->head;
    while (curr)
    {
        if (curr->next->id == id) {
            SCAN_NODE* temp = curr->next->next;
            free(curr->next);
            curr->next = temp;
            inscan->length--;
            break;
        }
        curr = curr->next;
    }
}

void scan_destory (SCAN* inscan)
{
    SCAN_NODE* curr = inscan->head;
    SCAN_NODE* del = NULL;

    while (curr)
    {
        del = curr;
        curr = curr->next;
        free(del);
        del = NULL;
    }
    free (inscan);
}