//
//  jpegio.h
//  edge_detect
//
//  Created by Claude Rogers on 7/7/11.
//  Copyright 2011 California Institute of Technology. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <setjmp.h>

#define IMAGE_HEIGHT 512
#define IMAGE_WIDTH 512
#define ROW_STRIDE (512*3)

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) 
my_error_exit(j_common_ptr cinfo);

GLOBAL(int) 
read_JPEG_file(char * filename, JSAMPLE *image_data, 
               int analyze_only, int r_thrs, int g_thrs);

GLOBAL(void)
write_JPEG_file (char *filename, int quality, JSAMPLE *image_buffer);