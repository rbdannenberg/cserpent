// gc.cpp - garbage collection
//
// Roger B. Dannenberg
// Jun 2024

#include "any.h"
#include "op_overload.h"
#include "gc.h"
#include "basic_obj.h"
#include "obj.h"
#include "csstring.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "csmem.h"

#if GC_DEBUG
// 1 is an address that will never match
Basic_obj *gc_trace_ptr = (Basic_obj *) 0x00000001080098a8;

char *color_name[] = {"FREE", "BLACK", "GRAY", "WHITE"};
char *tag_name[] = { "free", "symbol", "integer", "string",
                     "array", "dict", "object", "file" };


void gc_trace(Basic_obj *obj, const char *msg)
{
    gc_trace_2(obj, msg, -9999);
}


void gc_trace_2(Basic_obj *obj, const char *msg, int index)
{
    if (obj == gc_trace_ptr) {
        Basic_obj *next = obj->get_next();
        assert((uint64_t) next == 0 || (uint64_t) next > 0x100000000);
        printf("gc_trace %p in %s,", obj, msg);
        if (index != -9999) {
            printf(" %d", index);
        }
        printf(" tag %s (%d) color %s (%d) size %lld\n",
               tag_name[obj->get_tag()], obj->get_tag(),
               color_name[obj->get_color()], obj->get_color(), obj->get_size());
    }
}
#endif

Gc_states gc_state = GC_START;

Gc_color gc_initial_color = GC_WHITE;
bool gc_write_block = false;
bool gc_local_write_block = false;
int64_t gc_cycles = 0;

Basic_obj *gc_gray_list = nullptr;
static Chunk *sweep_chunk = nullptr;
static char *sweep_ptr = nullptr;
Array *gc_array = nullptr;
int64_t gc_array_index = 0;
int64_t gc_high_water = CHUNK_SIZE / 2;
Gc_frame *gc_frame_ptr = NULL;
void *gc_stack_top;  // link to top stack frame
Gc_color gc_frame_color;  // initial color for new frames


static const int SWEEP_NODE_COST = 1;
static const int FREE_NODE_COST = 2;
static const int COLOR_NODE_COST = 1;
static const int MARK_NODE_COST = 1;
static const int WORK_PER_POLL = 1000;


void if_node_make_gray(Any x) {
    if (is_basic_obj(x)) {
        basic_obj_make_gray(to_basic_obj(x));
    }
    // in the case of String and Symbol, each is either self-contained
    // as a short string or a pointer to a std::string, so there is
    // nothing to do in the Mark phase
}


void basic_obj_make_gray(Basic_obj *obj) {
    gc_trace(obj, "basic_obj_make_gray");
    if (obj && obj->get_color() == GC_BLACK) {
        obj->set_color(GC_GRAY);
        obj->set_next(gc_gray_list);
        gc_gray_list = obj;
    }
}



