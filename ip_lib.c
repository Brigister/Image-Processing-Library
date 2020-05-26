/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

#define E 2.71828182846

void is_null(ip_mat *t)
{
    if (t == NULL)
    {
        printf("il puntatore %p punta a NULL\n", (void *)&t);
        exit(1);
    }
}
void ip_mat_show(ip_mat *t)
{
    is_null(t);
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
    is_null(t);
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
    is_null(t);
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
    is_null(a);
    if (i < a->h && j < a->w && k < a->k)
    { /* j>=0 and k>=0 and i>=0 is non sense*/
        return a->data[i][j][k];
    }
    else
    {
        printf("Errore get_val!\n");
        exit(1);
    }
}

void set_val(ip_mat *a, unsigned int i, unsigned int j, unsigned int k, float v)
{
    is_null(a);
    if (i < a->h && j < a->w && k < a->k)
    {
        a->data[i][j][k] = v;
    }
    else
    {
        printf("Errore set_val!\n");
        exit(1);
    }
}

float get_normal_random(float media, float std)
{

    float y1 = ((float)(rand()) + 1.) / ((float)(RAND_MAX) + 1.);
    float y2 = ((float)(rand()) + 1.) / ((float)(RAND_MAX) + 1.);
    float num = cos(2 * PI * y2) * sqrt(-2. * log(y1));

    return media + num * std;
}

ip_mat *ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v)
{
    unsigned int i, j, z;

    ip_mat *nuova;
    stats *stat;
    float ***data;
    nuova = (ip_mat *)malloc(sizeof(ip_mat));

    nuova->w = w;
    nuova->h = h;
    nuova->k = k;

    /*allocamento stat*/

    stat = (stats *)malloc(sizeof(stats) * k);
    nuova->stat = stat;

    /*creazione*/
    data = (float ***)malloc(h * sizeof(float **));

    if (data == NULL)
    {
        printf("Ritenta, sarai più fortunato");
        exit(1);
    }

    for (i = 0; i < h; i++)
    {
        data[i] = (float **)malloc(w * sizeof(float *));
        if (data[i] == NULL)
        {
            printf("Ritenta, sarai più fortunato");
            exit(1);
        }

        for (j = 0; j < w; j++)
        {
            data[i][j] = (float *)malloc(k * sizeof(float));
            if (data[i][j] == NULL)
            {
                printf("Ritenta, sarai più fortunato");
                exit(1);
            }
        }
    }

    /* assign values to allocated memory */
    for (i = 0; i < h; i++)
        for (j = 0; j < w; j++)
            for (z = 0; z < k; z++)
                data[i][j][z] = v;

    nuova->data = data;

    return nuova;
}

/* Libera la memoria (data, stat e la struttura) */
void ip_mat_free(ip_mat *a)
{
    unsigned int i, j;

    is_null(a);
    if (a != NULL)
    {
        free(a->stat);
        /* printf("struttura statistiche liberata\n"); */

        for (i = 0; i < a->h; i++)
        {
            for (j = 0; j < a->w; j++)
                free(a->data[i][j]);

            free(a->data[i]);
        }
        free(a->data);
        free(a);
    }
    else
    {
        printf("Il puntatore %p punta a NULL\n", (void *)&a);
    }
}

