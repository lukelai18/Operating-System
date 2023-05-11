#include <boot/config.h>
#include <boot/multiboot_macros.h>
#include <drivers/screen.h>
#include <multiboot.h>
#include <types.h>
#include <util/debug.h>
#include <util/string.h>

#ifdef __VGABUF___

#define BITMAP_HEIGHT 13

// https://stackoverflow.com/questions/2156572/c-header-file-with-bitmapped-fonts
unsigned const char bitmap_letters[95][BITMAP_HEIGHT] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00}, // space :32
    {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
     0x18}, // ! :33
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36,
     0x36},
    {0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00,
     0x00},
    {0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e,
     0x18},
    {0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8,
     0x70},
    {0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c,
     0x38},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c,
     0x0e},
    {0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18,
     0x0c},
    {0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18,
     0x30},
    {0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00,
     0x00},
    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00,
     0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03,
     0x03},
    {0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66,
     0x3c},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38,
     0x18},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7,
     0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7,
     0x7e},
    {0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c,
     0x0c},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0,
     0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7,
     0x7e},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03,
     0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7,
     0x7e},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7,
     0x7e},
    {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c,
     0x06},
    {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30,
     0x60},
    {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3,
     0x7e},
    {0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,
     0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7,
     0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7,
     0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce,
     0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0,
     0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0,
     0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7,
     0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3,
     0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
     0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
     0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6,
     0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
     0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7,
     0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3,
     0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7,
     0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7,
     0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66,
     0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7,
     0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7,
     0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
     0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
     0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
     0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3,
     0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66,
     0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66,
     0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03,
     0xff},
    {0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
     0x3c},
    {0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60,
     0x60},
    {0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
     0x3c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c,
     0x18},
    {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30,
     0x70},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0,
     0xc0},
    {0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03,
     0x03},
    {0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33,
     0x1e},
    {0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0,
     0xc0},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18,
     0x00},
    {0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c,
     0x00},
    {0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0,
     0xc0},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
     0x78},
    {0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00,
     0x00},
    {0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00,
     0x00},
    {0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30,
     0x00},
    {0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00,
     0x00},
    {0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00,
     0x00},
    {0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18,
     0x0f},
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
     0x18},
    {0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18,
     0xf0},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00,
     0x00},
};

#define DOUBLE_BUFFERING 0

#define BITWISE_TERNARY(condition, x, y) \
    (!!(condition) * (x) + !(condition) * (y))

static uint32_t *fb;
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;

static uint32_t *fb_buffer;

void screen_init()
{
    static long inited = 0;
    if (inited)
        return;
    inited = 1;

    struct multiboot_tag_framebuffer *fb_tag = NULL;
    for (struct multiboot_tag *tag =
             (struct multiboot_tag *)((uintptr_t)(mb_tag + 1) + PHYS_OFFSET);
         tag->type != MULTIBOOT_TAG_TYPE_END; tag += TAG_SIZE(tag->size))
    {
        if (tag->type != MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            continue;
        }
        fb_tag = (struct multiboot_tag_framebuffer *)tag;
        break;
    }
    KASSERT(fb_tag);

    fb = (uint32_t *)(PHYS_OFFSET + fb_tag->common.framebuffer_addr);
    fb_width = fb_tag->common.framebuffer_width;
    fb_height = fb_tag->common.framebuffer_height;
    fb_pitch = fb_tag->common.framebuffer_pitch;
    KASSERT(fb_pitch == fb_width * sizeof(uint32_t));
    KASSERT(fb_tag->common.framebuffer_bpp == 32);
    KASSERT(fb_tag->common.framebuffer_type == 1);
    KASSERT(fb_tag->framebuffer_red_field_position == 0x10);
    KASSERT(fb_tag->framebuffer_green_field_position == 0x08);
    KASSERT(fb_tag->framebuffer_blue_field_position == 0x00);
    KASSERT(fb_tag->framebuffer_red_mask_size);
    KASSERT(fb_tag->framebuffer_green_mask_size == 8);
    KASSERT(fb_tag->framebuffer_blue_mask_size == 8);

    size_t npages = 0;
    for (uintptr_t page = (uintptr_t)PAGE_ALIGN_DOWN(fb);
         page < (uintptr_t)PAGE_ALIGN_UP(fb + fb_width * fb_height);
         page += PAGE_SIZE)
    {
        page_mark_reserved((void *)(page - PHYS_OFFSET));
        npages++;
    }

    struct multiboot_tag_vbe *vbe_info = NULL;
    for (struct multiboot_tag *tag =
             (struct multiboot_tag *)((uintptr_t)(mb_tag + 1) + PHYS_OFFSET);
         tag->type != MULTIBOOT_TAG_TYPE_END; tag += TAG_SIZE(tag->size))
    {
        if (tag->type != MULTIBOOT_TAG_TYPE_VBE)
        {
            continue;
        }
        vbe_info = (struct multiboot_tag_vbe *)tag;
        break;
    }
    KASSERT(vbe_info);

#if DOUBLE_BUFFERING
    fb_buffer = page_alloc_n(npages);
    KASSERT(fb_buffer && "couldn't allocate double buffer for screen");
#else
    fb_buffer = fb;
#endif
    pt_map_range(pt_get(), (uintptr_t)fb - PHYS_OFFSET, (uintptr_t)fb,
                 (uintptr_t)PAGE_ALIGN_UP(fb + fb_width * fb_height),
                 PT_PRESENT | PT_WRITE, PT_PRESENT | PT_WRITE);
    pt_set(pt_get());
    for (uint32_t i = 0; i < fb_width * fb_height; i++)
        fb_buffer[i] = 0x008A2BE2;
    screen_flush();
}

