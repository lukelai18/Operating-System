#include "vm/shadow.h"
#include "mm/page.h"
#include "mm/pframe.h"
#include "mm/slab.h"
#include "util/debug.h"
#include "util/string.h"

#define SHADOW_SINGLETON_THRESHOLD 5

typedef struct mobj_shadow
{
    // the mobj parts of this shadow object
    mobj_t mobj;
    // a reference to the mobj that is the data source for this shadow object
    // This should be a reference to a shadow object of some ancestor process.
    // This is used to traverse the shadow object chain.
    mobj_t *shadowed;
    // a reference to the mobj at the bottom of this shadow object's chain
    // this should NEVER be a shadow object (i.e. it should have some type other
    // than MOBJ_SHADOW)
    mobj_t *bottom_mobj;
} mobj_shadow_t;

#define MOBJ_TO_SO(o) CONTAINER_OF(o, mobj_shadow_t, mobj)

static slab_allocator_t *shadow_allocator;

static long shadow_get_pframe(mobj_t *o, size_t pagenum, long forwrite,
                              pframe_t **pfp);
static long shadow_fill_pframe(mobj_t *o, pframe_t *pf);
static long shadow_flush_pframe(mobj_t *o, pframe_t *pf);
static void shadow_destructor(mobj_t *o);

static mobj_ops_t shadow_mobj_ops = {.get_pframe = shadow_get_pframe,
                                     .fill_pframe = shadow_fill_pframe,
                                     .flush_pframe = shadow_flush_pframe,
                                     .destructor = shadow_destructor};

/*
 * Initialize shadow_allocator using the slab allocator.
 */
void shadow_init()
{
    // shadow_allocator=slab_allocator_create("shadow",sizeof(mobj_shadow_t));
    // KASSERT(shadow_allocator);
    // NOT_YET_IMPLEMENTED("VM: shadow_init");
}

/*
 * Create a shadow object that shadows the given mobj.
 *
 * Return a new, LOCKED shadow object on success, or NULL upon failure.
 *
 * Hints:
 *  1) Create and initialize a mobj_shadow_t based on the given mobj.
 *  2) Set up the bottom object of the shadow chain, which could have two cases:
 *     a) Either shadowed is a shadow object, and you can use its bottom_mobj
 *     b) Or shadowed is not a shadow object, in which case it is the bottom 
 *        object of this chain.
 * 
 *  Make sure to manage the refcounts correctly.
 */
mobj_t *shadow_create(mobj_t *shadowed)
{
    // mobj_shadow_t *new_sha=slab_obj_alloc(shadow_allocator);
    // if(new_sha==NULL){
    //     return NULL;
    // }

    // mobj_init(&new_sha->mobj,MOBJ_SHADOW,&shadow_mobj_ops);     // Initialize mobj field
    
    // // Initialize the shadowed and buttom mobj in mobj_shadow_t
    // new_sha->shadowed=shadowed;
    // if(shadowed->mo_type==MOBJ_SHADOW){ // If the shadowed is a shadow object
    //     new_sha->bottom_mobj=MOBJ_TO_SO(shadowed)->bottom_mobj;
    // }else{              // If the shadowed is not a shadowed object
    //     new_sha->bottom_mobj=shadowed;
    // }
    // new_sha->mobj.mo_refcount+=2;   // Have the refcount to bottom_mobj and shadowed
    // mobj_lock(&new_sha->mobj);
    // // NOT_YET_IMPLEMENTED("VM: shadow_create");
    // return &new_sha->mobj;
    return NULL;
}

/*
 * Given a shadow object o, collapse its shadow chain as far as you can.
 *
 * Hints:
 *  1) You can only collapse if the shadowed object is a shadow object.
 *  2) When collapsing, you must manually migrate pframes from o's shadowed
 *     object to o, checking to see if a copy doesn't already exist in o.
 *  3) Be careful with refcounting! In particular, when you put away o's
 *     shadowed object, its refcount should drop to 0, initiating its
 *     destruction (shadow_destructor).
 *  4) As a reminder, any refcounting done in shadow_collapse() must play nice
 *     with any refcounting done in shadow_destructor().
 *  5) Pay attention to mobj and pframe locking.
 */
