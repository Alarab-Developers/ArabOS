# ====================================
# Variables
# ====================================
BOOTX64=EFI/BOOT/BOOTX64.EFI
SYSTEM_DIR=النظام
SYSARCHIVE=المقلع/النظام.tar
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

$(ESP): FORCE
	@set -e; \
	echo "إنشاء أرشيف النظام..."; \
	if [ ! -d "$(SYSTEM_DIR)" ]; then \
		echo "خطأ: مجلد النظام غير موجود: $(SYSTEM_DIR)"; \
		exit 1; \
	fi; \
	mkdir -p المقلع; \
		tar -cf "$(SYSARCHIVE)" -C "$(SYSTEM_DIR)" الاعدادات الخدمات الوحدات; \
	echo "تم إنشاء $(SYSARCHIVE)"; \
	rm -f "$(ESP)"; \
	dd if=/dev/zero of="$(ESP)" bs=1M count=80; \
	mkfs.vfat -F 32 -n "ArabOS" "$(ESP)"; \
	echo "إنشاء مجلدات EFI..."; \
	mmd -i "$(ESP)" ::EFI; \
	mmd -i "$(ESP)" ::EFI/BOOT; \
	mcopy -i "$(ESP)" "$(BOOTX64)" ::EFI/BOOT/BOOTX64.EFI; \
	mmd -i "$(ESP)" ::المقلع; \
	mmd -i "$(ESP)" ::المقلع/اعدادات_القائمة; \
	mmd -i "$(ESP)" ::المقلع/اعدادات_القائمة/ثيم_القائمة; \
	echo "نسخ ملفات الإقلاع..."; \
	if [ -f "المقلع/اعدادات_القائمة/القائمة.نص" ]; then \
		mcopy -i "$(ESP)" "المقلع/اعدادات_القائمة/القائمة.نص" "::المقلع/اعدادات_القائمة/القائمة.نص"; \
	fi; \
	if [ -f "المقلع/شاهين.نواة" ]; then \
		mcopy -i "$(ESP)" "المقلع/شاهين.نواة" "::المقلع/شاهين.نواة"; \
	fi; \
	mcopy -i "$(ESP)" "$(SYSARCHIVE)" "::المقلع/النظام.tar"; \
	for name in الخلفية.png نافذه.png تحديد.png سهم.png شريط_المهلة.png شعار.png; do \
		file="المقلع/اعدادات_القائمة/ثيم_القائمة/$$name"; \
		if [ -f "$$file" ]; then \
			mcopy -o -i "$(ESP)" "$$file" "::المقلع/اعدادات_القائمة/ثيم_القائمة/$$name"; \
		fi; \
	done; \
	for f in المقلع/اعدادات_القائمة/ثيم_القائمة/زر_*.png; do \
		if [ -f "$$f" ]; then \
			base=$$(basename "$$f"); \
			mcopy -o -i "$(ESP)" "$$f" "::المقلع/اعدادات_القائمة/ثيم_القائمة/$$base"; \
		fi; \
	done; \
	echo "اكتمل إنشاء صورة الإقلاع: $(ESP)"

# ====================================
# Build UEFI ISO
# ====================================

$(ISO): $(ESP)
	rm -rf iso_build
	mkdir -p iso_build
	cp "$(ESP)" iso_build/efiboot.img

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
		-o "$(ISO)" \
		iso_build

	rm -rf iso_build
	@echo "تم إنشاء $(ISO)"

# ====================================
# OVMF Variables
# ====================================

$(OVMF_VARS):
	cp /usr/share/OVMF/OVMF_VARS_4M.fd "$(OVMF_VARS)"

# ====================================
# Run ISO Only
# ====================================

runiso: $(ISO) $(OVMF_VARS)
	qemu-system-x86_64 \
		-machine q35 \
		-m 4G \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-cdrom "$(ISO)"

# ====================================
# Run FAT Image
# ====================================

run: $(ESP) $(OVMF_VARS)
	qemu-system-x86_64 \
		-machine q35 \
		-m 4G \
		-device virtio-gpu-pci \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS) \
		-device ahci,id=ahci \
		-drive id=disk,file=$(ESP),format=raw,if=none \
		-device ide-hd,drive=disk,bus=ahci.0

# ====================================
# Create Missing Images
# ====================================

create-images:
	@mkdir -p المقلع/اعدادات_القائمة/ثيم_القائمة

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

	@echo "تم إنشاء الصور الافتراضية."

# ====================================
# Clean
# ====================================

clean:
	rm -f *.img
	rm -f *.iso
	rm -f "$(OVMF_VARS)"
	rm -rf iso_build

# إجبار Make على إعادة إنشاء صورة الإقلاع
.PHONY: all run runiso clean create-images create-kernel FORCE
FORCE:
