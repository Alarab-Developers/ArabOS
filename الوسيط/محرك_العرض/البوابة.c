#include <stdint.h>

#include "بوابة_النظام.h"
#include "المحركات/خادم_العرض/الخادم.h"

void _start(kernel_api_t* api)
{
    server_run(api);
}
