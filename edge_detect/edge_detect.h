//
//  edge_detect.h
//  edge_detect
//
//  Created by Claude Rogers on 7/7/11.
//  Copyright 2011 California Institute of Technology. All rights reserved.
//

void apply_mask (int mask[3][3], 
                 JSAMPLE *image_buffer, 
                 JSAMPLE *edge_buffer, int x, int y);

void detect_edges (JSAMPLE *image_buffer, JSAMPLE *edge_buffer);

void draw_lines (JSAMPLE *image_buffer, JSAMPLE *edge_buffer);

void analyze_image (JSAMPLE *image_buffer);