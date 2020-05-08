/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

void ip_mat_show(ip_mat *t)
{
    unsigned int i, l, j;
    printf("Matrix of size %d x %d x %d (hxwxk)\n", t->h, t->w, t->k);
    for (l = 0; l < t->k; l++)
    {
        printf("Slice %d\n", l);
        for (i = 0; i < t->h; i++)
        {
            for (j = 0; j < t->w; j++)
            {
                printf("%f ", get_val(t, i, j, l));
            }
            printf("\n");
        }
        printf("\n");
    }
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

ip_mat *bitmap_to_ip_mat(Bitmap *img)
{
    unsigned int i = 0, j = 0;

    unsigned char R, G, B;

    unsigned int h = img->h;
    unsigned int w = img->w;

    ip_mat *out = ip_mat_create(h, w, 3, 0);

    for (i = 0; i < h; i++) /* rows */
    {
        for (j = 0; j < w; j++) /* columns */
        {
            bm_get_pixel(img, j, i, &R, &G, &B);
            set_val(out, i, j, 0, (float)R);
            set_val(out, i, j, 1, (float)G);
            set_val(out, i, j, 2, (float)B);
        }
    }

    return out;
}

Bitmap *ip_mat_to_bitmap(ip_mat *t)
{

    Bitmap *b = bm_create(t->w, t->h);

    unsigned int i, j;
    for (i = 0; i < t->h; i++) /* rows */
    {
        for (j = 0; j < t->w; j++) /* columns */
        {
            bm_set_pixel(b, j, i, (unsigned char)get_val(t, i, j, 0),
                         (unsigned char)get_val(t, i, j, 1),
                         (unsigned char)get_val(t, i, j, 2));
        }
    }
    return b;
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

float get_normal_random()
{
    float y1 = ((float)(rand()) + 1.) / ((float)(RAND_MAX) + 1.);
    float y2 = ((float)(rand()) + 1.) / ((float)(RAND_MAX) + 1.);
    return cos(2 * PI * y2) * sqrt(-2. * log(y1));
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

    /*qualcosa*/
    float*** A = (float***)malloc(h * sizeof(float**));

	if (A == NULL) {
		fprintf(stderr, "Out of memory");
		exit(1);
	}

	for (int i = 0; i < h; i++)
	{
		A[i] = (float**)malloc(w * sizeof(float*));
		if (A[i] == NULL) {
			fprintf(stderr, "Out of memory");
			exit(1);
		}

		for (int j = 0; j < w; j++)
		{
			A[i][j] = (float*)malloc(k * sizeof(float));
				if (A[i][j] == NULL) {
				fprintf(stderr, "Out of memory");
				exit(1);
			}
		}
	}
	
	/* assign values to allocated memory */
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			for (int z = 0; z < k; z++)
				A[i][j][z] = v;
                /* MADONNA. */
    

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
    int i;

    for(i = 0; i < t->k; i++){
    
    t->stat[i].min = compute_min_data(t, t->h, t->w,i);
    t->stat[i].max = compute_max_data(t, t->h, t->w, i);
    t->stat[i].mean = compute_mean_data(t, t->h, t->w, i);
 
    }
}

ip_mat *ip_mat_sum(ip_mat *a, ip_mat *b){  
    
    int i,j,z;
    
    /*creo una matrice di dimensione uguale ad A e la setto tutta 0*/
    ip_mat *sum=ip_mat_create(a->h, a->w, a->k, 00.0);
    
    for(z=0;z<sum->k;z++){
        for (i = 0; i < sum->h; i++)
        {
            for (j = 0; j < sum->w; j++)
            {
                /*setto in posizione (scorri for) la somma della medesima posizione di *A e *B*/
                set_val(sum,i,j,z,(get_val(a, i, j, z)+get_val(b, i, j, z)));
            }
        }
    }
    return sum;
}

ip_mat *ip_mat_sub(ip_mat *a, ip_mat *b){    
    
    int i,j,z;
    
    /*creo una matrice di dimensione uguale ad A e la setto tutta 0*/
    ip_mat *sub=ip_mat_create(a->h, a->w, a->k, 00.0);
    
    for(z=0;z<sub->k;z++){
        for (i = 0; i < sub->h; i++)
        {
            for (j = 0; j < sub->w; j++)
            {
                /*setto in posizione (scorri for) la sottrazione della medesima posizione di *A e *B*/
                set_val(sub,i,j,z,(get_val(a, i, j, z)-get_val(b, i, j, z)));
            }
        }
    }
    return sub;
}




ip_mat *ip_mat_copy(ip_mat *in);                                                                                              /*manuel*/
ip_mat *ip_mat_subset(ip_mat *t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end); /*francesco*/
ip_mat *ip_mat_concat(ip_mat *a, ip_mat *b, int dimensione);                                                                  /*francesco*/


ip_mat *ip_mat_mul_scalar(ip_mat *a, float c);                                                                                /*riccardo*/
ip_mat *ip_mat_add_scalar(ip_mat *a, float c);                                                                                /*riccardo*/
ip_mat *ip_mat_mean(ip_mat *a, ip_mat *b);                                                                                    /*manuel*/
