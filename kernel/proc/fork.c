#include "errno.h"
#include "globals.h"
#include "types.h"

#include "util/debug.h"
#include "util/string.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/pframe.h"
#include "mm/tlb.h"

#include "fs/vnode.h"

#include "vm/shadow.h"

#include "api/exec.h"

/* Pushes the appropriate things onto the kernel stack of a newly forked thread
 * so that it can begin execution in userland_entry.
 * regs: registers the new thread should have on execution
 * kstack: location of the new thread's kernel stack
 * Returns the new stack pointer on success. */
static uintptr_t fork_setup_stack(const regs_t *regs, void *kstack)
{
    /* Pointer argument and dummy return address, and userland dummy return
     * address */
    uint64_t rsp =
        ((uint64_t)kstack) + DEFAULT_STACK_SIZE - (sizeof(regs_t) + 16);
    memcpy((void *)(rsp + 8), regs, sizeof(regs_t)); /* Copy over struct */
    return rsp;
}

/*
 * This function implements the fork(2) system call.
 *
 * TODO:
 * 1) Use proc_create() and kthread_clone() to set up a new process and thread. If
 *    either fails, perform any appropriate cleanup.
 * 2) Finish any initialization work for the new process and thread.
 * 3) Fix the values of the registers and the rest of the kthread's ctx. 
 *    Some registers can be accessed from the cloned kthread's context (see the context_t 
 *    and kthread_t structs for more details):
 *    a) We want the child process to also enter userland execution. 
 *       For this, the instruction pointer should point to userland_entry (see exec.c).
 *    b) Remember that the only difference between the parent and child processes 
 *       is the return value of fork(). This value is returned in the RAX register, 
 *       and the return value should be 0 for the child. The parent's return value would 
 *       be the process id of the newly created child process. 
 *    c) Before the process begins execution in userland_entry, 
 *       we need to push all registers onto the kernel stack of the kthread. 
 *       Use fork_setup_stack to do this, and set RSP accordingly. 
 *    d) Use pt_unmap_range and tlb_flush_all on the parent in advance of
 *       copy-on-write.
 * 5) Prepare the child process to be run on the CPU.
 * 6) Return the child's process id to the parent.
 */
long do_fork(struct regs *regs)
{
    proc_t* child_proc=proc_create("new_process");

    if(child_proc==NULL){
        curthr->kt_errno=ENOMEM;
        return -1;
    }
  
    kthread_t *new_thr=kthread_clone(curthr);
    if(new_thr==NULL){
        proc_destroy(child_proc);    
        curthr->kt_errno=ENOMEM;
        return -1;
    }
    list_insert_tail(&child_proc->p_threads,&new_thr->kt_plink);
    new_thr->kt_proc=child_proc;

    regs->r_rax=0;  // Set return value to 0 before copying to child process's stack 

    // Instruction pointer should point to userland_entry
    // new_thr->kt_ctx.c_kstack=(uintptr_t)new_thr->kt_kstack;
    // new_thr->kt_ctx.c_kstacksz=DEFAULT_STACK_SIZE;
    new_thr->kt_ctx.c_rip=(uintptr_t)userland_entry;

    new_thr->kt_ctx.c_pml4=child_proc->p_pml4;

    new_thr->kt_ctx.c_rsp=fork_setup_stack(regs,new_thr->kt_kstack);
    // regs->r_rax=child_proc->p_pid;  // Set the regs return value to child process's return ID

    // Use them at parent
    pt_unmap_range(curproc->p_pml4,USER_MEM_LOW,USER_MEM_HIGH);
    tlb_flush_all();

    sched_make_runnable(new_thr);   // Make the child process's thread runable

    // NOT_YET_IMPLEMENTED("VM: do_fork");
    return child_proc->p_pid;
    // return -1;
}
