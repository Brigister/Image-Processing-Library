#include <stdio.h>
#include "ip_lib.h"

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


int main()
{
    int i;
    ip_mat *sum;
    ip_mat *sub;

    printf("NUOVA \n");
    ip_mat *nuova = ip_mat_create(4, 2, 3, 99.9);
    ip_mat_show(nuova);
    ip_mat *prova = ip_mat_create(1, 1, 3, 22);
    ip_mat_show(prova);
    printf("SECONDA MAT-------------------------------\n");
    ip_mat *secondamat = ip_mat_create(4, 2, 3, 59.9);
    ip_mat_show(secondamat);
    printf("--------------------------------------\n");
    
    /* compute_stats(nuova);
    ip_mat_show_stats(nuova); */
    
    printf("SOMMA--------------------------------\n");

    sum=ip_mat_sum(nuova,secondamat);
    ip_mat_show(sum);

    printf("STATISTICHE DI SUM------------------------\n");

    compute_stats(sum);
    ip_mat_show_stats(sum);

    printf("sottrazione--------------------------------\n");

    sub=ip_mat_sub(nuova,secondamat);
    ip_mat_show(sub);

    printf("STATISTICHE DI SUM------------------------\n");

    compute_stats(sub);
    ip_mat_show_stats(sub);
    

    return 0;
}
