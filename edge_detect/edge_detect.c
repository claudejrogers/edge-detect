//
//  edge_detect.c
//  edge_detect
//
//  Created by Claude Rogers on 7/7/11.
//  Copyright 2011 California Institute of Technology. All rights reserved.
//

#include "jpegio.h"
#include "edge_detect.h"

void apply_mask (int mask[3][3], 
                 JSAMPLE *image_buffer, 
                 JSAMPLE *edge_buffer, int x, int y)
{
    int i, j, min, max, sum, index;
    min = 0;
    max = 255;
    sum = 0;
    index = 0;
    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            sum += image_buffer[x+(i*3)+y+(j*ROW_STRIDE)] * mask[i+1][j+1];
        }
    }
    if (sum < 0)
        sum = 0;
    if (sum > max)
        sum = max;
    if (sum > edge_buffer[x+y] && sum > 250)
        edge_buffer[x+y] = sum;
}

void detect_edges (JSAMPLE *image_buffer, JSAMPLE *edge_buffer)
{
    int i, x, y;
    int sobel_mask_0[3][3] = {
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1} };
    
    int sobel_mask_3[3][3] = {
        { 0, -1, -2},
        { 1,  0, -1},
        { 2,  1,  0} };
    
    int sobel_mask_7[3][3] = {
        { 0,  1,  2},
        {-1,  0,  1},
        {-2, -1,  0} };
    int max_x, max_y;
    max_x = 0;
    max_y = 0;
    for (i = 0; i < IMAGE_HEIGHT * ROW_STRIDE; i++)
        edge_buffer[i] = 0;
    for (y = (ROW_STRIDE); y < IMAGE_HEIGHT*ROW_STRIDE; y += (ROW_STRIDE)) {
        for (x = 3; x < ROW_STRIDE; x += 3) {
            if (x > max_x)
                max_x = x;
            if (y > max_y)
                max_y = y;
            apply_mask(sobel_mask_0, image_buffer, edge_buffer, x, y);
            apply_mask(sobel_mask_3, image_buffer, edge_buffer, x, y);
            apply_mask(sobel_mask_7, image_buffer, edge_buffer, x, y);
        }
    }
}

void draw_lines (JSAMPLE *image_buffer, JSAMPLE *edge_buffer)
{
    int i, j, sum;
    int summary[IMAGE_WIDTH], peaks[IMAGE_WIDTH];
    for (i = 0; i < ROW_STRIDE; i += 3) {
        sum = 0;
        for (j = 0; j < IMAGE_WIDTH*ROW_STRIDE; j+=(ROW_STRIDE)) {
            sum += edge_buffer[i+j];
        }
        summary[i / 3] = (sum / IMAGE_HEIGHT);
    }
    for (i = 0; i < IMAGE_WIDTH; i++)
        peaks[i] = 0;
    int last_index, last_value, this_value, flag;
    last_index = 0;
    last_value = 0;
    flag = 0;
    for (i = 0; i < IMAGE_WIDTH-2; i++) {
        this_value = abs(summary[i+2] - summary[i]);
        if (this_value > 20) {
            if (flag) {
                if (((i - last_index) < 50) && (last_index != 0)) {
                    if (this_value > last_value) {
                        peaks[i] = this_value;
                        peaks[last_index] = 0;
                        last_index = i;
                        last_value = this_value;
                    }
                } else if (i > 5) {
                    peaks[i] = this_value;
                    last_index = i;
                    last_value = this_value;
                }
            } else {
                peaks[i] = this_value;
                flag = 1;
                last_index = i;
                last_value = this_value;
            }
        }
    }
    int x, y;
    for (y = (ROW_STRIDE); y < IMAGE_HEIGHT*ROW_STRIDE; y+=(ROW_STRIDE)) {
        for (x = 3; x < ROW_STRIDE; x += 3) {
            if (peaks[x/3])
                image_buffer[x+y+2] = 255;
        }
    }
}

void analyze_image (JSAMPLE *image_buffer)
{
    int x, y, r, g, b;
    int g_pix;
    int flag;
    int lb;
    for (y = 100*ROW_STRIDE; y < IMAGE_HEIGHT*ROW_STRIDE; y+=100*ROW_STRIDE) {
        g_pix = 0;
        flag = 0;
        lb = 0;
        printf("%d:\t", (y / ROW_STRIDE));
        for (x = 0; x < ROW_STRIDE; x += 3) {
            r = image_buffer[x + y];
            g = image_buffer[x + y + 1];
            b = image_buffer[x + y + 2];
            if (b == 255) {
                flag += 1;
            }
            if (flag % 2) {
                if (g) {
                    g_pix++;
                }
            } else {
                printf("%d\t", g_pix);
                g_pix = 0;
                flag++;
                lb = x;
            }
        }
        printf("%d\n", g_pix);
    }
}