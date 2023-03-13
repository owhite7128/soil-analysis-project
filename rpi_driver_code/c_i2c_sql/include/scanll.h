#ifndef SCANLL_H
#define SCANLL_H

typedef struct scan_node {
    int id;
    float ph;
    struct scan_node* next;    
} SCAN_NODE;

typedef struct scan {
    SCAN_NODE* head;
    SCAN_NODE* tail;
    int length;
    int curr_id;
} SCAN;


SCAN* scan_create ();

void scan_add (SCAN* inscan, float inph);

void scan_remove_id (SCAN* inscan, int id);

void scan_destory (SCAN* inscan);

#endif