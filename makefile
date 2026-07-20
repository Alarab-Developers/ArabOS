# ====================================
# Variables
# ====================================
BOOTX64=EFI/BOOT/BOOTX64.EFI
ESP=esp.img
ISO=ArabOS.iso

OVMF_CODE=/usr/share/OVMF/OVMF_CODE_4M.fd
OVMF_VARS=OVMF_VARS.fd

export LANG=ar_EG.UTF-8
export LC_ALL=ar_EG.UTF-8

# ====================================
# Default Target
# ====================================

all: $(ISO)

# ====================================
# Build EFI FAT Image
# ====================================

$(ESP):
	rm -f $(ESP)

	dd if=/dev/zero of=$(ESP) bs=1M count=80
	mkfs.vfat -F 32 -n "ArabOS" $(ESP)

	# ========================================
	# إنشاء هيكل مجلدات EFI
	# ========================================
	mmd -i $(ESP) ::EFI
	mmd -i $(ESP) ::EFI/BOOT


	# نسخ ملف الإقلاع الرئيسي
	mcopy -i $(ESP) \
		$(BOOTX64) \
		::EFI/BOOT/BOOTX64.EFI

	# ========================================
	# إنشاء هيكل مجلدات المقلع/اعدادات_القائمة
	# ========================================
	mmd -i $(ESP) ::المقلع
	mmd -i $(ESP) ::النظام
	mmd -i $(ESP) ::المقلع/اعدادات_القائمة
	mmd -i $(ESP) ::المقلع/اعدادات_القائمة/ثيم_القائمة
	mmd -i $(ESP) ::النظام/الوحدات
	mmd -i $(ESP) ::النظام/الوحدات/الثيمات
	mmd -i $(ESP) ::النظام/الوحدات/الثيمات/شعار_التمهيد

	# نسخ ملف القائمة
	if [ -f "المقلع/اعدادات_القائمة/القائمة.نص" ]; then \
		mcopy -i $(ESP) \
			المقلع/اعدادات_القائمة/القائمة.نص \
			::المقلع/اعدادات_القائمة/القائمة.نص; \
	fi




	if [ -f "المقلع/النواة.تطبيق" ]; then \
		mcopy -i $(ESP) \
			المقلع/النواة.تطبيق \
			::المقلع/النواة.تطبيق; \
	fi
	# نسخ الصور من المسار الصحيح
	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/الخلفية.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/الخلفية.png || true

	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/نافذه.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/نافذه.png || true

	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/تحديد.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/تحديد.png || true

	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/سهم.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/سهم.png || true

	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/شريط_المهلة.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/شريط_المهلة.png || true

	mcopy -i $(ESP) \
		المقلع/اعدادات_القائمة/ثيم_القائمة/شعار.png \
		::المقلع/اعدادات_القائمة/ثيم_القائمة/شعار.png || true

	# نسخ صور الأزرار
	for f in المقلع/اعدادات_القائمة/ثيم_القائمة/زر_*.png; do \
		if [ -f "$$f" ]; then \
			base=$$(basename "$$f"); \
			echo "نسخ $$base"; \
			mcopy -i $(ESP) "$$f" "::المقلع/اعدادات_القائمة/ثيم_القائمة/$$base"; \
		fi \
	done

	# نسخ صور الشعار
	for f in النظام/الوحدات/الثيمات/شعار_التمهيد/*.png; do \
		if [ -f "$$f" ]; then \
			base=$$(basename "$$f"); \
			echo "نسخ شعار: $$base"; \
			mcopy -i $(ESP) "$$f" \
				"::النظام/الوحدات/الثيمات/شعار_التمهيد/$$base"; \
		fi \
	done

# ====================================
# Build UEFI ISO
# ====================================

$(ISO): $(ESP)
	rm -rf iso_build

	mkdir -p iso_build/EFI/BOOT


	cp $(BOOTX64) iso_build/EFI/BOOT/BOOTX64.EFI


	# نسخ مجلد المقلع بالكامل مع جميع محتوياته
	cp -r المقلع iso_build/

	cp $(ESP) iso_build/efiboot.img

	xorriso \
		-as mkisofs \
		-R \
		-J \
		-joliet-long \
		-input-charset utf-8 \
		-V ArabOS \
		-eltorito-alt-boot \
		-e efiboot.img \
		-no-emul-boot \
		-o $(ISO) \
		iso_build

	rm -rf iso_build

# ====================================
# OVMF Variables
# ====================================

$(OVMF_VARS):
	cp /usr/share/OVMF/OVMF_VARS_4M.fd $(OVMF_VARS)

# ====================================
# Run ISO Only
# ====================================

runiso: $(ISO) $(OVMF_VARS)
	qemu-system-x86_64 \
		-machine q35 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-cdrom $(ISO)

# ====================================
# Run FAT Image
# ====================================

run: $(ESP) $(OVMF_VARS)
	qemu-system-x86_64 \
		-machine q35 \
		-m 512M \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-drive format=raw,file=$(ESP)

# ====================================
# Create Missing Images
# ====================================

create-images:
	@mkdir -p المقلع/اعدادات_القائمة
	@mkdir -p النظام/الوحدات/الثيمات/شعار_التمهيد

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/الخلفية.png" ]; then \
		echo "إنشاء الخلفية.png افتراضية..."; \
		convert -size 1920x1080 gradient:darkblue-navy \
		المقلع/اعدادات_القائمة/ثيم_القائمة/الخلفية.png 2>/dev/null || true; \
	fi

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/نافذه.png" ]; then \
		echo "إنشاء نافذه.png افتراضية..."; \
		convert -size 1000x500 xc:'#1E1E2E' \
		-stroke white -strokewidth 2 -fill none \
		-draw "rectangle 10,10 990,490" \
		المقلع/اعدادات_القائمة/ثيم_القائمة/نافذه.png 2>/dev/null || true; \
	fi

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/سهم.png" ]; then \
		echo "إنشاء سهم.png افتراضي..."; \
		convert -size 16x16 xc:none \
		-fill white \
		-draw "polygon 4,2 4,14 12,8" \
		المقلع/اعدادات_القائمة/ثيم_القائمة/سهم.png 2>/dev/null || true; \
	fi

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/شعار.png" ]; then \
		echo "إنشاء شعار.png افتراضي..."; \
		convert -size 1000x46 gradient:blue-darkblue \
		المقلع/اعدادات_القائمة/ثيم_القائمة/شعار.png 2>/dev/null || true; \
	fi

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/تحديد.png" ]; then \
		echo "إنشاء تحديد.png افتراضي..."; \
		convert -size 980x44 xc:'#1A6EBF' \
		-fill '#2E8BFF' -draw "rectangle 0,0 980,44" \
		المقلع/اعدادات_القائمة/ثيم_القائمة/تحديد.png 2>/dev/null || true; \
	fi

	@if [ ! -f "المقلع/اعدادات_القائمة/ثيم_القائمة/شريط_المهلة.png" ]; then \
		echo "إنشاء شريط_المهلة.png افتراضي..."; \
		convert -size 960x6 xc:'#44AAFF' \
		المقلع/اعدادات_القائمة/ثيم_القائمة/شريط_المهلة.png 2>/dev/null || true; \
	fi

	@echo "تم إنشاء الصور الافتراضية في مجلد 'المقلع/اعدادات_القائمة'"

# ====================================
# Create a dummy kernel for testing
# ====================================



# ====================================
# Clean
# ====================================

clean:
	rm -f *.img
	rm -f *.iso
	rm -f $(OVMF_VARS)
	rm -rf iso_build

.PHONY: all run runiso clean create-images create-kernel
