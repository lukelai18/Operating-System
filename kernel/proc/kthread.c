// SMP.1 for non-curthr actions; none for curthr
#include "config.h"
#include "globals.h"
#include "mm/slab.h"
#include "util/debug.h"
#include "util/string.h"

/*==========
 * Variables
 *=========*/

/*
 * Global variable maintaining the current thread on the cpu
 */
kthread_t *curthr CORE_SPECIFIC_DATA;

/*
 * Private slab for kthread structs
 */
static slab_allocator_t *kthread_allocator = NULL;

/*=================
 * Helper functions
 *================*/

/*
 * Allocates a new kernel stack. Returns null when not enough memory.
 */
static char *alloc_stack() { return page_alloc_n(DEFAULT_STACK_SIZE_PAGES); }

/*
 * Frees an existing kernel stack.
 */
static void free_stack(char *stack)
{
    page_free_n(stack, DEFAULT_STACK_SIZE_PAGES);
}

/*==========
 * Functions
 *=========*/

/*
 * Initializes the kthread_allocator.
 */
void kthread_init()
{
    KASSERT(__builtin_popcount(DEFAULT_STACK_SIZE_PAGES) == 1 &&
            "stack size should me a power of 2 pages to reduce fragmentation");
    kthread_allocator = slab_allocator_create("kthread", sizeof(kthread_t));
    KASSERT(kthread_allocator);
}

/*
 * Creates and initializes a thread.
 * Returns a new kthread, or null on failure.
 *
 * Hints:
 * Use kthread_allocator to allocate a kthread
 * Use alloc_stack() to allocate a kernel stack
 * Use context_setup() to set up the thread's context - 
 *  also use DEFAULT_STACK_SIZE and the process's pagetable (p_pml4)
 * Remember to initialize all the thread's fields
 * Remember to add the thread to proc's threads list
 * Initialize the thread's kt_state to KT_NO_STATE
 * Initialize the thread's kt_recent_core to ~0UL (unsigned -1)
 */
kthread_t *kthread_create(proc_t *proc, kthread_func_t func, long arg1,
                          void *arg2)
{
    NOT_YET_IMPLEMENTED("PROCS: kthread_create");
    return NULL;
}

/*
 * Creates and initializes a thread that is a clone of thr.
 * Returns a new kthread, or null on failure.
 * 
 * P.S. Note that you do not need to implement this function until VM.
 *
 * Hints:
 * The only parts of the context that must be initialized are c_kstack and
 * c_kstacksz. The thread's process should be set outside of this function. Copy
 * over thr's retval, errno, and cancelled; other fields should be freshly
 * initialized. Remember to protect access to thr via its spinlock. See
 * kthread_create() for more hints.
 */
kthread_t *kthread_clone(kthread_t *thr)
{
    NOT_YET_IMPLEMENTED("VM: kthread_clone");
    return NULL;
}

/*
 * Free the thread's stack, remove it from its process's list of threads, and
 * free the kthread_t struct itself. Protect access to the kthread using its
 * kt_lock.
 *
 * You cannot destroy curthr.
 */
void kthread_destroy(kthread_t *thr)
{
    spinlock_lock(&thr->kt_lock);
    KASSERT(thr != curthr);
    KASSERT(thr && thr->kt_kstack);
    if (thr->kt_state != KT_EXITED)
        panic("destroying thread in state %d\n", thr->kt_state);
    free_stack(thr->kt_kstack);
    if (list_link_is_linked(&thr->kt_plink))
        list_remove(&thr->kt_plink);

    spinlock_unlock(&thr->kt_lock);
    slab_obj_free(kthread_allocator, thr);
}

/*
 * Sets the thread's return value and cancels the thread.
 *
 * Note: Check out the use of check_curthr_cancelled() in syscall_handler()
 * to see how a thread eventually notices it is cancelled and handles exiting
 * itself.
 *
 * Hints:
 * This should not be called on curthr.
 * Use sched_cancel() to actually mark the thread as cancelled.
 */
void kthread_cancel(kthread_t *thr, void *retval)
{
    NOT_YET_IMPLEMENTED("PROCS: kthread_cancel");
}

/*
 * Wrapper around proc_thread_exiting().
 */
void kthread_exit(void *retval)
{
    NOT_YET_IMPLEMENTED("PROCS: kthread_exit");
}
