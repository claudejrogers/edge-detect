//
//  main.c
//  edge_detect
//
//  Created by Claude Rogers on 7/7/11.
//  Copyright 2011 California Institute of Technology. All rights reserved.
//

#include <unistd.h>
#include "jpegio.h"
#include "edge_detect.h"

int main (int argc, char * argv[])
{
    int c;
    char *filename = NULL;
    char *outname = NULL;
    int draw_flag = 1;
    int analyze_only = 0;
    int green_thrs = 120; 
    int red_thrs = 120;
    
    opterr = 0;
    
    while ((c = getopt(argc, argv, "f:o:dar:g:")) != -1)
        switch (c)
    {
        case 'f':
            filename = optarg;
            break;
        case 'o':
            outname = optarg;
            break;
        case 'd':
            draw_flag = 0;
            break;
        case 'a':
            analyze_only = 1;
            break;
        case 'g':
            green_thrs = atoi(optarg);
            break;
        case 'r':
            red_thrs = atoi(optarg);
            break;
        default:
            abort();
    }
    if (!filename) {
        fprintf(stderr, "Input filename required\n");
        return 1;
    }
    if (!outname && !analyze_only) {
        fprintf(stderr, "Output filename required\n");
        return 2;
    }
    JSAMPLE image_data[IMAGE_HEIGHT * IMAGE_WIDTH * 3];
    JSAMPLE edge_data[IMAGE_HEIGHT * IMAGE_WIDTH * 3];
    (void) read_JPEG_file(filename, image_data, 
                          analyze_only, red_thrs, green_thrs);
    if (analyze_only) {
        analyze_image(image_data);
        return 0;
    }
    detect_edges(image_data, edge_data);
    if (draw_flag)
        draw_lines(image_data, edge_data);
    analyze_image(image_data);
    write_JPEG_file(outname, 100, image_data);
    return 0;
}
