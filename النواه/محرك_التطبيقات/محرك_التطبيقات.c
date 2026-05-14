#include "محرك_التطبيقات.h"
#include "محرك_التطبيقات/الصيغ/bin/bin_format.h"

void app_run(file_t* f)
{
    if (!f)
        return;

    bin_run(f);
}
