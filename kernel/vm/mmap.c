#include "vm/mmap.h"
#include "errno.h"
#include "fs/file.h"
#include "fs/vfs.h"
#include "fs/vnode.h"
#include "globals.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/tlb.h"
#include "util/debug.h"

/*
 * This function implements the mmap(2) syscall: Add a mapping to the current
 * process's address space. Supports the following flags: MAP_SHARED,
 * MAP_PRIVATE, MAP_FIXED, and MAP_ANON.
 *
 *  ret - If provided, on success, *ret must point to the start of the mapped area
 *
 * Return 0 on success, or:
 *  - EACCES: 
 *     - A file descriptor refers to a non-regular file.  
 *     - a file mapping was requested, but fd is not open for reading. 
 *     - MAP_SHARED was requested and PROT_WRITE is set, but fd is
 *       not open in read/write (O_RDWR) mode.
 *     - PROT_WRITE is set, but the file has FMODE_APPEND specified.
 *  - EBADF:
 *     - fd is not a valid file descriptor and MAP_ANON was
 *       not set
 *  - EINVAL:
 *     - addr is not page aligned and MAP_FIXED is specified 
 *     - off is not page aligned 
 *     - len is <= 0 or off < 0 
 *     - flags do not contain MAP_PRIVATE or MAP_SHARED
 *  - ENODEV:
 *     - The underlying filesystem of the specified file does not
 *       support memory mapping or in other words, the file's vnode's mmap
 *       operation doesn't exist
 *  - Propagate errors from vmmap_map()
 * 
 *  See the man page for mmap(2) errors section for more details
 * 
 * Hints:
 *  1) A lot of error checking.
 *  2) Call vmmap_map() to create the mapping.
 *     a) Use VMMAP_DIR_HILO as default, which will make other stencil code in
 *        Weenix happy.
 *  3) Call tlb_flush_range() on the newly-mapped region. This is because the
 *     newly-mapped region could have been used by someone else, and you don't
 *     want to get stale mappings.
 *  4) Don't forget to set ret if it was provided.
 * 
 *  If you are mapping less than a page, make sure that you are still allocating 
 *  a full page. 
 * We wanted to clarify a note in the stencil for do_mmap.  
 * In do_mmap, in addition to the errors specified in the stencil comments, you also
 * need to handle the case where MAP_FIXED is specified, and the address is out of range (EINVAL). 
 * Refer to the man pages for confusion with error handling.
 * You do not need to handle the case for -EACESS when the file is non-regular.
 */
long do_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off,
             void **ret)
{
    // if(!PAGE_ALIGNED(addr)&&flags&MAP_FIXED){
    //     return -EINVAL;
    // }
    // if(!PAGE_ALIGNED(off)){
    //     return -EINVAL;
    // }
    // if(len<=0||off<0){
    //     return -EINVAL;
    // }
    // if(!(flags&MAP_PRIVATE)&&!(flags&MAP_SHARED)){
    //     return -EINVAL;
    // }
    // // Avoid len too long or addr is greater than USER_MEM_HIGH
    // if((flags&MAP_FIXED)&&((len>USER_MEM_HIGH-(size_t)addr)||(size_t)addr>USER_MEM_HIGH)){
    //     return -EINVAL;
    // }
    // if(!(flags&MAP_ANON)&&(fd<0||fd>=NFILES)){
    //     return -EBADF;
    // }
    // // Mmap operation doesn't exist
    // if(curproc->p_files[fd]->f_vnode->vn_ops->mmap==NULL){
    //     return -ENODEV;
    // }
    // if((prot&PROT_WRITE)&&(curproc->p_files[fd]->f_mode&FMODE_APPEND)){
    //     return -EACCES;
    // }
    // if((prot&PROT_WRITE)&&(flags&MAP_SHARED)&&!(curproc->p_files[fd]->f_mode&(FMODE_READ|FMODE_WRITE))){
    //     return -EACCES;
    // }
    // // Maping was request
    // // TODO: I think maping flag is checked previously, so we don't need to check it again
    // if(!curproc->p_files[fd]->f_mode&FMODE_READ){
    //     return -EACCES;
    // }
    // vmarea_t *new_vma;
    // // Using page align up can handle the case when len is less than 1 page
    // long tmp=vmmap_map(curproc->p_vmmap,curproc->p_files[fd]->f_vnode,ADDR_TO_PN(addr),
    //     (size_t)PAGE_ALIGN_UP(len)/PAGE_SIZE,prot,flags,off,VMMAP_DIR_HILO,&new_vma);
    // if(tmp<0){
    //     return tmp;
    // }

    // tlb_flush_range((uintptr_t)PN_TO_ADDR(new_vma->vma_start),
    //     (size_t)PAGE_ALIGN_UP(len)/PAGE_SIZE);
    // *ret=PN_TO_ADDR(new_vma->vma_start);

    // // NOT_YET_IMPLEMENTED("VM: do_mmap");
    return 0;
}

/*
 * This function implements the munmap(2) syscall.
 *
 * Return 0 on success, or:
 *  - EINVAL:
 *     - addr is not aligned on a page boundary
 *     - the region to unmap is out of range of the user address space
 *     - len is 0
 *  - Propagate errors from vmmap_remove()
 * 
 *  See the man page for munmap(2) errors section for more details
 *
 * Hints:
 *  - Similar to do_mmap():
 *  1) Perform error checking.
 *  2) Call vmmap_remove().
 */
long do_munmap(void *addr, size_t len)
{
    // if(len==0){
    //     return -EINVAL;
    // }
    // if(!PAGE_ALIGNED(addr)){
    //     return -EINVAL;
    // }
    // if((size_t)addr<USER_MEM_LOW||(size_t)addr+len>USER_MEM_HIGH){
    //     return -EINVAL;
    // }

    // long tmp=vmmap_remove(curproc->p_vmmap,ADDR_TO_PN(addr),(size_t)PAGE_ALIGN_UP(len)/PAGE_SIZE);

    // // NOT_YET_IMPLEMENTED("VM: do_munmap");
    // return tmp;
    return 0;
}