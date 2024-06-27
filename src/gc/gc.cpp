// gc.cpp - garbage collection
//
// Roger B. Dannenberg
// Jun 2024

enum {
    GC_START,
    GC_MARK,
    GC_MARK_ARRAY,
    GC_MARK2,
    GC_MARK3,
    GC_SWEEP,
    GC_SWEEP2,
    GC_FINAL,
    GC_IDLE
} gc_state = GC_START;

Gc_color initial_color = GC_WHITE;
static Basic_obj *gray_list = nullptr;
static Chunk *sweep_chunk = nullptr;
static char *sweep_ptr = nullptr;

const int SWEEP_NODE_COST = 1;
const int FREE_NODE_COST = 2;
const int COLOR_NODE_COST = 1;
const int MARK_NODE_COST = 1;
const int WORK_PER_POLL = 1000;


// do a unit of GC
void gc_poll()
{
    int64_t work_done = 0;
    Basic_obj *obj;
    char *ptr;
    while (work_done < WORK_PER_POLL) {
        switch (gc_state) {
        case GC_START:
            initial_color = GC_WHITE;
            write_block = true;
            if_node_make_gray(symbols);
            gc_state = GC_MARK;
            break;
        case GC_MARK:
            while (work_done < WORK_PER_POLL) {
                obj = gray_list;
                if (!obj) {
                    gc_state = GC_MARK2;
                    break;
                }
                gray_list = gray_list->get_next();
                obj->set_color_2(GC_WHITE);
                switch (obj->get_tag()) {
                case tag_free:
                case tag_arraydata:
                    assert(false);
                    break;
                case tag_symbol:
                    Symbol *sym = (Symbol *) obj;
                    if_node_make_gray(sym->name);
                    if_node_make_gray(sym->value);
                    if_node_make_gray(sym->method);
                    work_done += 4 * MARK_NODE_COST;
                    break;
                case tag_string:
                    work_done += MARK_NODE_COST;
                    break;
                case tag_array:
                case tag_dict:
                    Array *array = (Array *) obj;
                    Array_data *data = array->data;
                    if (data) {
                        data->set_color(GC_WHITE);
                    }
                    // if the array is big, we want to mark the array incrementally
                    // to avoid a long pause. First, calculate work to do:
                    int64_t len = array->get_array_gc_len();
                    int64_t work_to_do = (len + 2) * MARK_NODE_COST;
                    int64_t time_left = WORK_PER_POLL - work_done;
                    if (time_left > work_to_do) {  // do it all here and now:
                        gc_array_len = len;
                        if (data) {
                            for (i = 0; i < len; i++) {
                                Any element = data->nodes[i];
                                if_node_make_gray(element);
                            }
                            work_done += work_to_do;
                        }
                    } else {
                        gc_state = GC_MARK_ARRAY;
                        gc_array = array;
                        gc_array_index = 0;
                    }                        
                    break;
                case tag_object:
                case tag_file:
                    long slots = obj->get_obj_slots();
                    long anyslots = obj->get_any_slots();  // returns bit set
                    for (i = 0; i < slots; i++) {
                        if ((1 << i) & anyslots) {
                            if_node_make_gray(obj->slots[i]);
                        }
                    }
                    work_done += slots * MARK_NODE_COST;
                    break;
                default:
                    assert(false);
                    break;
                }
            }
            break;
        case GC_MARK_ARRAY:
            if (gc_array->get_array_gc_len() < gc_array_index) {
                gc_array_index = gec_array->get_array_gc_len();
            }
            int64_t iterations = (WORK_PER_POLL - work_done) / MARK_NODE_COST;
            int64_t left_to_do = gc_array->get_array_gc_len() - gc_array_index;
            if (iterations > left_to_do) {
                iterations = left_to_do;
            }
            work_done += iterations * MARK_NODE_COST;
            if (gc_array->data) {
                while (iterations-- > 0) {
                    if_node_make_gray(gc_array->data->nodes[gc_array_index++]);
                }
            } else {  // no data, so make this scan come to an end
                gc_array_index = gc_array->get_array_gc_len();
            }
            if (gc_array_index >= gc_array->get_array_gc_len()) {
                // we're done scanning the array
                gc_state = GC_MARK;
            } else {
                return;  // we're out of time; come again soon
            }
            break;
        case GC_MARK2:  // after GC_MARK; not used yet, so fall through
        case GC_MARK3:
            write_block = false;  // nothing more will be put on gray list
            sweep_chunk = cs_chunk_list;
            sweep_ptr = sweep_chunk->chunk;  // iterates over objects
            initial_color = GC_GRAY;
            work_done += SWEEP_NODE_COST;
            // fall through to GC_SWEEP
        case GC_SWEEP: {  // free black nodes, oh sweet jesus this sounds so racist;
            // I'd switch to RED/GREEN right now, but I'm a member of the Cherokee nation.
            // In all seriousness, PLEASE suggest better terminology if you find these
            // WHITE/GRAY/BLACK labels inappropriate or insensitive.
            while (work_done < WORK_PER_POLL) {
                if (sweep_chunk) {
                    sweep_ptr = sweep_chunk->chunk;
                } else {
                    initial_color = GC_BLACK;
                    gc_state = GC_SWEEP2;
                    break;
                }
                Basic_obj *obj = (Basic_obj *) sweep_ptr;
                int64_t sz = obj->get_size();
                if (obj->get_color() == GC_BLACK) {
                    if (obj->has_tag(tag_file)) {
                        Cs_file *csfile = (Cs_file *) obj;
                        if (csfile->get_file()) {
                            csfile->get_file()->close();
                        }
                        csfile->set_file(NULL);
                    }
                    CSFREE(obj);
                    work_done += FREE_NODE_COST;
                } else if (obj->get_color() == GC_FREE) {
                    work_done += SWEEP_NODE_COST;
                } else if (obj->get_color() == GC_WHITE) {
                    obj->set_color(GC_BLACK);
                    work_done += COLOR_NODE_COST;
                }
                sweep_ptr += ((sz + 7) & ~7);  // round up to 8-byte alignment
                if (sweep_ptr >= sweep_end) {
                    sweep_chunk = sweep_chunk->next;
                }
            }
            break;
        case GC_SWEEP2:
            // in this phase, we take gray objects that were created during
            // GC_SWEEP and make them black
            while (work_done < WORK_PER_POLL && gray_list) {
                obj = gray_list;
                gray_list = gray_list->get_next();
                obj->set_color_2(GC_BLACK);
                work_done += COLOR_NODE_COST;
            }
            if (!gray_list) {
                gc_state = GC_FINAL;
            }
            break;
        case GC_FINAL:
            // set high water mark where gc starts again to 1.5 times current
            // amount of memory in use:
            gc_high_water = csallocated();
            gc_high_water += (gc_high_water >> 1);
            gc_state = GC_IDLE;
            return;  // exit loop; do not look for more work
        case GC_IDLE:
            if (csallocated() > gc_high_water) {
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
