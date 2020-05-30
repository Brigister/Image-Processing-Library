#include <stdio.h>
#include "bmp.c"
#include "ip_lib.c"

int main()
{

    Bitmap *b = NULL;

    ip_mat *input_img = NULL;

    b = bm_load("u.bmp");

    Bitmap *c = NULL;

    ip_mat *input_img2 = NULL;

    c = bm_load("flower2.bmp");

    input_img = bitmap_to_ip_mat(b);
    input_img2 = bitmap_to_ip_mat(c);

    ip_mat *filtro1 = create_emboss_filter();

    ip_mat *filtro2 = create_edge_filter();

    ip_mat *filtro3 = create_sharpen_filter();
    ip_mat *nuova = ip_mat_create(7, 7, 3, 22);

    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            set_val(nuova, i, i, k, i);
        }
    }
    ip_mat_show(nuova);

    ip_mat *filtro4 = create_gaussian_filter(3, 3, 3, 5);
    ip_mat_show(filtro4);

    ip_mat *risultato = ip_mat_convolve(nuova, filtro4);

    ip_mat_show(risultato);

    /* rescale(input_img, 255);
    compute_stats(input_img);
    ip_mat_show_stats(input_img); */

    /* Bitmap *grande = ip_mat_to_bitmap(risultato);
    bm_save(grande, "gaussianUcinqueBIG.bmp"); */

    return 0;
}
