#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define APP_FORMAT "[ت]"

typedef struct
{
    char format[128];
    uint64_t entry_point;
    uint64_t data_size;
} aros_header_t;

/* طباعة البايتات للتحقق من UTF-8 */
static void dump_bytes(const char* name, const char* s)
{
    printf("%s = \"%s\"\n", name, s);

    const unsigned char* p =
        (const unsigned char*)s;

    while (*p)
    {
        printf("%02X ", *p);
        p++;
    }

    printf("00\n\n");
}

static int pack(
    const char* in_path,
    const char* out_path
)
{
    FILE* in = fopen(in_path, "rb");

    if (!in)
    {
        fprintf(stderr,
            "cannot open %s\n",
            in_path);

        return 1;
    }

    FILE* out = fopen(out_path, "wb");

    if (!out)
    {
        fclose(in);

        fprintf(stderr,
            "cannot create %s\n",
            out_path);

        return 1;
    }

    fseek(in, 0, SEEK_END);

    uint64_t size =
        (uint64_t)ftell(in);

    rewind(in);

    uint8_t* data =
        malloc(size);

    if (!data)
    {
        fclose(in);
        fclose(out);

        return 1;
    }

    fread(
        data,
        1,
        size,
        in
    );

    aros_header_t hdr;

    memset(
        &hdr,
        0,
        sizeof(hdr)
    );

    /* الأفضل strcpy */
    strcpy(
        hdr.format,
        APP_FORMAT
    );

    hdr.entry_point = 0;
    hdr.data_size    = size;

    fwrite(
        &hdr,
        sizeof(hdr),
        1,
        out
    );

    fwrite(
        data,
        1,
        size,
        out
    );

    fclose(in);
    fclose(out);

    free(data);

    printf(
        "packed %s -> %s (%llu bytes)\n",
        in_path,
        out_path,
        (unsigned long long)size
    );

    return 0;
}
int main(
    int argc,
    char** argv
)
{
    if (argc < 2)
    {
        printf(
            "الاستخدام:\n"
            "./بناء_التطبيقات app.bin\n"
        );

        return 1;
    }

    char out_name[256];

    strcpy(
        out_name,
        argv[1]
    );

    char* dot =
        strrchr(
            out_name,
            '.'
        );

    if (dot)
        *dot = '\0';

    strcat(
        out_name,
        ".ت"
    );

    return pack(
        argv[1],
        out_name
    );
}
