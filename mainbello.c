#include <stdio.h>
#include "bmp.c"
#include "ip_lib.c"

int main()
{

    Bitmap *b = NULL;

    ip_mat *input_img = NULL;

    b = bm_load("mongolfiere.bmp");

    Bitmap *c = NULL;

    ip_mat *input_img2 = NULL;

    c = bm_load("flower2.bmp");

    input_img = bitmap_to_ip_mat(b);
    input_img2 = bitmap_to_ip_mat(c);

    ip_mat *blend = ip_mat_blend(input_img, input_img2, 0.3);
    Bitmap *bmblend = ip_mat_to_bitmap(blend);
    bm_save(bmblend, "testblend.bmp");

    ip_mat *bright = ip_mat_brighten(input_img, 50);
    clamp(bright, 0, 255);
    Bitmap *bmbright = ip_mat_to_bitmap(bright);
    bm_save(bmbright, "testbright.bmp");

    ip_mat *corrupt = ip_mat_corrupt(input_img, 50);
    clamp(corrupt, 0, 255);
    Bitmap *bmcorrupt = ip_mat_to_bitmap(corrupt);
    bm_save(bmcorrupt, "testcorrupt5.bmp");

    ip_mat *gray = ip_mat_to_gray_scale(input_img);
    Bitmap *bmgray = ip_mat_to_bitmap(gray);
    bm_save(bmgray, "testgray.bmp");

    return 0;
}