inline size_t screen_get_width() { return fb_width; }

inline size_t screen_get_height() { return fb_height; }

inline size_t screen_get_character_width() { return SCREEN_CHARACTER_WIDTH; }

inline size_t screen_get_character_height() { return SCREEN_CHARACTER_HEIGHT; }

inline void screen_draw_string(size_t x, size_t y, const char *s, size_t len,
                               color_t color)
{
    uint32_t *pos = fb_buffer + y * fb_width + x;
    while (len--)
    {
        const char c = *s++;
        if (c < ' ' || c > '~')
            continue;
        const unsigned char *bitmap = bitmap_letters[c - ' '];

        size_t bm_row = BITMAP_HEIGHT;
        while (bm_row--)
        {
            unsigned char cols = bitmap[bm_row];
            *pos = BITWISE_TERNARY(cols & 0x80, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x40, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x20, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x10, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x08, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x04, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x02, color.value, *pos);
            pos++;
            *pos = BITWISE_TERNARY(cols & 0x01, color.value, *pos);
            pos++;
            pos += fb_width - 8;
        }
        pos = pos - fb_width * BITMAP_HEIGHT + SCREEN_CHARACTER_WIDTH;
    }
}

inline void screen_draw_horizontal(uint32_t *pos, size_t count, color_t color)
{
    //    while(count--) *pos++ = color.value;
    __asm__ volatile("cld; rep stosl;" ::"a"(color.value), "D"(pos), "c"(count)
                     : "cc");
}

inline void screen_copy_horizontal(uint32_t *from, uint32_t *to, size_t count)
{
    __asm__ volatile("cld; rep movsl;" ::"S"(from), "D"(to), "c"(count)
                     : "cc");
}

inline void screen_draw_rect(size_t x, size_t y, size_t width, size_t height,
                             color_t color)
{
    uint32_t *top = fb_buffer + y * fb_width + x;
    screen_draw_horizontal(top, width, color);
    screen_draw_horizontal(top + height * fb_width, width, color);
    while (height--)
    {
        *top = *(top + width) = color.value;
        top += fb_width;
    }
}

inline void screen_fill(color_t color)
{
    __asm__ volatile("cld; rep stosl;" ::"a"(color.value), "D"(fb_buffer),
                     "c"(fb_width * fb_height)
                     : "cc");
}

inline void screen_fill_rect(size_t x, size_t y, size_t width, size_t height,
                             color_t color)
{
    uint32_t *top = fb_buffer + y * fb_width + x;
    while (height--)
    {
        screen_draw_horizontal(top, width, color);
        top += fb_width;
    }
}

