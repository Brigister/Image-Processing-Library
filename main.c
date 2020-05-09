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

    /*allocamento struttura array*/
    float ***data = (float ***)malloc(h * sizeof(float **));

    if (data == NULL)
    {
        printf("Out of memory");
        exit(1);
    }

    for (int i = 0; i < h; i++)
    {
        data[i] = (float **)malloc(w * sizeof(float *));
        if (data[i] == NULL)
        {
            printf("Out of memory");
            exit(1);
        }

        for (int j = 0; j < w; j++)
        {
            data[i][j] = (float *)malloc(k * sizeof(float));
            if (data[i][j] == NULL)
            {
                printf("Out of memory");
                exit(1);
            }
        }
    }

    /* assign values to allocated memory */
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            for (int z = 0; z < k; z++)
                data[i][j][z] = v;
    /* MADONNA. */

    nuova->data = data;

    return nuova;
}

void ip_mat_free(ip_mat *a)
{
   int i,j;

    for(i=0;i<a->h;i++)
    {
        for(int j=0;j<a->w;j++)
        {
                free(a->data[i][j]);
        }
        free(a->data[i]);
    }
    free(a->data);
   free(a->stat);
   free(a);
    
   
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

    for (i = 0; i < t->k; i++)
    {

        t->stat[i].min = compute_min_data(t, t->h, t->w, i);
        t->stat[i].max = compute_max_data(t, t->h, t->w, i);
        t->stat[i].mean = compute_mean_data(t, t->h, t->w, i);
    }
}

ip_mat *ip_mat_sum(ip_mat *a, ip_mat *b)
{

    int i, j, z;
    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("Matrici diverse\n");
        exit(1);
    }
    else
    {
        /*creo una matrice di dimensione uguale ad A e la setto tutta 0*/
        ip_mat *sum = ip_mat_create(a->h, a->w, a->k, 00.0);

        for (z = 0; z < sum->k; z++)
        {
            for (i = 0; i < sum->h; i++)
            {
                for (j = 0; j < sum->w; j++)
                {
                    /*setto in posizione (scorri for) la somma della medesima posizione di *A e *B*/
                    set_val(sum, i, j, z, (get_val(a, i, j, z) + get_val(b, i, j, z)));
                }
            }
        }
        compute_stats(sum);
        return sum;
    }
}

ip_mat *ip_mat_sub(ip_mat *a, ip_mat *b)
{

    int i, j, z;
    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("Matrici diverse\n");
        exit(1);
    }
    else
    {
        /*creo una matrice di dimensione uguale ad A e la setto tutta 0*/
        ip_mat *sub = ip_mat_create(a->h, a->w, a->k, 00.0);

        for (z = 0; z < sub->k; z++)
        {
            for (i = 0; i < sub->h; i++)
            {
                for (j = 0; j < sub->w; j++)
                {
                    /*setto in posizione (scorri for) la sottrazione della medesima posizione di *A e *B*/
                    set_val(sub, i, j, z, (get_val(a, i, j, z) - get_val(b, i, j, z)));
                }
            }
        }
        compute_stats(sub);
        return sub;
    }
}

/* Restituisce una sotto-matrice, ovvero la porzione individuata da:
 * t->data[row_start...row_end][col_start...col_end][0...k]
 * La terza dimensione la riportiamo per intero, stiamo in sostanza prendendo un sottoinsieme
 * delle righe e delle colonne.
 * */
