qemu-system-x86_64 \
    -bios /usr/share/ovmf/x64/OVMF.fd \
    -drive id=disk,file=image.hdd,if=none \
    -device ahci,id=ahci \
    -device ide-hd,drive=disk,bus=ahci.0 -m 256M \
    "$@"