// do a unit of GC
void gc_poll()
{
    int64_t work_done = 0;
    Basic_obj *obj;
    char *ptr;
    while (work_done < WORK_PER_POLL) {
        printf("gc_poll (top loop) state %d\n", gc_state);
        switch (gc_state) {
        case GC_START:
            // printf("*** Starting GC cycle ***\n");
            gc_initial_color = GC_WHITE;
            gc_write_block = true;
//            basic_obj_make_gray(cs_symbols); I think this is no longer
//            necessary because the symbol table is in
//            global memory, not on either heap. [RBD:] But then we need
//            to mark everything referenced by globals. Isn't it better to
//            use a cserpent dictionary as a symbol table? GC_START should
//            take (small) constant time.
            gc_mark_roots(); // mark objects accessible through globals
            gc_state = GC_MARK;
            break;
        case GC_MARKB:
        case GC_MARK:
            while (work_done < WORK_PER_POLL) {
                obj = gc_gray_list;
                gc_trace(obj, "GC_MARK gray list");
                if (!obj) {
                    gc_state = (gc_state == GC_MARK ? GC_MARK2 : GC_MARK3);
                    break;
                }
                gc_gray_list = gc_gray_list->get_next();
                obj->set_white();
                
                switch (obj->get_tag()) {
                case tag_free:
                    break;
                case tag_symbol:
                    // since this is a Symbol, we know the 3 slots are pointers
                    // or nil, so we don't have to decode Any to see if it is
                    // a Basic_obj, and we can call basic_obj_make_gray directly
                    basic_obj_make_gray((Basic_obj *)
                                        (obj->slots[0].integer));  // name
                    basic_obj_make_gray((Basic_obj *)
                                        (obj->slots[1].integer));  // value
                    basic_obj_make_gray((Basic_obj *)
                                        (obj->slots[2].integer));  // function
                    work_done += 4 * MARK_NODE_COST;
                    break;
                case tag_string:
                    work_done += MARK_NODE_COST;
                    break;
                case tag_array: 
                case tag_dict: {
                    Array *array = (Array *) obj;
                    std::vector<Any> *data = (std::vector<Any> *) obj->slots;
                    // if the array is big, we want to mark the array
                    // incrementally to avoid a long pause. First, calculate
                    // work to do:
                    int64_t len = data->size();
                    int64_t work_to_do = (len + 2) * MARK_NODE_COST;
                    int64_t time_left = WORK_PER_POLL - work_done;
                    if (time_left > work_to_do) {  // do it all here and now:
                        for (int64_t  i = 0; i < len; i++) {
                            Any element = (*data)[i];
                            if_node_make_gray(element);
                        }
                        work_done += work_to_do;
                    } else {
                        gc_state = (gc_state == GC_MARK ? GC_MARK_ARRAY :
                                                          GC_MARK_ARRAYB);
                        gc_array = array;
                        gc_array_index = 0;
                    }
                    break;
                }
                case tag_object:
                case tag_file: {
                    Obj *o = (Obj *) obj;
                    int64_t slots = o->get_slot_count();
                    uint64_t anyslots = o->get_any_slots();  // returns bit set
                    for (int64_t i = 0; i < slots; i++) {
                        if (((int64_t) 1 << i) & anyslots) {
                            if_node_make_gray(o->slots[i]);
                        }
                    }
                    work_done += slots * MARK_NODE_COST;
                    break;
                }
                default:
                    assert(false);
                    break;
                }
            }
            break;
        case GC_MARK_ARRAY:
        case GC_MARK_ARRAYB: {
            std::vector<Any> *data = (std::vector<Any> *) gc_array;
            if (data->size() < gc_array_index) {
                gc_array_index = data->size();
            }
            int64_t iterations = (WORK_PER_POLL - work_done) / MARK_NODE_COST;
            int64_t left_to_do = data->size() - gc_array_index;
            if (iterations > left_to_do) {
                iterations = left_to_do;
            }
            work_done += iterations * MARK_NODE_COST;
            while (iterations-- > 0) {
                if_node_make_gray((*data)[gc_array_index++]);
            }
            if (gc_array_index >= data->size()) {
                // we're done scanning the array
                gc_state = (gc_state == GC_MARK_ARRAY ? GC_MARK : GC_MARKB);
            } else {
                return;  // we're out of time; come again soon
            }
            break;
        }
        case GC_MARK2:  // after GC_MARK; start marking frames
            gc_frame_ptr = (Gc_frame *) gc_stack_top;
            gc_frame_color = GC_WHITE;
            gc_local_write_block = true;
            gc_state = GC_MARK_STACK;
        case GC_MARK_STACK:
            printf("gc_poll state %d\n", gc_state);
            while (work_done < WORK_PER_POLL && gc_frame_ptr) {
                if (static_cast<Gc_color>(
                        (gc_frame_ptr->header >> 57) & 0x03) != GC_WHITE) {
                    int n = (gc_frame_ptr->header >> 45) & 0xFFF;  // slot cnt
                    for (int i = 0; i < n; i++) {
                        basic_obj_make_gray(to_basic_obj(
                                                    gc_frame_ptr->anys[i]));
                    }
                    work_done += n * MARK_NODE_COST;
                }
            }
            if (!gc_frame_ptr) {
                gc_state = GC_MARKB;  // mark gray list again
            }
            break;
        case GC_MARK3:
            // printf("Start GC_SWEEP\n");
            gc_write_block = false;  // nothing more will be put on gray list
            gc_local_write_block = false;
            sweep_chunk = cs_chunk_list;
            sweep_ptr = sweep_chunk->chunk;  // iterates over objects
            gc_initial_color = GC_GRAY;
            gc_frame_color = GC_BLACK;
            work_done += SWEEP_NODE_COST;
            gc_state = GC_SWEEP;
            // fall through to GC_SWEEP
            printf("gc_poll state %d\n", gc_state);
        case GC_SWEEP: {  // free black nodes, oh sweet jesus this sounds
            // so racist; I'd switch to RED/GREEN right now, but I'm a member
            // of the Cherokee nation.
            // In all seriousness, PLEASE suggest better terminology if you
            // find these WHITE/GRAY/BLACK labels inappropriate or insensitive.
            // Maybe HOT, WARM, COLD?
            while (work_done < WORK_PER_POLL) {
                printf("    sweep_ptr %p sweep_chunk %p work_done %lld\n",
                       sweep_ptr, sweep_chunk, work_done);
                Basic_obj *obj = (Basic_obj *) sweep_ptr;
                gc_trace(obj, "GC_SWEEP");
                int64_t sz = obj->get_size();
                // printf("sweep_ptr %p, color %d, obj size %lld\n", sweep_ptr,
                //        obj->get_color(), sz);
                if (obj->get_color() == GC_BLACK) {
                    if (obj->has_tag(tag_string)) {
                        delete (reinterpret_cast<std::string *>(
                                                    obj->slots[0].integer));
                        work_done += MARK_NODE_COST;
                        CSFREE(obj);
                    } else if (obj->has_tag(tag_file)) {
                        /* TODO: implement Cs_file
                        Cs_file *csfile = (Cs_file *) obj;
                        if (csfile->get_file()) {
                            csfile->get_file()->close();
                        }
                        csfile->set_file(NULL);
                         */
                    }
                    if (!obj->has_tag(tag_free)) {
                        CSFREE(obj);
                        work_done += FREE_NODE_COST;
                    }
                } else if (obj->get_color() == GC_FREE) {
                    work_done += SWEEP_NODE_COST;
                } else if (obj->get_color() == GC_WHITE) {
                    gc_trace(obj, "GC_SWEEP, changing white to black");
                    obj->set_color(GC_BLACK);
                    work_done += COLOR_NODE_COST;
                }
                sweep_ptr += ((sz + 7) & ~7);  // round up to 8-byte alignment
                if (sweep_ptr >= sweep_chunk->next_free) {
                    // printf("sweep_ptr %p sweep_chunk->next_free %p\n",
                    //        sweep_ptr, sweep_chunk->next_free);
                    sweep_chunk = sweep_chunk->next;
                    if (sweep_chunk) {
                        sweep_ptr = sweep_chunk->chunk;
                    } else {
                        printf("    sweep_chunk is null\n");
                        gc_initial_color = GC_BLACK;
                        // prepare to change stack frames to black
                        gc_frame_ptr = (Gc_frame *) gc_stack_top;
                        gc_state = GC_SWEEP1;
                        // printf("** start GC_SWEEP1 **\n");
                        break;
                    }
                }
            }
            break;
        }
        case GC_SWEEP1:  // change all stack frames to black
            while (work_done < WORK_PER_POLL && gc_frame_ptr) {
                gc_frame_ptr->header = 
                        (gc_frame_ptr->header & ~0x0600000000000000uLL) |
                        (static_cast<uint64_t>(GC_BLACK) << 57);
                gc_frame_ptr = (Gc_frame *)
                        ((gc_frame_ptr->header & ~0xFFFFE00000000000uLL) << 3);
                work_done += COLOR_NODE_COST;
            }
            if (!gc_frame_ptr) {
                gc_state = GC_SWEEP2;
            }
            break;
        case GC_SWEEP2:
            // in this phase, we take gray objects that were created during
            // GC_SWEEP and make them black
            while (work_done < WORK_PER_POLL && gc_gray_list) {
                obj = gc_gray_list;
                gc_gray_list = gc_gray_list->get_next();
                gc_trace(obj, "GC_SWEEP2, off gc_gray_list, set black");
                obj->set_color(GC_BLACK);
                work_done += COLOR_NODE_COST;
            }
            if (!gc_gray_list) {
                // printf("** end GC_SWEEP2 **\n");
                gc_state = GC_FINAL;
            }
            break;
        case GC_FINAL:
            // set high water mark where gc starts again to 1.5 times current
            // amount of memory in use:
            gc_high_water = cs_current_bytes_allocated;
            gc_high_water += (gc_high_water >> 1);
            gc_state = GC_IDLE;
            gc_cycles++;
            printf("*** GC cycle ended ***\n");
            return;  // exit loop; do not look for more work
        case GC_IDLE:
            if (cs_current_bytes_allocated > gc_high_water) {
                gc_state = GC_START;
            }
            return;  // exit loop because our state might still be GC_IDLE
        default:
            assert(false);
            break;
        }
    }
}


