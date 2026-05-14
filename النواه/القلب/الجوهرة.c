#include "الجوهرة.h"
#include "محرك_الذاكرة/بوابة_الذاكرة.h"
#include "محرك_الجدولة/بوابة_الجدولة.h"
#include "محرك_العمليات/بوابة_العمليات.h"
#include "أرض_المستخدم/النظام/التعريفات/التخزين/ATA/ata.h"

#include "أرض_المستخدم/النظام/الخدمات/محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"

#include "أرض_المستخدم/النظام/التعريفات/أنظمة_الملفات/نظام_العرب/نظام_العرب.h"


void ata_write_sector(unsigned int lba, unsigned char* buffer);
void ata_read_sector(unsigned int lba, unsigned char* buffer);


void core_init() {

    memory_api.init();
    
    process_api.init();   

    scheduler_api.init();





    unsigned char sector[512];
    unsigned char readback[512];




    ata_write_sector(1, sector);
    ata_read_sector(1, readback);









    
    // تهيئة محرك أنظمة الملفات
    fs_engine_init();
    
    // تسجيل نظام العرب كالنظام الافتراضي(نظام الملفات :) )
    fs_engine_register_driver(get_arabfs_driver());







   
}
void core_run() {

    scheduler_api.schedule();

}