float compute_min_data(ip_mat *t, unsigned int h, unsigned int w, unsigned int k)
{
    unsigned int i, j;
    float min;

    is_null(t);
    min = get_val(t, 0, 0, k);
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

float compute_max_data(ip_mat *t, unsigned int h, unsigned int w, unsigned int k)
{
    unsigned int i, j;
    float max;

    is_null(t);

    max = get_val(t, 0, 0, k);
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

float compute_mean_data(ip_mat *t, unsigned int h, unsigned int w, unsigned int k)
{
    unsigned int i, j;

    float acc = 0;
    int counter = 0;

    is_null(t);

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
    unsigned int i;

    is_null(t);

    for (i = 0; i < t->k; i++)
    {

        t->stat[i].min = compute_min_data(t, t->h, t->w, i);
        t->stat[i].max = compute_max_data(t, t->h, t->w, i);
        t->stat[i].mean = compute_mean_data(t, t->h, t->w, i);
    }
}

ip_mat *ip_mat_sum(ip_mat *a, ip_mat *b)
{
    unsigned int i, j, z;

    is_null(a);
    is_null(b);

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
    unsigned int i, j, z;

    is_null(a);
    is_null(b);
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

ip_mat *ip_mat_subset(ip_mat *t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{
    unsigned int i, j, z;

    is_null(t);

    if (/* row_start >= 0 && */ row_start <= row_end && row_end <= t->h && /* col_start >= 0  &&*/ col_start <= col_end && col_end <= t->w)
    {
        unsigned int valorant_alpha = row_end - row_start;
        unsigned int valorant_beta = col_end - col_start;
        ip_mat *sub_mat = ip_mat_create(valorant_alpha, valorant_beta, t->k, 0);
        for (z = 0; z < sub_mat->k; z++)
        {
            for (i = 0; i < sub_mat->h; i++)
            {
                for (j = 0; j < sub_mat->w; j++)
                {
                    set_val(sub_mat, i, j, z, get_val(t, i + row_start, j + col_start, z));
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

    unsigned int i, j, z;
    ip_mat *concatenata;

    is_null(a);
    is_null(b);

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

ip_mat *ip_mat_copy(ip_mat *in)
{

    /*  variabili di scorrimento per i cicli */

    unsigned int i, j, z;
    ip_mat *copy;

    is_null(in);

    /* creo una nuova matrice di dimensioni uguali a quella data e la inizializzo a 0 */
    copy = ip_mat_create(in->h, in->w, in->k, 00.0);

    /* setto i valori della matrice copy nella posizione data dal ciclo for  uguali a quelli della matrice in ingresso */

    for (z = 0; z < copy->k; z++)
    {
        for (i = 0; i < copy->h; i++)
        {
            for (j = 0; j < copy->w; j++)
            {
                set_val(copy, i, j, z, (get_val(in, i, j, z)));
            }
        }
    }
    return copy;
}

ip_mat *ip_mat_mean(ip_mat *a, ip_mat *b)
{

    /* variabili di scorrimento */
    unsigned int i, j, z;

    is_null(a);
    is_null(b);

    /* verifico che le matrici date siano della stessa dimensione, altrimenti ritorno un printf di errore  */
    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("Matrici con dimensioni diverse\n");
        exit(1);
    }
    else
    {
        /* creo una nuova matrice di dimensione uguale a quelle date e la inizializzo a 0  */

        ip_mat *media = ip_mat_create(a->h, a->w, a->k, 00.0);

        /* setto i valori della matrice media nella posizione data dal ciclo for con i valori dati  dalle media di a e b */

        for (z = 0; z < media->k; z++)
        {
            for (i = 0; i < media->h; i++)
            {
                for (j = 0; j < media->w; j++)
                {

                    set_val(media, i, j, z, ((get_val(a, i, j, z) + get_val(b, i, j, z)) / 2));
                }
            }
        }
        /* ritorno la matrice  media in output */
        return media;
    }
}

ip_mat *ip_mat_mul_scalar(ip_mat *a, float c)
{
    unsigned int i, j, z;
    float supp;
    ip_mat *nuova_ms;
    unsigned int h = a->h;
    unsigned int w = a->w;
    unsigned int k = a->k;

    is_null(a);

    nuova_ms = ip_mat_create(h, w, k, 9.9);
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            for (z = 0; z < k; z++)
            {
                supp = get_val(a, i, j, z);
                set_val(nuova_ms, i, j, z, supp * c);
            }
        }
    }

    return nuova_ms;
}

ip_mat *ip_mat_add_scalar(ip_mat *a, float c)
{
    unsigned int i, j, z;
    float supp;
    ip_mat *nuova_as;
    unsigned int h = a->h;
    unsigned int w = a->w;
    unsigned int k = a->k;

    is_null(a);

    nuova_as = ip_mat_create(h, w, k, 9.9);
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            for (z = 0; z < k; z++)
            {
                supp = get_val(a, i, j, z);
                set_val(nuova_as, i, j, z, supp + c);
            }
        }
    }

    return nuova_as;
}

void ip_mat_init_random(ip_mat *t, float mean, float var)
{
    unsigned int i, j, z;

    is_null(t);

    for (i = 0; i < t->h; i++)
        for (j = 0; j < t->w; j++)
            for (z = 0; z < t->k; z++)
            {

                float rand = get_normal_random(mean, var);
                /* rand = rand * var + mean; */
                t->data[i][j][z] = rand;
            }
}

ip_mat *ip_mat_blend(ip_mat *a, ip_mat *b, float alpha)
{

    is_null(a);
    is_null(b);

    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("Le due ip_mat hanno dimensione diversa, impossibile fare il blend\n");
        exit(1);
    }
    else if (alpha < 0.0 || alpha > 1.0)
    {
        printf("Il valore alpha non rientra nel range [0,1]\n");
        exit(1);
    }
    else
    {
        unsigned int i, j, z;

        ip_mat *blend = ip_mat_create(a->h, a->w, a->k, 0);

        for (i = 0; i < a->h; i++)
            for (j = 0; j < a->w; j++)
                for (z = 0; z < a->k; z++)
                {
                    /*bait*/
                    float ercole = alpha * get_val(a, i, j, z);
                    float filottete = (1 - alpha) * get_val(b, i, j, z);
                    set_val(blend, i, j, z, ercole + filottete);
                }
        return blend;
    }
}

ip_mat *ip_mat_to_gray_scale(ip_mat *in)
{
    ip_mat *result;

    is_null(in);

    result = ip_mat_create(in->h, in->w, in->k, 0);

    unsigned int i, j;

    for (i = 0; i < in->h; i++)
    {
        for (j = 0; j < in->w; j++)
        {
            result->data[i][j][0] = (get_val(in, i, j, 0) + get_val(in, i, j, 1) + get_val(in, i, j, 2)) / 3;
            result->data[i][j][1] = (get_val(in, i, j, 0) + get_val(in, i, j, 1) + get_val(in, i, j, 2)) / 3;
            result->data[i][j][2] = (get_val(in, i, j, 0) + get_val(in, i, j, 1) + get_val(in, i, j, 2)) / 3;
        }
    }

    return result;
}

ip_mat *ip_mat_brighten(ip_mat *a, float bright)
{
    ip_mat *result;

    is_null(a);

    result = ip_mat_add_scalar(a, bright);

    return result;
}

/* ci verrà fornito una nuova funzione get_normal 
 * che genererà numeri casuali gaussiani
 */
ip_mat *ip_mat_corrupt(ip_mat *a, float amount)
{
    unsigned int i, j, z;
    ip_mat *corrupted;
    float std = amount / 2.0;
    float a_val, rng, corrupted_val;

    is_null(a);

    corrupted = ip_mat_create(a->h, a->w, a->k, 0.0);
    /* ip_mat *copy = ip_mat_copy(a); */
    /*     ip_mat *random = ip_mat_create(a->h, a->w, a->k, 0);
    ip_mat_init_random(random, 0, 2 * amount); */
    for (i = 0; i < a->h; i++)
    {
        for (j = 0; j < a->w; j++)
        {
            for (z = 0; z < a->k; z++)
            {
                a_val = get_val(a, i, j, z);

                rng = get_normal_random(0, std);
                corrupted_val = a_val + (rng);
                /*printf("%f\n", corrupted_val);*/
                set_val(corrupted, i, j, z, corrupted_val);
            }
        }
    }
    clamp(corrupted, 0, 255);
    return corrupted;
}

/*---------------------------------------PARTE 3 -----------------------------------------*/

ip_mat *ip_mat_convolve(ip_mat *a, ip_mat *f)
{
    unsigned int i, j, z;

    unsigned int pad_h = ((f->h) - 1) / 2;
    unsigned int pad_w = ((f->w) - 1) / 2;
    ip_mat *result;
    ip_mat *padding;

    is_null(a);
    is_null(f);

    padding = ip_mat_padding(a, pad_h, pad_w);
    printf("paddata\n");
    ip_mat_show(padding);

    result = ip_mat_create(a->h, a->w, a->k, 0.0);
    /* ip_mat *result = ip_mat_create(f->h, f->w, f->k, 0.0); */

    /* row_start = a->h row_end = f->h 
     * col_start col_end
     *  */

    /* row_start = 0+1 row_end = f->h +1        row-end = a->h
     * col_start col_end
     *  */
    /* 
        sub_set(0 _ 2, 0 _ 2) 

        se faccio a resulti l padding sulla base del filtro lo paddo di 1 per lato
        che non è gucci
        sono riandato via

        283 -> 285 
        362 -> 364 

     */
    for (z = 0; z < padding->k; z++)
    {
        for (i = 0; i <= (padding->h - f->h); i++)
        {
            for (j = 0; j <= (padding->w - f->w); j++)
            {
                unsigned int r, c;
                float final_val = 0;

                ip_mat *sub = ip_mat_subset(padding, i, f->h + i, j, f->w + j);
                /* printf("SUB  i = %d, j = %d\n", i, j); */
                /* ip_mat_show(sub); */

                for (r = 0; r < sub->h; r++)
                {
                    for (c = 0; c < sub->w; c++)
                    {
                        /* printf("r=%d,c=%d\n", r, c); */
                        float cur_val = get_val(sub, r, c, z);
                        /* printf("curval = %f\n", cur_val); */

                        float new_val = cur_val * get_val(f, r, c, z);
                        /* printf("newval = %f\n", new_val); */
                        final_val += new_val;
                    }
                }
                set_val(result, i, j, z, final_val);
                ip_mat_free(sub);
            }
        }
    }
    ip_mat_free(padding);
    return result;
}

ip_mat *ip_mat_padding(ip_mat *a, unsigned int pad_h, unsigned int pad_w)
{
    unsigned int i, j, z;
    ip_mat *result;
    unsigned h = a->h + 2 * pad_h;
    unsigned w = a->w + 2 * pad_w;

    is_null(a);

    result = ip_mat_create(h, w, a->k, 0.0);

    for (i = 0; i < a->h; i++)
    {
        for (j = 0; j < a->w; j++)
        {
            for (z = 0; z < a->k; z++)
            {
                float val = get_val(a, i, j, z);
                set_val(result, i + pad_h, j + pad_w, z, val);
            }
        }
    }
    return result;
}

/* Crea un filtro di sharpening */
ip_mat *create_sharpen_filter()
{
    /*  creo una nuova ip_mat 3x3x3 e la inizializzo a 0*/
    ip_mat *filtro = ip_mat_create(3, 3, 3, 00.0);
    int k = 0;

    for (k = 0; k < 3; k++)
    {
        /* setto valori prima riga della matrice filtro */
        set_val(filtro, 0, 1, k, -1);

        /* setto valori seconda riga della matrice filtro */
        set_val(filtro, 1, 0, k, -1);
        set_val(filtro, 1, 1, k, 5);
        set_val(filtro, 1, 2, k, -1);

        /* setto valori della terza riga della matrice filtro */
        set_val(filtro, 2, 1, k, -1);
    }
    return filtro;
}

/* Crea un filtro per rilevare i bordi */
ip_mat *create_edge_filter()
{

    /* creo una nuova ip_mat 3x3x3 e la inizializzo a -1 */
    unsigned int k;
    ip_mat *filtro = ip_mat_create(3, 3, 3, -1.0);

    /* setto il centro a 8 */
    for (k = 0; k < 3; k++)
    {
        set_val(filtro, 1, 1, k, 8);
    }
    return filtro;
}

/* Crea un filtro per aggiungere profondità */
ip_mat *create_emboss_filter()
{

    /* creo una nuova ip_mat 3x3x3 e la inizializzo a 0  */

    ip_mat *filtro = ip_mat_create(3, 3, 3, 1.0);
    int k = 0;

    for (k = 0; k < 3; k++)
    {
        /*setto valori prima riga della matrice filtro*/
        set_val(filtro, 0, 0, k, -2);
        set_val(filtro, 0, 1, k, -1);
        set_val(filtro, 0, 2, k, 0);
        /*setto valori seconda riga della matrice filtro */
        set_val(filtro, 1, 0, k, -1);
        /*setto valori della terza riga della matrice filtro*/
        set_val(filtro, 2, 0, k, 0);
        set_val(filtro, 2, 2, k, 2);
    }

    return filtro;
}

/* Crea un filtro medio per la rimozione del rumore */
ip_mat *create_average_filter(unsigned int w, unsigned int h, unsigned int k)
{

    /*il filtro puo essere negativo?*/
    float val = 1;
    float jeez = h * w;
    float avg = val / jeez;

    ip_mat *avg_filter = ip_mat_create(w, h, k, avg);

    return avg_filter;
}

/* Crea un filtro gaussiano per la rimozione del rumore */
float val_kernel_gaus(int x, int y, float sigma)
{
    float result;
    result = (1 / ((2 * PI) * (pow(sigma, 2)))) * pow(E, -1 * (((pow(x, 2) + pow(y, 2))) / (2 * (pow(sigma, 2)))));

    return result;
}

ip_mat *create_gaussian_filter(unsigned int w, unsigned int h, unsigned int k, float sigma)
{

    /*  inizializzo gli indici cx e cy della cella centrale del kernel e le variabili per memorizzare le distanze e la somma  */
    unsigned int i, j, z;
    int distanza_x, distanza_y;
    /* pensare al piano cartesiano */
    int cx = h / 2;
    int cy = w / 2;
    float somma = 0;

    /*  creo una nuova ip map wxhxk */

    ip_mat *gaussian_filter = ip_mat_create(w, h, k, 0.00);

    /* per ogni valori del ciclo calcolo le nuove distanze e le passo come valori nella funzione val_kernel_gaus
   setto per ogni valore del ciclo il valore calcolato dalla funzione val_kernel_gauss */

    for (z = 0; z < k; z++)
    {
        for (i = 0; i < gaussian_filter->h; i++)
        {

            for (j = 0; j < gaussian_filter->w; j++)
            {
                float kernel_val;
                distanza_x = j - cx;
                distanza_y = i - cy;

                kernel_val = val_kernel_gaus(distanza_x, distanza_y, sigma);
                /* printf("kernel_val[%d,%d] = %f\n", i, j, kernel_val); */
                set_val(gaussian_filter, i, j, z, kernel_val);
            }
        }
    }

    /*   calcolo la somma dei valori del canale */

    for (i = 0; i < gaussian_filter->h; i++)
    {
        for (j = 0; j < gaussian_filter->w; j++)
        {
            somma = somma + get_val(gaussian_filter, i, j, 1);
            /*somma1 = somma1 + get_val(gaussian_filter, i, j, 1);
            somma2 = somma2 + get_val(gaussian_filter, i, j, 2); */
        }
    }

    /* setto i nuovi valori  dividendoli per la somma */
    for (z = 0; z < k; z++)
    {
        for (i = 0; i < gaussian_filter->h; i++)
        {

            for (j = 0; j < gaussian_filter->w; j++)
            {
                float new_val = get_val(gaussian_filter, i, j, z) / somma;
                set_val(gaussian_filter, i, j, z, new_val);
            }
        }
    }

    /* printf("la somma dei valori del filtro e' %lf\n", somma); */
    return gaussian_filter;
}

/* Effettua una riscalatura dei dati tale che i valori siano in [0,new_max].
 * Utilizzate il metodo compute_stat per ricavarvi il min, max per ogni canale.
 *
 * I valori sono scalati tramite la formula valore-min/(max - min)
 *
 * Si considera ogni indice della terza dimensione indipendente, quindi l'operazione
 * di scalatura va ripetuta per ogni "fetta" della matrice 3D.
 * Successivamente moltiplichiamo per new_max gli elementi della matrice in modo da ottenere un range
 * di valori in [0,new_max].
 * */
void rescale(ip_mat *t, float new_max)
{
    unsigned int i, j, z;

    is_null(t);
    compute_stats(t);

    for (z = 0; z < t->k; z++)
    {
        float min = t->stat->min;
        float max = t->stat->max;

        if (max == min)
        {
            printf("Non facciamo divisioni per 0 qui\n");
            exit(1);
        }

        for (i = 0; i < t->h; i++)
        {
            for (j = 0; j < t->w; j++)
            {

                float cur_val = get_val(t, i, j, z);

                if (cur_val < 0.0)
                {
                    set_val(t, i, j, z, 0.0);
                }
                else
                {
                    float new_val = ((cur_val - min) / (max - min)) * new_max;
                    set_val(t, i, j, z, new_val);
                }
            }
        }
    }
    compute_stats(t);
}

void clamp(ip_mat *t, float low, float high)
{
    unsigned int i, j, z;

    is_null(t);

    for (i = 0; i < t->h; i++)
    {
        for (j = 0; j < t->w; j++)
        {
            for (z = 0; z < t->k; z++)
            {
                if (t->data[i][j][z] > high)
                {
                    t->data[i][j][z] = high;
                }
                if (t->data[i][j][z] < low)
                {
                    t->data[i][j][z] = low;
                }
            }
        }
    }
}

/*lohackers*/
/* float get_normal_random_with_mean_var(float mean, float var)
{
    float sigma;
    assert(var >= 0);
    sigma = sqrt(var);
    return get_normal_random() * sigma + mean;
} */