ip_mat *ip_mat_subset(ip_mat *t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{

    int i, j, z;

    if (row_start >= 0 && row_start <= row_end && row_end <= t->h && col_start >= 0 && col_start <= col_end && col_end < t->w)
    {
        ip_mat *sub_mat = ip_mat_create(row_end, col_end, t->k, 0);
        /*NON SONO SICURO DEL -1 https://moodle.unive.it/mod/forum/discuss.php?d=63261 */
        for (z = 0; z < sub_mat->k; z++)
        {
            for (i = 0; i < sub_mat->h; i++)
            {
                for (j = 0; j < sub_mat->w; j++)
                {
                    set_val(sub_mat, i, j, z, get_val(t, i, j, z));
                }
            }
        }
        return sub_mat;
    }
    else
    {
        printf("Valori non accettabili!\n");
        exit(1);
    }
}

ip_mat *ip_mat_concat(ip_mat *a, ip_mat *b, int dimensione)
{

    int i, j, z;
    ip_mat *concatenata;

    if (dimensione == 0)
    {
        if (a->w == b->w && a->k == b->k)
        {
            concatenata = ip_mat_create(a->h + b->h, a->w, a->k, 0);

            for (z = 0; z < concatenata->k; z++)
            {
                for (i = 0; i < a->h; i++)
                {
                    for (j = 0; j < concatenata->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(a, i, j, z));
                    }
                }
                for (i = (a->h); i < concatenata->h; i++)
                {
                    for (j = 0; j < concatenata->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(b, i - (a->h), j, z));
                    }
                }
            }
        }
        else
        {
            printf("Non è possibile fare la concatenazione\n");
            exit(1);
        }
    }
    else if (dimensione == 1)
    {
        if (a->h == b->h && a->k == b->k)
        {
            concatenata = ip_mat_create(a->h, a->w + b->w, a->k, 0);

            for (z = 0; z < concatenata->k; z++)
            {
                for (i = 0; i < a->h; i++)
                {
                    for (j = 0; j < a->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(a, i, j, z));
                    }

                    for (j = a->w; j < concatenata->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(b, i, j - (a->w), z));
                    }
                }
            }
        }

        else
        {
            printf("Non è possibile fare la concatenazione\n");
            exit(1);
        }
    }
    else if (dimensione == 2)
    {
        if (a->h == b->h && a->w == b->w)
        {
            concatenata = ip_mat_create(a->h, a->w, a->k + b->k, 0);

            for (z = 0; z < a->k; z++)
            {
                for (i = 0; i < a->h; i++)
                {
                    for (j = 0; j < concatenata->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(a, i, j, z));
                    }
                }
            }
            for (z = a->k; z < concatenata->k; z++)
            {
                for (i = 0; i < a->h; i++)
                {
                    for (j = 0; j < concatenata->w; j++)
                    {
                        set_val(concatenata, i, j, z, get_val(b, i, j, z - (a->k)));
                    }
                }
            }
        }
        else
        {
            printf("Non è possibile fare la concatenazione\n");
            exit(1);
        }
    }
    else
    {
        printf("Dimensione Errata\n");
        exit(1);
    }

    return concatenata;
}
ip_mat *ip_mat_mul_scalar(ip_mat *a, float c){
      int i,j,z;
      float supp;
      int h=a->h;
      int w=a->w;
      int k=a->k;
      ip_mat *nuova_ms = ip_mat_create(h, w, k, 9.9);
      for(i=0;i<h;i++){
         for(j=0;j<w;j++){
            for(z=0;z<k;z++){
               supp=get_val(a,i,j,z);
               set_val(nuova_ms,i,j,z,supp*c);
            }
         }
      }
      
      return nuova_ms;   
      
}

ip_mat *ip_mat_add_scalar(ip_mat *a, float c){
      int i,j,z;
      float supp;
      int h=a->h;
      int w=a->w;
      int k=a->k;
      ip_mat *nuova_as = ip_mat_create(h, w, k, 9.9);
      for(i=0;i<h;i++){
         for(j=0;j<w;j++){
            for(z=0;z<k;z++){
               supp=get_val(a,i,j,z);
               set_val(nuova_as,i,j,z,supp+c);
            }
         }
      }
      
      return nuova_as;       
}      
    
      

int main()
{
    int i;
    ip_mat *sum;
    ip_mat *sub;
    ip_mat *sub_mat;
    ip_mat *concatenata;
    printf("NUOVA \n");
    ip_mat *nuova = ip_mat_create(3, 2, 3, 22.22);
    ip_mat_show(nuova);
    ip_mat_free(nuova);
   

    /* printf("sottomatrice---------------------------------\n");

    sub_mat = ip_mat_subset(nuova,1, 6, 1, 1);
    ip_mat_show(sub_mat); */

    /* printf("SECONDA MAT-------------------------------\n");
    ip_mat *secondamat = ip_mat_create(3, 2, 5, 59.9);
    ip_mat_show(secondamat); */

    /*  printf("concatenata---------------------------------\n");

    concatenata = ip_mat_concat(nuova, secondamat, 2);
    ip_mat_show(concatenata); */

    /*   
    printf("stat--------------------------------\n");
     compute_stats(nuova);
    ip_mat_show_stats(nuova); */

    /*      printf("SOMMA--------------------------------\n");

    sum=ip_mat_sum(nuova,secondamat);
    ip_mat_show(sum);
 */
    /*
    printf("STATISTICHE DI SUM------------------------\n");

    compute_stats(sum);
    ip_mat_show_stats(sum);

    printf("sottrazione--------------------------------\n");

    sub=ip_mat_sub(nuova,secondamat);
    ip_mat_show(sub);

    printf("STATISTICHE DI SUM------------------------\n");

    compute_stats(sub);
    ip_mat_show_stats(sub); */

    return 0;
}
