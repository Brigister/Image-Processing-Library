#include <stdio.h>
#include "ip_lib.c"
#include "bmp.c"

int main()
{
    int i;
    ip_mat *sum;
    ip_mat *sub;
    ip_mat *sub_mat;
    ip_mat *concatenata;

    /*     printf("NUOVA \n");
    ip_mat *nuova = ip_mat_create(3, 2, 3, 22.22);
    ip_mat_show(nuova);
    ip_mat_init_random(nuova, 20.0, 3.0);
    ip_mat_show(nuova); */

    /* bitmap->ip_mat->operazione->bitmap */

    Bitmap *c = NULL;
    Bitmap *b = NULL;

    ip_mat *input_img = NULL;
    ip_mat *input_img2 = NULL;

    b = bm_load("flower2.bmp");
    c = bm_load("mongolfiere.bmp");
    input_img = bitmap_to_ip_mat(b);
    input_img2 = bitmap_to_ip_mat(c);

    ip_mat *blend = ip_mat_blend(input_img, input_img2, 0);
    ip_mat_show_stats(blend);

    Bitmap *bmblend = ip_mat_to_bitmap(blend);
    bm_save(bmblend, "blended.bmp");

    bm_free(b);
    bm_free(c);
    bm_free(bmblend);
    ip_mat_free(input_img);
    ip_mat_free(input_img2);
    ip_mat_free(blend);

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
