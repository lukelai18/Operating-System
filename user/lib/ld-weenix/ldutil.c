/*
 *  File: ldutil.c
 *  Date: 15 March 1998
 *  Acct: David Powell (dep)
 *  Desc: Functions that didn't fit anywhere else
 *
 *
 *  Acct: Sandy Harvie (charvie)
 *  Date: 27 March 2019
 *  Desc: Modified for x86-64
 */

/* LINTLIBRARY */

#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "ldtypes.h"
#include "ldutil.h"

static const char *err_zero = "ld.so.1: panic - unable to open /dev/zero\n";

/* I wrote this back before I had printf... maybe it should disappear. */

void _ldverify(int test, const char *msg)
{
    if (test)
    {
        (void)write(STDERR_FILENO, msg, strlen(msg));
        exit(LD_ERR_EXIT);
    }
}

/* This function simply attempts to open /dev/zero, exiting if the call
 * to open failed.  The file descriptor of the newly opened file is
 * returned. */

int _ldzero()
{
    int zfd;

    if ((zfd = open("/dev/zero", O_RDONLY, 0)) < 0)
    {
        printf("%s", err_zero);
        exit(1);
    }

    return zfd;
}

/* This is the hash operation used for the string-to-symbol hash
 * table in dynamic ELF binaries.  This function is taken more or less
 * directly from the LLM */

unsigned long _ldelfhash(const char *name)
{
    uint32_t h = 0, g;

    while (*name)
    {
        h = (h << 4) + *name++;
        /* LINTED */
        if ((g = h & 0xf0000000))
            h ^= g >> 24;
        h &= ~g;
    }

    return h;
}