// gc_alter_array -- notify GC that an array has changed.
//
// This is important when, e.g. Array::uninsert is called. If we are in
// the middle of scanning an array, then by moving elements from the end
// of the array to the front, the elements might "hop over" the scan
// and escape garbage collection. Also, if the array length changes while
// scanning, an assertion is violated.
//     To solve all these problems, the array is simply rescanned if
// altered during a scan. This could make the GC not terminate, but
// we're going to assume that arrays are not such hot spots that this
// could happen.
//
void gc_alter_array(Array *a)
{
    if (gc_state == GC_MARK_ARRAY && gc_array == a) {
        gc_array_index = 0; // start over
    }
}

#if GC_DEBUG
void list_check(Basic_obj *head, long slots) {
    extern Cs_class *cs_obj_class;  // defined in gc_test
    while (head) {
        // printf("list_check for %ld slots: %p\n", slots, head);
        assert(head != cs_class_class);
        assert(head != cs_obj_class);
        int64_t actual = head->get_slot_count();
        assert(actual == slots);
        assert(head->get_color() == GC_FREE);
        assert(head->get_tag() == tag_free);
        head = head->get_next();
    }
}

// heap_check - scan heap and check for valid structure
//
// copied from csmem.cpp:
#define LOG2_MAX_LINEAR_BYTES 9 // up to (512 - 16) byte chunks
#define MAX_LINEAR_BYTES (1 << LOG2_MAX_LINEAR_BYTES)
#define LOG2_MAX_EXPONENTIAL_BYTES 25 // up to 16MB = 2^24
#define LOG2_MEM_QUANTUM 4  // linear sizes increment by this
#define MEM_QUANTUM (1 << LOG2_MEM_QUANTUM)
extern Basic_obj *linear_free[MAX_LINEAR_BYTES / MEM_QUANTUM - 1];
extern Basic_obj *exponential_free[LOG2_MAX_EXPONENTIAL_BYTES -
                                   LOG2_MAX_LINEAR_BYTES];
