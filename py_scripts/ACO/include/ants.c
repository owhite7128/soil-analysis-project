#include "ants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Global Modifiers
#define ALPHA 0.5
#define BETA 1
#define PHERO_EVAP 0.5
#define Q_CONST 1

// Returns a pointer to a dynamically allocated array of ANT objects
ANT* new_ants (WEB web_use, int ant_num)
{
    ANT* ants = NULL;
    ants = (ANT*) malloc (ant_num*sizeof(ANT));
    for (int i=0; i<ant_num; i++) {
        ants[i].path_length = 0;
        ants[i].at = 0;
        ants[i].dist_travel = 0.0;
        ants[i].id = i;
        ants[i].path = NULL;
    }
    return ants;
}

// Returns a list of node ids that are remanaing for the ant to travel to, The first value returned is always the length, exclusive, of the list.
int* nodes_left (NODE* web_nodes, int web_len, NODE* ant_nodes, int ant_len) 
{
    int ret_len = 1;
    int* ret;
    int* temp;
    ret = (int*) malloc(ret_len*sizeof(int));
    ret[0] = 0;
    int in = 0;
    for (int i=0; i<web_len; i++)
    {
        in = 0;
        for (int j=0; j<ant_len; j++)
        {
            if (web_nodes[i].id == ant_nodes[j].id)
            {
                in = 1;
            }
        }
        if (in == 0)
        {
            ret_len++;
            temp = (int*) malloc(ret_len*sizeof(int));
            for(int j=1; j<ret_len-1; j++)
            {
                temp[j] = ret[j];
            }
            free(ret);
            ret = temp;
            ret[0] = ret_len-1;
            ret[ret_len-1] = web_nodes[i].id;
        }
    }
    return ret;
}

int node_is_in (int* node_list, int id)
{
    int in = 0;
    for (int i=1; i<node_list[0]; i++)
    {
        if (id == node_list[i])
        {
            in = 1;
        }
    }
    return in;
}

EDGE* get_edge (WEB web_use, int start, int end)
{
    EDGE* edge_ret = NULL;
    for (int i=0; i<web_use.edge_length; i++){
        if(web_use.edge_list[i].nodes[0] == start && web_use.edge_list[i].nodes[1] == end)
        {
            edge_ret = &web_use.edge_list[i];
            break;
        }
    }
    return edge_ret;
}

int select_node (int* node_list, int ant_at, WEB web_use)
{
    float sum = 0.0;
    int edge_idx = (web_use.node_length - 1) * ant_at;
    float* probs = (float*) malloc (node_list[0]*sizeof(float));

    int selected = -1;
    
    EDGE* curr_edge = NULL;


    float roll_sum = 0.0;
    float total = 0.0;

    for(int i=0; i<web_use.node_length-1; i++)
    {
        if (node_is_in(node_list, i) && !(i == ant_at))
        {
            sum += (pow(web_use.edge_list[edge_idx+i].desire, BETA) * pow(web_use.edge_list[edge_idx+i].phero, ALPHA));
        }
    }

    for (int i=0; i<node_list[0]; i++) {
        curr_edge = get_edge (web_use, ant_at, node_list[i+1]);

        probs[i] = (pow(curr_edge->desire, BETA) * pow(curr_edge->phero, ALPHA))/(sum);
        total += probs[i];

    }

    // Why is total not 1? Unsure... look into later.
    float choice = ((float)rand()/(float)(RAND_MAX)) * total;

    for (int i=0; i<node_list[0]; i++)
    {
        if ((choice-roll_sum) <= probs[i])
        {
            selected = node_list[i+1];
            break;
        }
        roll_sum += probs[i];
    }   
    return selected;
}

void update_best_path (ANT* ants, int ant_num, WEB *web_use)
{
    float best_ant_dist = web_use->best_path_dist;
    if (best_ant_dist < 0.0)
    {
        best_ant_dist = __FLT_MAX__;
    }
    int best_ant_id = -1;
    for (int i=0; i<ant_num; i++)
    {
        if (ants[i].dist_travel < best_ant_dist)
        {
            best_ant_dist = ants[i].dist_travel;
            best_ant_id = ants[i].id;
        }
    }
    if (web_use->best_path_dist != best_ant_dist) {
        for (int i=0; i<web_use->node_length-1; i++)
        {
            web_use->best_path[i] = ants[best_ant_id].path[i].id;
        }
    
        web_use->best_path_dist = best_ant_dist;
        web_use->best_path[web_use->node_length-1] = ants[best_ant_id].at;
    }
}

int edge_in_path (ANT ant_t, int start, int end)
{
    for (int i=0; i<ant_t.path_length-1; i++)
    {
        if (ant_t.path[i].id == start && ant_t.path[i+1].id == end)
        {
            return 1;
        }
    }
    return 0;
}

