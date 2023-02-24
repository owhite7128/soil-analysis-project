#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ants.h"

// Read in the Coords from Arr? Not 100% sure.
// Then initialize ACO with X ants and X generations
// Maybe being tabled... SADGE

int main (int argc, char** argv)
{
    clock_t begin = clock();

    srand (time(NULL));
    int coords_len = 11;
    int coords_arr[11][2] = {{0,0}, {0,1}, {1,0}, {1,1}, {2,1}, {1,2}, {3,4}, {5,2}, {5,6}, {2,3}, {22,12}};
    int ant_num = 100;
    int gen_num = 100;
    WEB new_web = instatiate_web(coords_len, coords_arr);
    ANT* ants;

    for (int i=0; i<gen_num; i++)
    {
        ANT* ants = new_ants(new_web, ant_num);
        walk (ants, ant_num, &new_web);
        destroy_ants (ants, ant_num);
    }

    out_best_path (new_web);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf ("Time Taken: %lf\n", time_spent);

    return 0;
}