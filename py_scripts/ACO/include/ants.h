#ifndef ANTS_H
#define ANTS_H

typedef struct node
{
    int id;
    int coords[2];
} NODE;

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