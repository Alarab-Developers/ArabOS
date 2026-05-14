gcc \
-ffreestanding \
-fno-pie \
-fno-stack-protector \
-nostdlib \
-c app.c \
-o app.o

ld \
-Ttext 0x0 \
--oformat binary \
app.o \
-o app.bin


gcc \
-ffreestanding \
-fno-pie \
-fno-stack-protector \
-nostdlib \
-c app_manger.c \
-o app_manger.o

ld \
-Ttext 0x0 \
--oformat binary \
app_manger.o \
-o app_manger.bin

