#include <stdio.h>
#include "ip_lib.h"

ip_mat *ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v)
{
    int i, j, z, p;

    ip_mat *nuova = (ip_mat *)malloc(sizeof(ip_mat));

    nuova->w = w;
    nuova->h = h;
    nuova->k = k;

    /*tutto rotto non si capisce*/
    stats *stat = (stats *)malloc(sizeof(stats) * k);

    for (p = 0; p < k; p++)
    {
        stat->min = v;
        stat->max = v;
        stat->mean = v;
        printf("min =%f, max =%f, mean=%f\n", stat->min, stat->max, stat->mean);
    }

    //nuova->stat = stat;

    /*qualcosa*/
    float ***data = (float ***)malloc(sizeof(float **) * k);
    /* allocamento canali */
    for (z = 0; z < k; z++)
    {
        /*righe*/
        for (i = 0; i < h; i++)
        {
            data[i] = (float **)malloc(sizeof(float *) * h);
            /* allocamento colonne */
            for (j = 0; j < w; j++)
            {

                data[i][j] = (float *)malloc(sizeof(float) * w);

                data[i][j][z] = v;
                printf("%f ", data[i][j][z]);
            }
            printf("\n");
        }
        printf("nuova canala\n");
    }

    nuova->data = data;

    return nuova;
}

int main()
{

    ip_mat_create(3, 4, 2, 22);

    return 0;
}