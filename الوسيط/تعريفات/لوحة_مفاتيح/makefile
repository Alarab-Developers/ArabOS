# المتغيرات
CC = gcc
LD = ld

CFLAGS = -m64 -ffreestanding -fno-pie -fno-pic -nostdlib -Wall -Wextra -I. -c
# هنا قمنا بتغيير الخيارات لإنتاج ملف ثنائي مباشر وتحديد نقطة بداية افتراضية (مثلاً 0x1000)
LDFLAGS = -nostdlib --oformat binary -Ttext 0x1000

# الهدف الافتراضي
all: keyboard.bin

# بناء الملف التنفيذي النهائي
keyboard.bin: لوحة_مفاتيح.o
	$(LD) $(LDFLAGS) -o keyboard.bin لوحة_مفاتيح.o

# تجميع ملف السي
لوحة_مفاتيح.o: لوحة_مفاتيح.c
	$(CC) $(CFLAGS) لوحة_مفاتيح.c -o لوحة_مفاتيح.o

# تنظيف الملفات الناتجة عن البناء
clean:
	rm -f *.o keyboard.bin

