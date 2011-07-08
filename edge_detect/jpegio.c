//
//  jpegio.c
//  edge_detect
//
//  Created by Claude Rogers on 7/7/11.
//  Copyright 2011 California Institute of Technology. All rights reserved.
//

#include "jpegio.h"

METHODDEF(void) 
my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

GLOBAL(int) 
read_JPEG_file(char * filename, JSAMPLE *image_data, 
               int analyze_only, int r_thrs, int g_thrs)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    
    FILE *infile;
    JSAMPARRAY buffer;
    int row_stride;
    
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        return 0;
    }
    
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return 0;
    }
    
    jpeg_create_decompress(&cinfo);
    
    jpeg_stdio_src(&cinfo, infile);
    
    (void) jpeg_read_header(&cinfo, TRUE);
    
    (void) jpeg_start_decompress(&cinfo);
    
    row_stride = cinfo.output_width * cinfo.out_color_components;
    
    buffer = (*cinfo.mem->alloc_sarray) 
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    
    while (cinfo.output_scanline < cinfo.output_height) {
        int y, i, j;
        unsigned char r, g, b;
        y = cinfo.output_scanline;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        for (i = 0; i < cinfo.output_width; i++) {
            r = buffer[0][i * 3];
            if (r < r_thrs)
                r = 0;
            g = buffer[0][i * 3 + 1];
            if (g < g_thrs)
                g = 0;
            b = buffer[0][i * 3 + 2];
            j = (y * IMAGE_HEIGHT) + i;
            image_data[j * 3] = r;
            image_data[j * 3 + 1] = g;
            if (analyze_only) {
                image_data[j * 3 + 2] = b;
            } else {
                image_data[j * 3 + 2] = 0;
            }
        }
    }
    
    (void) jpeg_finish_decompress(&cinfo);
    
    jpeg_destroy_decompress(&cinfo);
    
    fclose(infile);
    return 1;
}

GLOBAL(void)
write_JPEG_file (char *filename, int quality, JSAMPLE *image_buffer)
{
    struct jpeg_compress_struct cinfo;
    
    struct jpeg_error_mgr jerr;
    
    FILE *outfile;
    JSAMPROW row_pointer[1];
    
    cinfo.err = jpeg_std_error(&jerr);
    
    jpeg_create_compress(&cinfo);
    
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        (void) exit(0);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = IMAGE_WIDTH;
    cinfo.image_height = IMAGE_HEIGHT;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    
    jpeg_start_compress(&cinfo, TRUE);
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * ROW_STRIDE];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}
