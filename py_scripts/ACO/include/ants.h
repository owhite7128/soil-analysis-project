#ifndef ANTS_H
#define ANTS_H

// Web Node, defined with a unique ID, and the coords at which it is.
typedef struct node
{
    int id;
    int coords[2];
} NODE;

// Web Edge, defined with a unique ID, the ids of the two nodes it connects, the distance between them, the desiribility, pointers to both nodes, and the pheromones that edge has
typedef struct edge
{
    int id;
    int nodes[2];
    float dist;
    float desire;
    NODE* node_1;
    NODE* node_2;
    float phero;
} EDGE;

// The Web, Contains
typedef struct web
{
    int id;
    NODE* node_list;
    int node_length;
    EDGE* edge_list;
    int edge_length;
    int* best_path;
    float best_path_dist;
} WEB;

WEB instatiate_web (int coords_len, int coords[][2]);

void out_best_path (WEB web);

void destroy_web (WEB web);

typedef struct ant {
    int id;
    int at;
    NODE* path;
    int path_length;
    float dist_travel;
} ANT;

void ant_walk (ANT* ant, WEB web);

void walk (ANT* ants, int ant_num, WEB *web_use);

ANT* new_ants (WEB web, int ant_num);

void destroy_ants (ANT* ants, int ant_num);

#endif