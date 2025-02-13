#ifndef LUM_CONT_DA_H
#define LUM_CONT_DA_H

#define STB_DS_IMPLEMENTATION
#include <stddef.h>
#include "allocators/mem_alloc.h"

#define STBDS_REALLOC(ctx, ptr, size) lum_get_default_allocator()->realloc(lum_get_default_allocator(), ptr, size)
#define STBDS_FREE(ctx, ptr)          lum_get_default_allocator()->free(lum_get_default_allocator(), ptr)

#include "stb_ds.h"

// **Macro Wrappers for stb_ds**
#define cont_da_create()         NULL  // stb_ds manages this
#define cont_da_size(a)          stbds_arrlen(a)
#define cont_da_reserve(a, n)    stbds_arrsetlen(a, n)
#define cont_da_free(a)          stbds_arrfree(a)
#define cont_da_pop(a)           stbds_arrpop(a)

// **Custom Append Macro**
#define cont_da_app(a, v)        stbds_arrput(a, v)

#endif
