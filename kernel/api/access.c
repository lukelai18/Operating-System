#include "errno.h"
#include "globals.h"
#include <mm/mm.h>
#include <util/string.h>

#include "util/debug.h"

#include "mm/kmalloc.h"
#include "mm/mman.h"

#include "api/access.h"
#include "api/syscall.h"

static inline long userland_address(const void *addr)
{
    return addr >= (void *)USER_MEM_LOW && addr < (void *)USER_MEM_HIGH;
}

/*
 * Check for permissions on [uaddr, uaddr + nbytes), then
 * copy nbytes from userland address uaddr to kernel address kaddr.
 * Do not access the userland virtual addresses directly; instead,
 * use vmmap_read.
 */
long copy_from_user(void *kaddr, const void *uaddr, size_t nbytes)
{
    if (!range_perm(curproc, uaddr, nbytes, PROT_READ))
    {
        return -EFAULT;
    }
    KASSERT(userland_address(uaddr) && !userland_address(kaddr));
    return vmmap_read(curproc->p_vmmap, uaddr, kaddr, nbytes);
}

/*
 * Check for permissions on [uaddr, uaddr + nbytes), then
 * copy nbytes from kernel address kaddr to userland address uaddr.
 * Do not access the userland virtual addresses directly; instead,
 * use vmmap_write.
 */
long copy_to_user(void *uaddr, const void *kaddr, size_t nbytes)
{
    if (!range_perm(curproc, uaddr, nbytes, PROT_WRITE))
    {
        return -EFAULT;
    }
    KASSERT(userland_address(uaddr) && !userland_address(kaddr));
    return vmmap_write(curproc->p_vmmap, uaddr, kaddr, nbytes);
}

/*
 * Duplicate the string identified by ustr into kernel memory.
 * The kernel memory string kstr should be allocated using kmalloc.
 */
long user_strdup(argstr_t *ustr, char **kstrp)
{
    KASSERT(!userland_address(ustr));
    KASSERT(userland_address(ustr->as_str));

    *kstrp = kmalloc(ustr->as_len + 1);
    if (!*kstrp)
        return -ENOMEM;
    long ret = copy_from_user(*kstrp, ustr->as_str, ustr->as_len + 1);
    if (ret)
    {
        kfree(*kstrp);
        return ret;
    }
    return 0;
}

/*
 * Duplicate the string of vectors identified by uvec into kernel memory.
 * The vector itself (char**) and each string (char*) should be allocated
 * using kmalloc.
 */
long user_vecdup(argvec_t *uvec, char ***kvecp)
{
    KASSERT(!userland_address(uvec));
    KASSERT(userland_address(uvec->av_vec));

    char **kvec = kmalloc((uvec->av_len + 1) * sizeof(char *));
    *kvecp = kvec;

    if (!kvec)
    {
        return -ENOMEM;
    }
    memset(kvec, 0, (uvec->av_len + 1) * sizeof(char *));

    long ret = 0;
    for (size_t i = 0; i < uvec->av_len && !ret; i++)
    {
        argstr_t argstr;
        copy_from_user(&argstr, uvec->av_vec + i, sizeof(argstr_t));
        ret = user_strdup(&argstr, kvec + i);
    }

    if (ret)
    {
        for (size_t i = 0; i < uvec->av_len; i++)
            if (kvec[i])
                kfree(kvec[i]);
        kfree(kvec);
        *kvecp = NULL;
    }

    return ret;
}

/*
 * Return 1 if process p has permissions perm for virtual address vaddr;
 * otherwise return 0.
 * 
 * Check against the vmarea's protections on the mapping. 
 */
long addr_perm(proc_t *p, const void *vaddr, int perm)
{
    vmarea_t *cur_vma=vmmap_lookup(p->p_vmmap,ADDR_TO_PN(vaddr));
    if(cur_vma==NULL){  // Error checking
        return 0;
    }

    return ((cur_vma->vma_prot & perm) == perm);

    // // Check page protection flags, read, write and execute
    // if((perm&PROT_EXEC)&&(cur_vma->vma_prot&PROT_EXEC)){
    //     return 1;
    // }
    // if((perm&PROT_READ)&&(cur_vma->vma_prot&PROT_READ)){
    //     return 1;
    // }
    // if((perm&PROT_WRITE)&&(cur_vma->vma_prot&PROT_WRITE)){
    //     return 1;
    // }
    // // NOT_YET_IMPLEMENTED("VM: addr_perm");
    // return 0;
}

/*
 * Return 1 if process p has permissions perm for virtual address range [vaddr,
 * vaddr + len); otherwise return 0.
 * 
 * Hints: 
 * You can use addr_perm in your implementation.
 * Make sure to consider the case when the range of addresses that is being 
 * checked is less than a page. 
 */
long range_perm(proc_t *p, const void *vaddr, size_t len, int perm)
{
    // size_t cur_vaddr=(size_t)vaddr;
    // size_t end_vaddr=(size_t)vaddr+len;

    size_t start_pagenum=(size_t)ADDR_TO_PN((uintptr_t)vaddr);
    size_t end_pagenum=(size_t)ADDR_TO_PN((uintptr_t)vaddr+len-1);

    for(size_t i=start_pagenum;i<=end_pagenum;i++){
        if(!addr_perm(p,PN_TO_ADDR(i),perm)){
            return 0;
        }
    }

    return 1;
    // while(cur_vaddr<end_vaddr){
    //     if(!addr_perm(p,(void *)cur_vaddr,perm)){
    //         return 0;
    //     }
    //     // Update current virtual address
    //     // TODO: Need to come back，not sure how to consider the case when the rest range is less than 1 PAGE_SIZE
    //     if(end_vaddr-cur_vaddr<PAGE_SIZE){  // If the rest of searching range is less than 1 SIZE
    //         cur_vaddr=end_vaddr;
    //     }else{
    //         cur_vaddr=cur_vaddr+PAGE_SIZE;  
    //     }
    // }

    // NOT_YET_IMPLEMENTED("VM: range_perm")
}
