// gc.h - garbage collection declarations
#pragma once
enum Gc_color {
    GC_FREE = 0,
    GC_BLACK = 1,
    GC_GRAY = 2,
    GC_WHITE = 3
};

extern Gc_color initial_color;
extern bool write_block;

void if_node_make_gray(Any x);
void basic_obj_make_gray(Basic_obj *obj);
void gc_poll();
