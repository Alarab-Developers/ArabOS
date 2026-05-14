#include <string.h>
#include "بوابة_الذاكرة.h"
#include "محرك_الذاكرة/محرك_الذاكرة.h"
#include "محرك_الذاكرة/مدير_الكومة/مدير_الكومة.h"
#include "محرك_الذاكرة/مدير_الصفحات/مدير_الصفحات.h"

/* ================================================================
 * pml4 الكيرنل — عنوان فيزيائي
 * يُحفظ هنا بعد paging_init لاستخدامه في api_map
 * ================================================================ */
static uint64_t kernel_pml4_phys = 0;

/* ================================================================
 * api_alloc_page
 * ================================================================ */
static uint64_t api_alloc_page() {
    return alloc_frame();
}

/* ================================================================
 * api_map — map في address space الكيرنل الحالي
 * pml4 الكيرنل هو virtual pointer (pml4_table في .bss)
 * ================================================================ */
static void api_map(uint64_t virt, uint64_t phys, uint64_t flags) {
    map_page(paging_get_pml4(), virt, phys, flags);
}

/* ================================================================
 * api_map_to — map في address space مُحدد
 * target: عنوان virtual لجدول PML4
 * ================================================================ */
static void api_map_to(uint64_t* target, uint64_t virt,
                        uint64_t phys, uint64_t flags) {
    map_page(target, virt, phys, flags);
}

/* ================================================================
 * api_init
 *
 * ترتيب التهيئة الصحيح:
 *   1. paging_init — تبني الجداول الثابتة (identity + higher half)
 *      وتستخدم early_alloc_frame داخلياً
 *   2. نحمّل CR3 للجدول الجديد
 *   3. memory_init — تهيئة frame allocator + heap
 *      (تحتاج map_page التي تحتاج pml4 → لذا بعد خطوة 1)
 * ================================================================ */
static void api_init(void) {
    paging_init();

    uint64_t* virt_pml4 = paging_get_pml4();

    /* احفظ العنوان الفيزيائي لـ pml4 الكيرنل */
    kernel_pml4_phys = (uint64_t)virt_pml4 - 0xFFFF800000000000ULL;

    /* حمّل CR3 بالعنوان الفيزيائي */
    load_cr3_phys(kernel_pml4_phys);

    memory_init();
}

/* ================================================================
 * api_get_pml4
 * يُرجع عنوان virtual لجدول PML4 الكيرنل
 * ================================================================ */
static uint64_t* api_get_pml4() {
    return paging_get_pml4();
}

/* ================================================================
 * api_create_address_space
 *
 * تُنشئ PML4 جديداً معزولاً للعملية:
 * - يُنسخ kernel space (indices 256-511)
 * - يُرجع العنوان الفيزيائي (لوضعه مباشرةً في CR3)
 * ================================================================ */
static uint64_t* api_create_address_space() {

    uint64_t phys = alloc_frame();
    if (!phys) return 0;

    /* الوصول للصفحة عبر higher half mapping */
    uint64_t* virt = (uint64_t*)(0xFFFF800000000000ULL + phys);

    memset(virt, 0, 4096);

    /* نسخ kernel space من PML4 الكيرنل */
    uint64_t* k = paging_get_pml4();
    for (int i = 256; i < 512; i++)
        virt[i] = k[i];

    /* نُرجع العنوان الفيزيائي — يُوضع مباشرةً في CR3 */
    return (uint64_t*)phys;
}

/* ================================================================
 * api_switch_address_space
 *
 * target_pml4: عنوان فيزيائي (كما تُرجعه api_create_address_space)
 *
 * المشكلة السابقة:
 *   load_cr3 كانت تطرح HIGHER_HALF_BASE من المعامل —
 *   لكن p->pml4 هو عنوان فيزيائي أصلاً →
 *   فيزيائي - HIGHER_HALF_BASE = قيمة سالبة كبيرة → CR3 خاطئ → triple fault
 *
 * الإصلاح:
 *   نستخدم load_cr3_phys التي تضع المعامل مباشرةً في CR3 بدون طرح.
 * ================================================================ */
static void api_switch_address_space(uint64_t* target_pml4) {
    /* target_pml4 هو عنوان فيزيائي خام */
    load_cr3_phys((uint64_t)target_pml4);
}

/* ================================================================
 * api_get_current_address_space
 * ================================================================ */
static uint64_t* api_get_current_address_space() {
    return paging_get_pml4();
}

/* ================================================================
 * memory_api
 * ================================================================ */
memory_api_t memory_api = {
    .init                      = api_init,
    .alloc                     = kmalloc,
    .free                      = kfree,
    .alloc_page                = api_alloc_page,
    .map                       = api_map,
    .map_to                    = api_map_to,
    .get_pml4                  = api_get_pml4,
    .create_address_space      = api_create_address_space,
    .switch_address_space      = api_switch_address_space,
    .get_current_address_space = api_get_current_address_space,
};