inline void screen_copy_rect(size_t fromx, size_t fromy, size_t width,
                             size_t height, size_t tox, size_t toy)
{
    uint32_t *from = fb_buffer + fromy * fb_width + fromx;
    uint32_t *to = fb_buffer + toy * fb_width + tox;
    while (height--)
    {
        screen_copy_horizontal(from, to, width);
        from += fb_width;
        to += fb_width;
    }
}

inline void screen_flush()
{
#if DOUBLE_BUFFERING
    __asm__ volatile("cld; rep movsl;" ::"S"(fb_buffer), "D"(fb),
                     "c"(fb_width * fb_height)
                     : "cc");
#endif
}

static char *shutdown_message = "Weenix has halted cleanly!";
void screen_print_shutdown()
{
    color_t background = {.value = 0x00000000};
    color_t foreground = {.value = 0x00FFFFFF};
    screen_fill(background);
    size_t str_len = strlen(shutdown_message);
    size_t str_width = str_len * screen_get_character_width();
    size_t str_height = screen_get_character_height();
    screen_draw_string((screen_get_width() - str_width) >> 1,
                       (screen_get_height() - str_height) >> 1,
                       shutdown_message, str_len, foreground);
}

#else

#include "config.h"
#include "drivers/screen.h"
#include "main/io.h"

/* Port addresses for the CRT controller */
#define CRT_CONTROL_ADDR 0x3d4
#define CRT_CONTROL_DATA 0x3d5

/* Addresses we can pass to the CRT_CONTROLL_ADDR port */
#define CURSOR_HIGH 0x0e
#define CURSOR_LOW 0x0f

static uintptr_t vga_textbuffer_phys = 0xB8000;
static uint16_t *vga_textbuffer;
static uint16_t vga_blank_screen[VGA_HEIGHT][VGA_WIDTH];
uint16_t vga_blank_row[VGA_WIDTH];

void vga_enable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void vga_disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_init()
{
    /* map the VGA textbuffer (vaddr) to the VGA textbuffer physical address */
    size_t pages =
        ADDR_TO_PN(PAGE_ALIGN_UP((uintptr_t)sizeof(vga_blank_screen)));
    vga_textbuffer = page_alloc_n(pages);
    KASSERT(vga_textbuffer);

    pt_map_range(pt_get(), (uintptr_t)vga_textbuffer_phys,
                 (uintptr_t)vga_textbuffer,
                 (uintptr_t)vga_textbuffer + ((uintptr_t)PN_TO_ADDR(pages)),
                 PT_PRESENT | PT_WRITE, PT_PRESENT | PT_WRITE);
    pt_set(pt_get());

    for (size_t i = 0; i < VGA_WIDTH; i++)
    {
        vga_blank_row[i] = (VGA_DEFAULT_ATTRIB << 8) | ' ';
    }
    for (size_t i = 0; i < VGA_HEIGHT; i++)
    {
        memcpy(&vga_blank_screen[i], vga_blank_row, VGA_LINE_SIZE);
    }

    vga_enable_cursor();
    vga_clear_screen();
}

void vga_set_cursor(size_t row, size_t col)
{
    uint16_t pos = (row * VGA_WIDTH) + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_clear_screen()
{
    memcpy(vga_textbuffer, vga_blank_screen, sizeof(vga_blank_screen));
}

void vga_write_char_at(size_t row, size_t col, uint16_t v)
{
    KASSERT(row < VGA_HEIGHT && col < VGA_WIDTH);
    vga_textbuffer[(row * VGA_WIDTH) + col] = v;
}

static char *shutdown_message = "Weenix has halted cleanly!";
void screen_print_shutdown()
{
    vga_disable_cursor();
    vga_clear_screen();
    int x = (VGA_WIDTH - strlen(shutdown_message)) / 2;
    int y = VGA_HEIGHT / 2;

    for (size_t i = 0; i < strlen(shutdown_message); i++)
    {
        vga_write_char_at(y, x + i,
                          (VGA_DEFAULT_ATTRIB << 8) | shutdown_message[i]);
    }
}

#endif