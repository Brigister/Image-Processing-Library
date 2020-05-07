#include <stdio.h>
#include "ip_lib.h"

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

    /*qualcosa*/
    float*** A = (float***)malloc(h * sizeof(float**));

	if (A == NULL) {
		fprintf(stderr, "Out of memory");
		exit(0);
	}

	for (int i = 0; i < h; i++)
	{
		A[i] = (float**)malloc(w * sizeof(float*));
		if (A[i] == NULL) {
			fprintf(stderr, "Out of memory");
			exit(0);
		}

		for (int j = 0; j < w; j++)
		{
			A[i][j] = (float*)malloc(k * sizeof(float));
				if (A[i][j] == NULL) {
				fprintf(stderr, "Out of memory");
				exit(0);
			}
		}
	}
	
	// assign values to allocated memory
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			for (int z = 0; z < k; z++)
				A[i][j][z] = 99.9;

	// print the 3D array
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			for (int z = 0; z < k; z++)
				printf("%f ", A[i][j][z]);

			printf("\n");
			}
		printf("\n");
	}

    nuova->data = A;

    return nuova;
}
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

void compute_stats(ip_mat *t)
{
    

    t->stat[0].min = compute_min_data(t, t->h, t->w, 0);
    t->stat[0].max = compute_max_data(t, t->h, t->w, 0);
    t->stat[0].mean = compute_mean_data(t, t->h, t->w, 0);
 
    t->stat[1].min = compute_min_data(t, t->h, t->w, 1);
    t->stat[1].max = compute_max_data(t, t->h, t->w, 1);
    t->stat[1].mean = compute_mean_data(t, t->h, t->w, 1); 

    t->stat[2].min = compute_min_data(t, t->h, t->w, 2);
    t->stat[2].max = compute_max_data(t, t->h, t->w, 2);
    t->stat[2].mean = compute_mean_data(t, t->h, t->w, 2);
}

int main()
{
    int i;

    ip_mat *nuova = ip_mat_create(3, 3, 3, 99.9);
    compute_stats(nuova);
    ip_mat_show_stats(nuova);

    return 0;
}
