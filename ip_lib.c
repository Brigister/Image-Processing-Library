/* Created by Sebastiano Vascon on 23/03/20.
 *
 * Gruppo 16 Antenna 5G
 * 
 * Simone Zanon 858807
 * Francesco Favaro 859381
 * Riccardo Nalgi 881667
 * Manuel Boscolo 882852
 */

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

/*controllo puntatore ip_mat = null*/
void null_checker(ip_mat *t)
{
    if (t == NULL)
    {
        printf("il puntatore %p punta a NULL\n", (void *)&t);
        exit(1);
    }
}
/*mostra canali matrice ed annessi valori*/
void ip_mat_show(ip_mat *t)
{
    unsigned int i, l, j;

    null_checker(t);
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
/*mostra stats annesse di una ip_mat dopo averle calcolate*/
void ip_mat_show_stats(ip_mat *t)
{
    unsigned int k;
    null_checker(t);
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
    Bitmap *b;
    unsigned int i, j;

    null_checker(t);

    b = bm_create(t->w, t->h);

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
    null_checker(a);
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
    null_checker(a);
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
/*creazione di una ip_mat data altezza, larghezza, canali (3 per img) e valore elementi*/
ip_mat *ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v)
{
    unsigned int i, j, z;

    ip_mat *nuova;
    stats *stat;
    float ***data;
    nuova = (ip_mat *)malloc(sizeof(ip_mat));

    /* h = altezza o numero righe, w = larghezza o numero colonne, k= profondità o numero canali*/
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

    /* assegnazione valori a data */
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

    null_checker(a);
    if (a != NULL)
    {
        free(a->stat);

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

/*esibizione stats di una ip_mat per canale ; max e min per confronto. Mean con accumulatore e contatore.*/
void compute_stats(ip_mat *t)
{
    unsigned int i, j, z;

    null_checker(t);

    for (z = 0; z < t->k; z++)
    {
        float min, max;
        float counter = 0;
        float somma = 0;
        min = max = get_val(t, 0, 0, z);

        for (i = 0; i < t->h; i++)
        {
            for (j = 0; j < t->w; j++)
            {
                float temp = get_val(t, i, j, z);

                if (temp < min)
                {
                    min = temp;
                }
                if (temp > max)
                {
                    max = temp;
                }
                somma += temp;
                counter++;
            }
        }
        t->stat[z].max = max;
        t->stat[z].min = min;
        t->stat[z].mean = somma / counter;
    }
}
/*somma di due ip_mat, le dimensioni devono essere uguali*/
ip_mat *ip_mat_sum(ip_mat *a, ip_mat *b)
{
    unsigned int i, j, z;

    null_checker(a);
    null_checker(b);

    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("ip_mat_sum: Operazione non consentita, range matrici diverse\n");
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
/*differenza di due ip_mat, le dimensioni devono essere uguali*/
ip_mat *ip_mat_sub(ip_mat *a, ip_mat *b)
{
    unsigned int i, j, z;

    null_checker(a);
    null_checker(b);

    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("ip_mat_sub: Operazione non consentita, range matrici diverse\n");
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
/*sotto matrice di una ip_mat di input. Necessito di row/col start ed end della sottomatrice da creare*/
ip_mat *ip_mat_subset(ip_mat *t, unsigned int row_start, unsigned int row_end, unsigned int col_start, unsigned int col_end)
{
    unsigned int i, j, z;

    null_checker(t);
    /* controllo che le dimensioni di input non sforino la ip_mat di partenza*/
    if (row_start <= row_end && row_end <= t->h && col_start <= col_end && col_end <= t->w)
    {
        /*calcolo dimensione della sottomatrice, k arbitrario*/
        unsigned int valorant_alpha = row_end - row_start;
        unsigned int valorant_beta = col_end - col_start;
        ip_mat *sub_mat = ip_mat_create(valorant_alpha, valorant_beta, t->k, 0);
        for (z = 0; z < sub_mat->k; z++)
        {
            for (i = 0; i < sub_mat->h; i++)
            {
                for (j = 0; j < sub_mat->w; j++)
                {
                    /*setto i valori della sottomatrice prendendo i valori della matrice di partenza a partire da rowstart/colstart*/
                    set_val(sub_mat, i, j, z, get_val(t, i + row_start, j + col_start, z));
                }
            }
        }
        return sub_mat;
    }
    else
    {
        printf("ip_mat_subset: Operazione non consentita, range valori inaccettabili!\n");
        exit(1);
    }
}
/*concatenazione di due ip_mat su di una dimensione. Dimensione = 0, unione valori delle a->h+b->h*/
/* Dimensione =1, unione valori delle a->w+b->w */
/* Dimensione =2, unione valori delle a->k+b->k */
ip_mat *ip_mat_concat(ip_mat *a, ip_mat *b, int dimensione)
{

    unsigned int i, j, z;
    ip_mat *concatenata;

    null_checker(a);
    null_checker(b);

    /*check sul parametro in input dimensione e check essenziale sulle dimensioni delle ip_mat di input*/
    /*a-b devono avere le stesse dimensioni*/
    if (dimensione == 0)
    {
        if (a->w == b->w && a->k == b->k)
        {
            concatenata = ip_mat_create(a->h + b->h, a->w, a->k, 0);

            /*lavoriamo sulle h. Da 0<=i<=a->h prendo i valori di a->h, oltre prendo quelli di b->h*/
            /*sneakything -> get_val(b, i-(a->h)) per continuare nello stesso for*/
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
            printf("ip_mat_concat: Operazione non consentita, range matrici diverse\n");
            exit(1);
        }
    }
    else if (dimensione == 1)
    {
        if (a->h == b->h && a->k == b->k)
        {
            concatenata = ip_mat_create(a->h, a->w + b->w, a->k, 0);

            /*lavoriamo sulle w. Da 0<=j<=a->w prendo i valori di a->w, oltre prendo quelli di b->w*/
            /*sneakything -> get_val(b, i, j-(a->w)) per continuare nello stesso for*/
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
            printf("ip_mat_concat: Operazione non consentita, range matrici diverse\n");
            exit(1);
        }
    }
    else if (dimensione == 2)
    {
        if (a->h == b->h && a->w == b->w)
        {
            concatenata = ip_mat_create(a->h, a->w, a->k + b->k, 0);

            /*lavoriamo sulle k. Da 0<=z<=a->k prendo i valori di a->k, oltre prendo quelli di b->k*/
            /*sneakything -> get_val(b, i, j, z-(a->k)) per continuare nello stesso for*/
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
            printf("ip_mat_concat: Operazione non consentita, range matrici diverse\n");
            exit(1);
        }
    }
    else
    {
        printf("ip_mat_concat: Operazione non consentita, la dimensione deve esse compresa tra 0 e 2 :)\n");
        exit(1);
    }
    return concatenata;
}

/*copia una ip_mat di input in una nuova ip_mat. Stessi valori, stesse dimensioni*/
ip_mat *ip_mat_copy(ip_mat *in)
{
    unsigned int i, j, z;
    ip_mat *copy;

    null_checker(in);

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
/*calcola la media dei valori di due ip_mat, la restituisco in una nuova ip_mat con valore media canale per canale*/
/*dimensioni devono essere identiche.*/
ip_mat *ip_mat_mean(ip_mat *a, ip_mat *b)
{
    unsigned int i, j, z;

    null_checker(a);
    null_checker(b);

    /* verifico che le matrici date siano della stessa dimensione, altrimenti ritorno un printf di errore  */
    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("ip_mat_mean: Operazione non consentita, le matrici non sono uguali\n");
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
/*moltiplicazione tra una ip_mat, per ogni valore in ogni canale, ed uno scalare C dato in input*/
ip_mat *ip_mat_mul_scalar(ip_mat *a, float c)
{
    unsigned int i, j, z;
    float supp;
    ip_mat *nuova_ms;

    null_checker(a);

    nuova_ms = ip_mat_copy(a);
    for (i = 0; i < nuova_ms->h; i++)
    {
        for (j = 0; j < nuova_ms->w; j++)
        {
            for (z = 0; z < nuova_ms->k; z++)
            {
                supp = get_val(a, i, j, z);
                set_val(nuova_ms, i, j, z, supp * c);
            }
        }
    }
    return nuova_ms;
}
/*addizione tra una ip_mat, per ogni valore in ogni canale, ed uno scalare C dato in input*/
ip_mat *ip_mat_add_scalar(ip_mat *a, float c)
{
    unsigned int i, j, z;
    float supp;
    ip_mat *nuova_as;

    null_checker(a);

    nuova_as = ip_mat_copy(a);
    for (i = 0; i < nuova_as->h; i++)
    {
        for (j = 0; j < nuova_as->w; j++)
        {
            for (z = 0; z < nuova_as->k; z++)
            {
                supp = get_val(a, i, j, z);
                set_val(nuova_as, i, j, z, supp + c);
            }
        }
    }

    return nuova_as;
}
/*modifico i valori di una ip_mat di input con rand, ovvero un valore generato da una gaussian con media mean e varianza var */
/*utilizzo get_normal_random, metodo fornito*/
/*i valori dovrebbero rimanere var^2 + mean*/
void ip_mat_init_random(ip_mat *t, float mean, float var)
{
    unsigned int i, j, z;

    null_checker(t);

    for (i = 0; i < t->h; i++)
        for (j = 0; j < t->w; j++)
            for (z = 0; z < t->k; z++)
            {

                float rand = get_normal_random(mean, var);
                /* rand = rand * var + mean; */
                t->data[i][j][z] = rand;
            }
}

/*<-----------------------  PARTE 2: OPERAZIONI SULLE IMMAGINI   ---------------------------------->*/

/* Fusione di due ip_mat. Dimensioni devono combaciare. Controllo valore alpha rientri nel range */
/* comb convessa  = alpha*valoreA + (1-alpha)*valoreB in ogni canale */
ip_mat *ip_mat_blend(ip_mat *a, ip_mat *b, float alpha)
{

    null_checker(a);
    null_checker(b);

    if (a->h != b->h || a->w != b->w || a->k != b->k)
    {
        printf("ip_mat_blend: Le due ip_mat hanno dimensione diversa, impossibile fare il blend\n");
        exit(1);
    }
    else if (alpha < 0.0 || alpha > 1.0)
    {
        printf("ip_mat_blend: Il valore alpha non rientra nel range [0,1]\n");
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
                    /*computazione*/
                    float ercole = alpha * get_val(a, i, j, z);
                    float filottete = (1 - alpha) * get_val(b, i, j, z);
                    set_val(blend, i, j, z, ercole + filottete);
                }
        return blend;
    }
}
/*converto una ip_mat "RGB" ad una ip_mat su scala di grigio*/
/*lavoro sui valori dei canali, per ogni posizione, calcolo la media di quella posizione sui 3 canali ((0,0,0)+(0,0,1)+(0,0,2))/3*/
/*assegno questo valore nella posizione della ip_risultato, sui 3 canali -> il risultato avrà 3 canali dai valori uguali */
ip_mat *ip_mat_to_gray_scale(ip_mat *in)
{
    unsigned int i, j;
    ip_mat *result;

    null_checker(in);

    result = ip_mat_create(in->h, in->w, in->k, 0);

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
/*illuminazione di una ip_mat. Aggiungo il valore bright ad ogni valore in ogni canale e restituisco l'output*/
ip_mat *ip_mat_brighten(ip_mat *a, float bright)
{
    ip_mat *result;

    null_checker(a);

    result = ip_mat_add_scalar(a, bright);

    return result;
}

/*Corruzione di una ip_mat con un valore gaussiano modificato da amount. Valore output = val+gauss*amount  */
/* mi salvo il valore di a in una data posizione. Genero un valore dalla gaussiana con intervallo 0 e amount/2.*/
/* Sommo il valore precedentemente salvato con il valore generato per ottenere il valore corrotto e posiziono tutto nella sua posizione*/
ip_mat *ip_mat_corrupt(ip_mat *a, float amount)
{
    ip_mat *temp;
    ip_mat *corrupted;
    float std = amount / 2.0;

    null_checker(a);

    temp = ip_mat_copy(a);
    ip_mat_init_random(temp, 0, std);
    corrupted = ip_mat_sum(a, temp);

    return corrupted;
}

/*---------------------------------------PARTE 3 -----------------------------------------*/

/*convoluzione di una ip_mat a con un filtro f*/
/*calcolo i valori di padding dalle dimensioni del fitro input f (righe-1/2, colonne-1/2), e applico un padding alla matrice di input A*/
/*creo una matrice risultato con dimensioni uguali alla matrice di input A*/
/*tricky part ora : i valori della matrice risultato saranno la somma delle moltiplicazioni di ogni sottomatrice di A per il filtro F*/
/* per ottenere questo creo X sottomatrici di A, utilizzando le dimensioni del filtro per definire row/col END. */
/* scorro la sottomatrice e calcolo il valore finale -> valore posizione 0 0 0 sottomatrice + (valore posizione 0 0 0 sottomatrice * valore posizione 0 0 0 filtro)  */
/*posiziono in result i risultati*/
ip_mat *ip_mat_convolve(ip_mat *a, ip_mat *f)
{
    unsigned int i, j, z;

    unsigned int pad_h = ((f->h) - 1) / 2;
    unsigned int pad_w = ((f->w) - 1) / 2;
    ip_mat *result;
    ip_mat *padding;

    null_checker(a);
    null_checker(f);

    padding = ip_mat_padding(a, pad_h, pad_w);

    result = ip_mat_create(a->h, a->w, a->k, 0.0);

    for (z = 0; z < padding->k; z++)
    {
        for (i = 0; i <= (padding->h - f->h); i++)
        {
            for (j = 0; j <= (padding->w - f->w); j++)
            {
                unsigned int r, c;
                float final_val = 0;

                ip_mat *sub = ip_mat_subset(padding, i, f->h + i, j, f->w + j);
                /* printf("sub  i = %d, j = %d\n", i, j); */

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
/*padding di una ip_mat di input con valori condizionati da pad_h e pad_W*/
/* dovro aggiungere un padding orizzontale e verticale. il padding verticale equivale ad a->h + 2*pad_h */
/* il padding orizzonale equivarrà ad a->w + 2* pad_w   */
/* i valori rimangono quelli della ip_mat a iniziale, cambiano solo le dimensioni dell'output, appunto paddate*/
ip_mat *ip_mat_padding(ip_mat *a, unsigned int pad_h, unsigned int pad_w)
{
    unsigned int i, j, z;
    ip_mat *result;
    unsigned h = a->h + 2 * pad_h;
    unsigned w = a->w + 2 * pad_w;

    null_checker(a);

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

/* creazione filtro sharpen 3x3x3, 0 -1 0 /-1 5 -1/ 0-1 0  */
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

/* creazione filtro edge 3x3x3, -1 -1 -1 /-1 8 -1/ -1 -1 -1, tutti -1, 8 al centro  */
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

/* creazione filtro emboss 3x3x3, -2 -1 0 /-1 1 1/ 0 1 2 */
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

/* creazione filtro average w*h || se w=3 h=3 -> 3x3 c c c/ c c c / c c c  || c = 1/(w*h)  */
ip_mat *create_average_filter(unsigned int w, unsigned int h, unsigned int k)
{
    float val = 1;
    float denom = h * w;
    float avg = val / denom;

    ip_mat *avg_filter = ip_mat_create(w, h, k, avg);

    return avg_filter;
}

/* creazione del filtro gaussiano per rimuovere il rumore*/
ip_mat *create_gaussian_filter(unsigned int w, unsigned int h, unsigned int k, float sigma)
{

    /* 1) calcolo cella centrale del filtro */
    /* 2) calcolo la distanza dal centro per ogni posizione del filtro */
    /* 3) calcolo il kernel per ogni posizione del filtro con l'adeguata distanza appena calcolata  */
    /* 4) normalizzo il filtro, ovvero divido ogni valore per la somma di tutti i valori del filtro */
    /* et voilà */
    float sigma_quadrato = pow(sigma, 2.);
    float somma;
    unsigned int i, j, z;

    /* prendo il  centro di una matrice w*h con w e h di input */
    int cx = (h - 1) / 2;
    int cy = (w - 1) / 2;

    /*  creo una nuova ip map wxhxk */
    ip_mat *gaussian_filter = ip_mat_create(w, h, k, 0.00);
    /* per ogni valori del ciclo calcolo le nuove distanze e le passo come valori nella funzione val_kernel_gaus
   setto per ogni valore del ciclo il valore calcolato dalla funzione val_kernel_gauss */
    for (z = 0; z < k; z++)
    {
        somma = 0;

        for (i = 0; i < gaussian_filter->h; i++)
        {

            for (j = 0; j < gaussian_filter->w; j++)
            {
                float kernel_val;

                int x = i - cx;
                int y = j - cy;

                /* kernel_val = (1. / (2. * PI * sigma_quadrato)) * pow(E, -1 * (((pow(x, 2) + pow(y, 2))) / (2 * sigma_quadrato))); */
                kernel_val = (1. / (2. * PI * sigma_quadrato)) * exp(-1 * ((pow(x, 2) + pow(y, 2)) / (2 * sigma_quadrato)));
                somma += kernel_val;

                set_val(gaussian_filter, i, j, z, kernel_val);
            }
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

    return gaussian_filter;
}

/* Rescale di una ip_mat ovvero si assicura che i valori della ip_mat siano contenuti in un range [0, new_max]. New_max = valore in questione - min/(max-min). */
/* otteniamo max e min attraverso compute stats. Presente controllo max=min che comporterebbe una divisione per 0. */
void rescale(ip_mat *t, float new_max)
{
    unsigned int i, j, z;

    null_checker(t);
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

/*Altra funzione di rescale, che si basa su due valori [high,low] attraverso i quali confronta ogni valore della ip_mat di input*/
/*e se sono minori/maggiori li setta uguali ai valori del range su cui confronta */
void clamp(ip_mat *t, float low, float high)
{
    unsigned int i, j, z;

    null_checker(t);

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