void update_phero (ANT* ants, int ant_num, WEB *web_use)
{
    // new_phero = (1-PHERO_EVAP)*old_phero + sum(deltaP)
    // deltaP = Q_const/ant->dist or 0 (if path not taken)
    float ant_sum = 0.0;

    for (int i=0; i<web_use->edge_length; i++)
    {
        ant_sum = 0.0;
        for (int j=0; j<ant_num; j++)
        {
            if(edge_in_path (ants[j], web_use->edge_list[i].nodes[0], web_use->edge_list[i].nodes[1]))
            {
                ant_sum += ((Q_CONST/ants[j].dist_travel));
            }
        }
        web_use->edge_list[i].phero = (1-PHERO_EVAP)*web_use->edge_list[i].phero + ant_sum;
    }
    return;
}

void walk (ANT* ants, int ant_num, WEB *web_use)
{
    for (int i=0; i<ant_num; i++)
    {
        ant_walk(&ants[i], *web_use);
    }
    update_phero (ants, ant_num, web_use);
    update_best_path (ants, ant_num, web_use);
}

// Has one ant "walk" around the web
void ant_walk (ANT* ant, WEB web_use)
{
    for (int i=0; i<web_use.node_length-1; i++)
    {
        // Add Node departed to ant path
        ant->path_length++;
        NODE* temp = (NODE*) malloc (ant->path_length*sizeof(NODE));
        for(int j=0; j<ant->path_length-1; j++)
        {
            temp[j].id = ant->path[j].id;
            temp[j].coords[0] = ant->path[j].coords[0];
            temp[j].coords[1] = ant->path[j].coords[1];
        }
        free (ant->path);
        ant->path = temp;

        ant->path[ant->path_length-1].id = ant->at;
        ant->path[ant->path_length-1].coords[0] = web_use.node_list[ant->at].coords[0];
        ant->path[ant->path_length-1].coords[1] = web_use.node_list[ant->at].coords[1];

        // Decide which path to follow
        int choose_node = -1;
        int* ids = nodes_left(web_use.node_list, web_use.node_length, ant->path, ant->path_length);
        choose_node = select_node (ids, ant->at, web_use);


        // Add Edge distance to ants dist_traveled
        int edge_idx = -1;
        if (ant->at > choose_node)
        {
            edge_idx = (web_use.node_length - 1)*ant->at + choose_node;
        }else {
            edge_idx = (web_use.node_length - 1)*ant->at + choose_node-1;
        }
        ant->dist_travel += web_use.edge_list[edge_idx].dist;

        // Set ant->at to choose_node
        ant->at = choose_node;
    }
    return;
}

int sum_down (int start)
{
    int ret = 0;
    while (start > 0)
    {
        ret += start;
        start--;
    }
    return ret;
}

float dist (int coord1[2], int coord2[2])
{
    return sqrt(pow((float)(coord2[0]-coord1[0]),2)+pow((float)(coord2[1]-coord1[1]),2));
}

WEB instatiate_web (int coords_len, int coords[][2])
{
    WEB new_web;
    new_web.id = 0;
    new_web.node_list = (NODE*) malloc(coords_len*sizeof(NODE));
    new_web.node_length = coords_len;
    for (int i=0; i<coords_len; i++) {
        new_web.node_list[i].id = i;
        new_web.node_list[i].coords[0] = coords[i][0];
        new_web.node_list[i].coords[1] = coords[i][1];
    }
    new_web.best_path = (int*) malloc(coords_len*sizeof(int));
    new_web.best_path_dist = -1.0;

    int edge_len = coords_len * (coords_len - 1);
    new_web.edge_list = (EDGE*) malloc(edge_len*sizeof(EDGE));
    new_web.edge_length = edge_len;
    int g = 0;
    for (int i=0; i<coords_len; i++)
    {
        for (int j=0; j<coords_len; j++)
        {
            if (i != j) {
                new_web.edge_list[g].id = g;
                new_web.edge_list[g].nodes[0] = i;
                new_web.edge_list[g].nodes[1] = j;
                new_web.edge_list[g].dist = dist(new_web.node_list[i].coords, new_web.node_list[j].coords);
                new_web.edge_list[g].desire = (1.0)/(new_web.edge_list[g].dist);
                new_web.edge_list[g].node_1 = &(new_web.node_list[i]);
                new_web.edge_list[g].node_2 = &(new_web.node_list[j]);
                new_web.edge_list[g].phero = 5.0;
                g++;
            }
        }
    }


    return new_web;
}

void out_best_path (WEB web_use)
{
    for (int i=0; i<web_use.node_length-1; i++)
    {
        printf ("(%d, %d) -> ", web_use.node_list[web_use.best_path[i]].coords[0], web_use.node_list[web_use.best_path[i]].coords[1]);
    }
    printf ("(%d, %d)\n", web_use.node_list[web_use.best_path[web_use.node_length-1]].coords[0], web_use.node_list[web_use.best_path[web_use.node_length-1]].coords[1]);
    printf ("Distance of Best Pathway: %f\n", web_use.best_path_dist);
}

void destroy_web (WEB web)
{
    return;
}

void destroy_ants (ANT* ants, int ant_num)
{
    for (int i=0; i<ant_num; i++)
    {
        free(ants[i].path);
    }
    free (ants);
    ants = NULL;
    return;
}