#include "مدير_الكومة.h"
#include "محرك_الذاكرة/مدير_الصفحات/مدير_الصفحات.h"
#include "مكتبة_المحركات/مكتبة_معالجة_النصوص.h"

#define PAGE_SIZE         4096
#define HEAP_START        0xFFFFC00000000000ULL
#define HIGHER_HALF_BASE  0xFFFF800000000000ULL

/* =========================================================
 * Physical Memory Layout
 * ========================================================= */

static uint64_t first_free_frame = 0;
static uint64_t alloc_base_frame = 0;

static uint64_t last_frame = 0x4000000ULL; /* 64MB */

static uint8_t* frame_bitmap = 0;
static uint64_t bitmap_size  = 0;

/* =========================================================
 * Heap State
 * ========================================================= */

static uint64_t heap_current = HEAP_START;
static uint64_t heap_end     = HEAP_START;

extern uint64_t get_kernel_end_phys();

/* =========================================================
 * frame_allocator_init
 * ========================================================= */

void frame_allocator_init() {

    uint64_t kernel_end = get_kernel_end_phys();

    /*
     * أول frame متاح بعد الكيرنل
     */
    first_free_frame =
        (kernel_end + PAGE_SIZE - 1) &
        ~((uint64_t)PAGE_SIZE - 1);

    /*
     * عدد الـ frames الكلي
     */
    uint64_t total_frames =
        (last_frame - first_free_frame) / PAGE_SIZE;

    /*
     * حجم الـ bitmap بالبايت
     */
    bitmap_size = (total_frames + 7) / 8;

    uint64_t* k_pml4 = paging_get_pml4();

    /*
     * Map bitmap memory
     */
    for (uint64_t offset = 0;
         offset < bitmap_size;
         offset += PAGE_SIZE) {

        uint64_t phys = first_free_frame + offset;

        uint64_t virt =
            HIGHER_HALF_BASE + phys;

        map_page(
            k_pml4,
            virt,
            phys,
            PAGE_PRESENT | PAGE_WRITE
        );
    }

    /*
     * Virtual address للـ bitmap
     */
    frame_bitmap =
        (uint8_t*)(HIGHER_HALF_BASE + first_free_frame);

    /*
     * تصفير الـ bitmap
     */
    memset(frame_bitmap, 0, bitmap_size);

    /*
     * عدد الـ frames المستخدمة للـ bitmap نفسه
     */
    uint64_t bitmap_frames =
        (bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;

    /*
     * احجز frames الخاصة بالـ bitmap
     */
    for (uint64_t i = 0; i < bitmap_frames; i++) {

        uint64_t byte_idx = i / 8;
        uint64_t bit_idx  = i % 8;

        frame_bitmap[byte_idx] |= (1 << bit_idx);
    }

    /*
     * أول frame حقيقي متاح للتخصيص
     */
    alloc_base_frame =
        first_free_frame +
        (bitmap_frames * PAGE_SIZE);
}

/* =========================================================
 * alloc_frame
 * ========================================================= */

uint64_t alloc_frame() {

    if (!frame_bitmap)
        return 0;

    uint64_t total_bits = bitmap_size * 8;

    for (uint64_t i = 0; i < total_bits; i++) {

        uint64_t byte_idx = i / 8;
        uint64_t bit_idx  = i % 8;

        /*
         * frame غير مستخدم
         */
        if (!(frame_bitmap[byte_idx] & (1 << bit_idx))) {

            /*
             * احجزه
             */
            frame_bitmap[byte_idx] |= (1 << bit_idx);

            /*
             * أرجع العنوان الفيزيائي
             */
            return alloc_base_frame + (i * PAGE_SIZE);
        }
    }

    return 0;
}

/* =========================================================
 * heap_expand
 * ========================================================= */

static int heap_expand() {

    uint64_t frame = alloc_frame();

    if (!frame)
        return 0;

    uint64_t* k_pml4 = paging_get_pml4();

    map_page(
        k_pml4,
        heap_end,
        frame,
        PAGE_PRESENT | PAGE_WRITE
    );

    heap_end += PAGE_SIZE;

    return 1;
}

/* =========================================================
 * heap_init
 * ========================================================= */

void heap_init() {

    frame_allocator_init();

    uint64_t* k_pml4 = paging_get_pml4();

    /*
     * أول صفحة للهيب
     */
    uint64_t frame = alloc_frame();

    if (!frame)
        return;

    map_page(
        k_pml4,
        HEAP_START,
        frame,
        PAGE_PRESENT | PAGE_WRITE
    );

    heap_current = HEAP_START;
    heap_end     = HEAP_START + PAGE_SIZE;
}

/* =========================================================
 * kmalloc
 * ========================================================= */

void* kmalloc(size_t size) {

    if (size == 0)
        return 0;

    /*
     * إصلاح alignment:
     *
     * المشكلة القديمة:
     *   الـ header = sizeof(size_t) = 8 bytes
     *   → ptr = heap_current + 8
     *   → ptr % 16 = 8  (غير محاذٍ!)
     *   → p->rsp يُكتب في عنوان غير محاذٍ
     *   → iretq يقرأ RSP خاطئاً → crash
     *
     * الإصلاح:
     *   الـ header = 16 bytes ثابتة
     *   → ptr = heap_current + 16
     *   → ptr محاذٍ لـ 16 دائماً 
     */
    #define KMALLOC_HEADER_SIZE 16

    /* 16-byte alignment للحجم */
    if (size % 16 != 0)
        size += 16 - (size % 16);

    /* Header (16) + payload */
    size_t total = size + KMALLOC_HEADER_SIZE;

    /* Align heap_current لـ 16 */
    if (heap_current % 16 != 0)
        heap_current += 16 - (heap_current % 16);

    /* وسّع الهيب عند الحاجة */
    while (heap_current + total > heap_end) {

        if (!heap_expand())
            return 0;
    }

    /* Header: نكتب الحجم في أول 8 bytes */
    size_t* header = (size_t*)heap_current;
    *header = size;

    /*
     * User pointer: يبدأ بعد الـ 16 bytes
     * heap_current محاذٍ لـ 16 → ptr محاذٍ لـ 16 
     */
    void* ptr = (void*)(heap_current + KMALLOC_HEADER_SIZE);

    /* صفّر الذاكرة */
    memset(ptr, 0, size);

    /* حرّك المؤشر */
    heap_current += total;

    return ptr;
}

/* =========================================================
 * kfree
 * ========================================================= */

void kfree(void* ptr) {

    /*
     * allocator بسيط (bump allocator)
     * لا يدعم free حالياً
     */

    (void)ptr;
}
