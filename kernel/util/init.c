#include "kernel.h"

#include "mm/kmalloc.h"

#include "util/debug.h"
#include "util/init.h"
#include "util/list.h"
#include "util/string.h"

static int _init_search_count = 0;

struct init_function
{
    init_func_t if_func;
    const char *if_name;
    list_link_t if_link;

    int if_search;
    int if_called;
    list_t if_deps;
};

struct init_depends
{
    const char *id_name;
    list_link_t id_link;
};

static void _init_call(list_t *funcs, struct init_function *func)
{
    list_iterate(&func->if_deps, dep, struct init_depends, id_link)
    {
        struct init_function *found = NULL;
        list_iterate(funcs, f, struct init_function, if_link)
        {
            if (strcmp(dep->id_name, f->if_name) == 0)
            {
                found = f;
                break;
            }
        }

        if (!found)
        {
            panic("'%s' dependency for '%s' does not exist", dep->id_name,
                  func->if_name);
        }

        if (func->if_search == found->if_search)
        {
            panic("circular dependency between '%s' and '%s'", func->if_name,
                  found->if_name);
        }

        dbg(DBG_INIT, "'%s' depends on '%s': ", func->if_name, found->if_name);
        if (!found->if_called)
        {
            dbgq(DBG_INIT, "calling\n");
            found->if_search = func->if_search;
            _init_call(funcs, found);
        }
        else
        {
            dbgq(DBG_INIT, "already called\n");
        }
    }

    KASSERT(!func->if_called);

    dbg(DBG_INIT, "Calling %s (0x%p)\n", func->if_name, func->if_func);
    func->if_func();
    func->if_called = 1;
}

void init_call_all()
{
    list_t funcs;
    char *buf, *end;

    list_init(&funcs);
    buf = (char *)&kernel_start_init;
    end = (char *)&kernel_end_init;

    while (buf < end)
    {
        struct init_function *curr = kmalloc(sizeof(*curr));
        KASSERT(NULL != curr);

        list_insert_tail(&funcs, &curr->if_link);
        list_init(&curr->if_deps);

        KASSERT(NULL != *(uintptr_t *)buf);
        curr->if_func = (init_func_t) * (uintptr_t *)buf;
        curr->if_name = buf + sizeof(curr->if_func);
        curr->if_search = 0;
        curr->if_called = 0;

        buf += sizeof(curr->if_func) + strlen(curr->if_name) + 1;

        while ((NULL == *(uintptr_t *)buf) && (buf < end))
        {
            struct init_depends *dep = kmalloc(sizeof(*dep));
            KASSERT(NULL != dep);

            list_insert_tail(&curr->if_deps, &dep->id_link);

            dep->id_name = buf + sizeof(curr->if_func);
            buf += sizeof(curr->if_func) + strlen(dep->id_name) + 1;
        }
    }

    KASSERT(buf == end);

    dbg(DBG_INIT, "Initialization functions and dependencies:\n");
    list_iterate(&funcs, func, struct init_function, if_link)
    {
        dbgq(DBG_INIT, "%s (0x%p): ", func->if_name, func->if_func);
        list_iterate(&func->if_deps, dep, struct init_depends, id_link)
        {
            dbgq(DBG_INIT, "%s ", dep->id_name);
        }
        dbgq(DBG_INIT, "\n");
    }

    list_iterate(&funcs, func, struct init_function, if_link)
    {
        if (!func->if_called)
        {
            func->if_search = ++_init_search_count;
            _init_call(&funcs, func);
        }
    }

    list_iterate(&funcs, func, struct init_function, if_link)
    {
        list_iterate(&func->if_deps, dep, struct init_depends, id_link)
        {
            kfree(dep);
        }
        kfree(func);
    }
}
