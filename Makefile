BUILD=build
ISO=نظام_العرب.iso
النواه=النواه.elf
ISO_DIR=iso
BACKGROUND=الخلفية.png

all: $(ISO)

# ================= الوسيط =================
النظام_المضغوط:
	rm -f الوسيط/النظام_المضغوط.tar
	tar -cf الوسيط/النظام_المضغوط.tar \
		النظام \
		المستخدمين \
		التكوين \
		المتغيرات

# ================= إنشاء initramfs =================
النظام/الوسيط.img: النظام_المضغوط
	mkdir -p النظام
	cd الوسيط && find . | cpio -o -H newc > ../النظام/الوسيط.img

# ================= إنشاء ISO =================
$(ISO): النظام/الوسيط.img
	mkdir -p $(ISO_DIR)/boot/grub
	mkdir -p $(ISO_DIR)/boot/الخطوط
	mkdir -p $(ISO_DIR)/النظام

	cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/
	cp boot/$(BACKGROUND) $(ISO_DIR)/boot/
	cp boot/الخطوط/الخط.pf2 $(ISO_DIR)/boot/الخطوط/

	@if [ -f boot/$(النواه) ]; then \
		cp boot/$(النواه) $(ISO_DIR)/boot/; \
	else \
		echo "تحذير: الملف boot/$(النواه) غير موجود، سيتم المتابعة بدون النواة."; \
	fi

	cp النظام/الوسيط.img $(ISO_DIR)/النظام/

	# نسخ مجلدات النظام
	cp -r النظام $(ISO_DIR)/
	cp -r المستخدمين $(ISO_DIR)/
	cp -r التكوين $(ISO_DIR)/
	cp -r المتغيرات $(ISO_DIR)/

	grub-mkrescue -o $(ISO) $(ISO_DIR)

# ================= التشغيل =================
run: $(ISO)
	qemu-system-x86_64 -m 2G \
		-enable-kvm \
		-bios /usr/share/ovmf/OVMF.fd \
		-cdrom $(ISO) \
		-vga virtio \
		-display gtk,zoom-to-fit=on \
		-machine pc,accel=kvm \
		-global ICH9-LPC.disable_s3=1 \
		-global ICH9-LPC.disable_s4=1

# ================= التصحيح =================
debug: $(ISO)
	qemu-system-x86_64 -m 2G \
		-bios /usr/share/ovmf/OVMF.fd \
		-cdrom $(ISO) \
		-no-reboot -no-shutdown \
		-d int,mmu,guest_errors,cpu_reset \
		-D qemu.log \
		-serial stdio \
		-display gtk \
		-vga std

# ================= التنظيف =================
clean:
	rm -rf $(ISO_DIR)
	rm -f $(ISO)
	rm -f النظام/الوسيط.img
	rm -f الوسيط/النظام_المضغوط.tar

.PHONY: all clean run debug النظام_المضغوط
