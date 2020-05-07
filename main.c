#include <stdio.h>
#include "ip_lib.h"

void ip_mat_show_stats(ip_mat *t)
{
    unsigned int k;

    compute_stats(t);

    for (k = 0; k < t->k; k++)
    {
        printf("Channel %d:\n", k);
        printf("\t Min: %f\n", t->stat[k].min);
        printf("\t Max: %f\n", t->stat[k].max);
        printf("\t Mean: %f\n", t->stat[k].mean);
    }
}

float get_val(ip_mat *a, unsigned int i, unsigned int j, unsigned int k)
{
    if (i < a->h && j < a->w && k < a->k)
    { /* j>=0 and k>=0 and i>=0 is non sense*/
        return a->data[i][j][k];
    }
    else
    {
        printf("Errore get_val!!!");
        exit(1);
    }
}

void set_val(ip_mat *a, unsigned int i, unsigned int j, unsigned int k, float v)
{
    if (i < a->h && j < a->w && k < a->k)
    {
        a->data[i][j][k] = v;
    }
    else
    {
        printf("Errore set_val!!!");
        exit(1);
    }
}

ip_mat *ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v)
{
    int i, j, z, p;

    ip_mat *nuova = (ip_mat *)malloc(sizeof(ip_mat));

    nuova->w = w;
    nuova->h = h;
    nuova->k = k;

    /*allocamento stat*/
    stats *stat = (stats *)malloc(sizeof(stats) * k);
    nuova->stat = stat;

    /*
    *stat+0 = struct stat
    *stat+1 = struct stat
    *stat+2 = stract stat

    stat[0].min
    */

    /* float ***A = (float ***)malloc(k * sizeof(float **));

    if (A == NULL)
    {
        fprintf(stderr, "Out of memory");
        exit(0);
    }

    for (int i = 0; i < k; i++)
    {
        A[i] = (float **)malloc(h * sizeof(float *));
        if (A[i] == NULL)
        {
            fprintf(stderr, "Out of memory");
            exit(0);
        }

        for (int j = 0; j < h; j++)
        {
            A[i][j] = (float *)malloc(w * sizeof(float));
            if (A[i][j] == NULL)
            {
                fprintf(stderr, "Out of memory");
                exit(0);
            }
        }
    }

    for (i = 0; i < k; i++)
    {
        printf("canale %d\n", i);
        for (j = 0; j < h; j++)
        {
            for (z = 0; z < w; z++)
            {
                A[i][j][z] = v;
                printf("%f ", A[i][j][z]);
            }
            printf("\n");
        }
        printf("\n");
    }
 */

    /*qualcosa*/
    float ***data = (float ***)malloc(sizeof(float **) * k);
    /* allocamento canali */
    for (z = 0; z < k; z++)
    {
        printf("canale %d\n", z);
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
        printf("\n");
    }

    nuova->data = data;

    return nuova;
}

/* void ip_mat_free(ip_mat *a)
{
    int i, j;

    for (i = 0; i < a->k; i++)
    {
        for (j = 0; j < a->h; j++)
        {
            free(a->data[i][j]);
        }
    }
    free(a);
    a = 0;

    return 0;
} */

float compute_min_data(ip_mat *t, int h, int w, int k)
{
    int i, j;

    float min = get_val(t, 0, 0, k);
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            float temp = get_val(t, i, j, k);
            if (min > temp)
            {
                min = temp;
            }
        }
    }
    return min;
}

float compute_max_data(ip_mat *t, int h, int w, int k)
{
    int i, j;

    float max = get_val(t, 0, 0, k);
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            float temp = get_val(t, i, j, k);
            if (max < temp)
            {
                max = temp;
            }
        }
    }
    return max;
}

float compute_mean_data(ip_mat *t, int h, int w, int k)
{
    int i, j;

    float acc = 0;
    int counter = 0;

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            acc += get_val(t, i, j, k);
            counter++;
        }
    }
    return acc / counter;
}

/* Calcola il valore minimo, il massimo e la media per ogni canale
 * e li salva dentro la struttura ip_mat stats
 * */
void compute_stats(ip_mat *t)
{
    int i;
    /*     for (i = 0; i < t->k; i++)
    {
        t->stat[i].min = compute_min_data(t, t->h, t->w, i);
        t->stat[i].max = compute_max_data(t, t->h, t->w, i);
        t->stat[i].mean = compute_mean_data(t, t->h, t->w, i);
    } */

    t->stat[0].min = compute_min_data(t, t->h, t->w, 0);
    t->stat[0].max = compute_max_data(t, t->h, t->w, 0);
    t->stat[0].mean = compute_mean_data(t, t->h, t->w, 0);
    /* 
    t->stat[1].min = compute_min_data(t, t->h, t->w, 1);
    t->stat[1].max = compute_max_data(t, t->h, t->w, 1);
    t->stat[1].mean = compute_mean_data(t, t->h, t->w, 1); */

    t->stat[2].min = compute_min_data(t, t->h, t->w, 2);
    t->stat[2].max = compute_max_data(t, t->h, t->w, 2);
    t->stat[2].mean = compute_mean_data(t, t->h, t->w, 2);
}

int main()
{
    int i;

    ip_mat *nuova = ip_mat_create(10, 10, 3, 21);

    /*    for (i = 0; i < 10; i++)
    {
        set_val(nuova, i, i, 0, i);
    } */

    printf("canale 0 r1 c1 %f\n", get_val(nuova, 0, 0, 0));
    printf("canale 1 r1 c1 %f\n", get_val(nuova, 0, 0, 1));
    printf("canale 2 r1 c1 %f\n", get_val(nuova, 0, 0, 2));
    compute_stats(nuova);
    ip_mat_show_stats(nuova);

    return 0;
}