void shadow_collapse(mobj_t *o)
{
    mobj_shadow_t *sha_o=MOBJ_TO_SO(o);
    while(sha_o->shadowed->mo_type==MOBJ_SHADOW){
       if(sha_o->shadowed->mo_refcount==1){
            list_iterate(&sha_o->shadowed->mo_pframes,cur_pf,pframe_t,pf_link){
                pframe_t *pf;
                mobj_find_pframe(sha_o->shadowed,cur_pf->pf_pagenum,&pf);
                if(pf==NULL){
                    list_remove(&pf->pf_link);
                    list_insert_tail(&o->mo_pframes,&cur_pf->pf_link);
                    pframe_release(&pf);
                }
                else{
                    pframe_release(&pf);
                    mobj_free_pframe(o,&pf);
                }
            }
        }
        o=MOBJ_TO_SO(o)->shadowed;
        if(o->mo_type==MOBJ_SHADOW){
            sha_o=MOBJ_TO_SO(o);
        } else{
            break;
        }
    }
    NOT_YET_IMPLEMENTED("VM: shadow_collapse");
}

/*
 * Obtain the desired pframe from the given mobj, traversing its shadow chain if
 * necessary. This is where copy-on-write logic happens!
 *
 * Arguments: 
 *  o        - The object from which to obtain a pframe
 *  pagenum  - Number of the desired page relative to the object
 *  forwrite - Set if the caller wants to write to the pframe's data, clear if
 *             only reading
 *  pfp      - Upon success, pfp should point to the desired pframe.
 *
 * Return 0 on success, or:
 *  - Propagate errors from mobj_default_get_pframe() and mobj_get_pframe()
 *
 * Hints:
 *  1) If forwrite is set, use mobj_default_get_pframe().
 *  2) If forwrite is clear, check if o already contains the desired frame.
 *     a) If not, iterate through the shadow chain to find the nearest shadow
 *        mobj that has the frame. Do not recurse! If the shadow chain is long,
 *        you will cause a kernel buffer overflow (e.g. from forkbomb).
 *     b) If no shadow objects have the page, call mobj_get_pframe() to get the
 *        page from the bottom object and return what it returns.
 * 
 *  Pay attention to pframe locking.
 */
static long shadow_get_pframe(mobj_t *o, size_t pagenum, long forwrite,
                              pframe_t **pfp)
{
    // // If forwrite is set
    // if(forwrite){
    //     long tmp=mobj_default_get_pframe(o,pagenum,forwrite,pfp);
    //     return tmp;
    // } else{     
    //     // If forwrite is clear
    //     // Check if o already contains the desired frame
    //     mobj_find_pframe(o,pagenum,pfp);
    //     if(*pfp!=NULL){
    //         kmutex_unlock(&(*pfp)->pf_mutex);
    //         return 0;
    //     }
    //     // If not in the mobj o, check if "o" is the shadow object, then iterate through shadow mobj
    //     if(o->mo_type==MOBJ_SHADOW){
    //         o=MOBJ_TO_SO(o)->shadowed;
    //         while(o->mo_type==MOBJ_SHADOW){
    //             list_iterate(&o->mo_pframes,pf,pframe_t,pf_link){
    //             // Check each shadowed object's page frame
    //                 mobj_find_pframe(o,pagenum,pfp);
    //                 if(*pfp!=NULL){
    //                     kmutex_unlock(&(*pfp)->pf_mutex);
    //                     return 0;
    //                 }
    //                 o=MOBJ_TO_SO(o)->shadowed;
    //             }
    //         }
    //     }     
    //     // If we still cannot find 
    //     long tmp=mobj_get_pframe(o,pagenum,forwrite,pfp);
    //     if(tmp<0){
    //         kmutex_unlock(&(*pfp)->pf_mutex);
    //         return tmp;
    //     }
    // }
    // // NOT_YET_IMPLEMENTED("VM: shadow_get_pframe");
    // return 0;
    return -1;
}