//
// controls for what heap_scan does:
constexpr int heap_check = 0;  // expensive scan heap and make tests
constexpr int heap_print = 1;  // print all objects in use (not free)
constexpr int gray_check = 2;  // look for gray objects

static void heap_scan(int fn)
{
    Chunk *chunk = cs_chunk_list;
    char *ptr = chunk->chunk;  // iterates over objects
    Basic_obj *obj = nullptr;
    Basic_obj *prev = nullptr;
    int64_t sz = 0;
    
    while (true) {
        prev = obj;
        int64_t prev_size = sz;
        
        obj = (Basic_obj *) ptr;  // iterates over objects
        sz = obj->get_size();
        obj->get_next();  // checks reasonable next field
        if (obj->has_tag(tag_object)) {
            assert(obj->slots[0].integer > 0x100000000);
        }
        if (fn == heap_print && obj->has_tag(tag_object)) {
            printf("obj on heap: %p\n", obj);
        }
        if (fn == gray_check && obj->get_color() == GC_GRAY) {
            printf("found gray object: %p\n", obj);
            assert(false);
        }
        ptr += ((sz + 7) & ~7);  // round up to 8-byte alignment
        if (ptr >= chunk->next_free) {
            assert(ptr == chunk->next_free);
            chunk = chunk->next;
            if (chunk) {
                ptr = chunk->chunk;
            } else {
                break;
            }
        }
    }

    // make sure all free lists only contain freed objects of the right
    // size
    long elem_size = 16;
    for (int i = 0; i < MAX_LINEAR_BYTES / MEM_QUANTUM - 1; i++) {
        long sz = (i + 1) << LOG2_MEM_QUANTUM;
        long slots = (sz - 8) / 8;
        list_check(linear_free[i], slots);
    }
    elem_size = MAX_LINEAR_BYTES;
    for (int i = 0; i < LOG2_MAX_EXPONENTIAL_BYTES - LOG2_MAX_LINEAR_BYTES;
         i++) {
        long sz = 1 << (i + LOG2_MAX_LINEAR_BYTES);
        long slots = (sz - 8) / 8;
        list_check(exponential_free[i], slots);
    }
}

void gc_heap_print()
{
    heap_scan(heap_print);
}

void gc_gray_check()
{
    if (gc_state == GC_IDLE) {  // gray list should be empty
        // printf("gray check ...\n");
        heap_scan(gray_check);
    }
}
#endif

#if GC_DEBUG_2
void gc_heap_check()
{
    heap_scan(heap_check);
}
#endif
