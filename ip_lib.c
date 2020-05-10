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
        printf("Errore get_val!\n");
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
        printf("Errore set_val!\n");
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

    /*creazione*/
    float ***data = (float ***)malloc(h * sizeof(float **));

    if (data == NULL)
    {
        printf("Ritenta, sarai più fortunato");
        exit(1);
    }

    for (int i = 0; i < h; i++)
    {
        data[i] = (float **)malloc(w * sizeof(float *));
        if (data[i] == NULL)
        {
            printf("Ritenta, sarai più fortunato");
            exit(1);
        }

        for (int j = 0; j < w; j++)
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
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            for (int z = 0; z < k; z++)
                data[i][j][z] = v;

    nuova->data = data;

    return nuova;
}

/* Libera la memoria (data, stat e la struttura) */
void ip_mat_free(ip_mat *a)
{
    int i, j, z;

    free(a->stat);
    printf("struttura statistiche liberata\n");

    for (i = 0; i < a->h; i++)
    {
        for (j = 0; j < a->w; j++)
        {
            free((a->data[i][j]));
        }
        free((a->data[i]));
    }
    printf("array liberato\n");
    free(a->data);
    printf("puntatore array liberato\n");
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

ip_mat *ip_mat_subset(ip_mat *t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{
    int i, j, z;

    if (row_start >= 0 && row_start <= row_end && row_end <= t->h && col_start >= 0 && col_start <= col_end && col_end < t->w)
    {
        ip_mat *sub_mat = ip_mat_create(row_end, col_end, t->k, 0);
        /*https://moodle.unive.it/mod/forum/discuss.php?d=63261 */
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

ip_mat *ip_mat_copy(ip_mat *in)
{

    /*  variabili di scorrimento per i cicli */

    int i, j, z;

    /* creo una nuova matrice di dimensioni uguali a quella data e la inizializzo a 0 */

    ip_mat *copy = ip_mat_create(in->h, in->w, in->k, 00.0);

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

    int i, j, z;

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
    int i, j, z;
    float supp;
    int h = a->h;
    int w = a->w;
    int k = a->k;
    ip_mat *nuova_ms = ip_mat_create(h, w, k, 9.9);
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
    int i, j, z;
    float supp;
    int h = a->h;
    int w = a->w;
    int k = a->k;
    ip_mat *nuova_as = ip_mat_create(h, w, k, 9.9);
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
    int i, j, k;

    for (int i = 0; i < t->h; i++)
        for (int j = 0; j < t->w; j++)
            for (int z = 0; z < t->k; z++)
            {

                float rand = get_normal_random();
                rand = rand * var + mean;
                t->data[i][j][z] = rand;
            }
}

void clamp(ip_mat *t, float low, float high)
{
    int i, j, k;

    for (int i = 0; i < t->h; i++)
    {
        for (int j = 0; j < t->w; j++)
        {
            for (int z = 0; z < t->k; z++)
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

ip_mat *ip_mat_blend(ip_mat *a, ip_mat *b, float alpha)
{
    if (alpha < 0.0 || alpha > 1.0)
    {
        printf("Valore blend non corretto");
        exit(1);
    }
    else
    {
        int i, j, z;

        ip_mat *blend = ip_mat_create(a->h, a->w, a->k, 0);

        for (int i = 0; i < a->h; i++)
            for (int j = 0; j < a->w; j++)
                for (int z = 0; z < a->k; z++)
                {
                    /*bait*/
                    float ercole = alpha * get_val(a, i, j, z);
                    float filottete = (1 - alpha) * get_val(b, i, j, z);
                    set_val(blend, i, j, z, ercole + filottete);
                }
        clamp(blend, 0, 255);
        return blend;
    }
}

ip_mat *ip_mat_to_gray_scale(ip_mat *in); /* riccardo */

ip_mat *ip_mat_brighten(ip_mat *a, float bright)
{
    ip_mat *copy = ip_mat_copy(a);

    ip_mat *result = ip_mat_add_scalar(copy, bright);
    clamp(result, 0.0, 255.0);
    return result;
}

ip_mat *ip_mat_corrupt(ip_mat *a, float amount)
{
    float media;
    ip_mat *copy = ip_mat_copy(a);

    ip_mat *random = ip_mat_create(a->h, a->w, a->k, 0);
    ip_mat_init_random(random, media, amount);
    ip_mat *result = ip_mat_sum(copy, random);
    clamp(result, 0, 255);

    return result;
}

/*---------------------------------------PARTE 3 -----------------------------------------*/

/* Effettua la convoluzione di un ip_mat "a" con un ip_mat "f".
 * La funzione restituisce un ip_mat delle stesse dimensioni di "a".
 * */
ip_mat *ip_mat_convolve(ip_mat *a, ip_mat *f);

/* Aggiunge un padding all'immagine. Il padding verticale è pad_h mentre quello
 * orizzontale è pad_w.
 * L'output sarà un'immagine di dimensioni:
 *      out.h = a.h + 2*pad_h;
 *      out.w = a.w + 2*pad_w;
 *      out.k = a.k
 * con valori nulli sui bordi corrispondenti al padding e l'immagine "a" riportata
 * nel centro
 * */
ip_mat *ip_mat_padding(ip_mat *a, int pad_h, int pad_w);

/* Crea un filtro di sharpening */
ip_mat *create_sharpen_filter();

/* Crea un filtro per rilevare i bordi */
ip_mat *create_edge_filter();

/* Crea un filtro per aggiungere profondità */
ip_mat *create_emboss_filter();

/* Crea un filtro medio per la rimozione del rumore */
ip_mat *create_average_filter(int w, int h, int k);

/* Crea un filtro gaussiano per la rimozione del rumore */
ip_mat *create_gaussian_filter(int w, int h, int k, float sigma);

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
void rescale(ip_mat *t, float new_max);