/*
 * Use the given mobj's shadow chain to fill the given pframe.
 *
 * Return 0 on success, or:
 *  - Propagate errors from mobj_get_pframe()
 *
 * Hints:
 *  1) Explore mobj_default_get_pframe(), which calls mobj_create_pframe(), to
 *     understand what state pf is in when this function is called, and how you
 *     can use it.
 *  2) As you can see above, shadow_get_pframe would call
 *     mobj_default_get_pframe (when the forwrite is set), which would 
 *     create and then fill the pframe (shadow_fill_pframe is called).
 *  3) Traverse the shadow chain for a copy of the frame, starting at the given
 *     mobj's shadowed object. You can use mobj_find_pframe to look for the 
 *     page frame. pay attention to locking/unlocking, and be sure not to 
 *     recurse when traversing.
 *  4) If none of the shadow objects have a copy of the frame, use
 *     mobj_get_pframe on the bottom object to get it.
 *  5) After obtaining the desired frame, simply copy its contents into pf.
 */
static long shadow_fill_pframe(mobj_t *o, pframe_t *pf)
{
    // // KASSERT(o->mo_type==MOBJ_SHADOW&&"Make sure it is shadow object");
    // mobj_t *cur_o=MOBJ_TO_SO(o)->shadowed;  // The first mobj we need to iterate
    // size_t request_pagenum=pf->pf_pagenum;  // Requested page number
    // pframe_t *cur_pf;       // The finded pframe
    // while(cur_o->mo_type==MOBJ_SHADOW){
    //     mobj_lock(cur_o);   // Lock current mobj
    //     mobj_find_pframe(cur_o,request_pagenum,&cur_pf);
    //     // If we found the page frame
    //     if(cur_pf!=NULL){
    //         memset(pf->pf_addr,cur_pf->pf_addr,PAGE_SIZE);  // Copy its content into pf
    //         kmutex_unlock(&cur_pf->pf_mutex);
    //         mobj_unlock(cur_o);
    //         return 0;
    //     }
    //     mobj_unlock(cur_o);

    //     cur_o=MOBJ_TO_SO(cur_o)->shadowed;  // Update current mobj
    // }
    // // If none of the shadow object have a copy of the pframe, create a new one
    // long tmp=mobj_get_pframe(o,request_pagenum,1,&cur_pf);
    // if(tmp<0){
    //     kmutex_unlock(&cur_pf->pf_mutex);
    //     return tmp;
    // }
    // memset(pf->pf_addr,cur_pf->pf_dirty,PAGE_SIZE);
    // kmutex_unlock(&cur_pf->pf_mutex);
    // NOT_YET_IMPLEMENTED("VM: shadow_fill_pframe");
    return 0;
}

/*
 * Flush a shadow object's pframe to disk.
 *
 * Return 0 on success.
 *
 * Hint:
 *  - Are shadow objects backed to disk? Do you actually need to do anything
 *    here?
 */
static long shadow_flush_pframe(mobj_t *o, pframe_t *pf)
{
    // NOT_YET_IMPLEMENTED("VM: shadow_flush_pframe");
    return 0;
}

/*
 * Clean up all resources associated with mobj o.
 *
 * Hints:
 *  - Check out mobj_put() to understand how this function gets called.
 *
 *  1) Call mobj_default_destructor() to flush o's pframes.
 *  2) Put the shadow and bottom_mobj members of the shadow object.
 *  3) Free the mobj_shadow_t.
 */
static void shadow_destructor(mobj_t *o)
{
    // mobj_default_destructor(o);
    // mobj_put(&MOBJ_TO_SO(o)->shadowed);
    // mobj_put(&MOBJ_TO_SO(o)->bottom_mobj);
    // slab_obj_free(shadow_allocator,o);
    NOT_YET_IMPLEMENTED("VM: shadow_destructor");
}
