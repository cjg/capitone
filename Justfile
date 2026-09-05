build:
    python -m prg32 cartridge build capitone.c --entry-prefix capitone --name Capitone --portable --out bin/capitone.prg32 

run-qemu: build
    python -m prg32 qemu upload --flash ${PRG32_HOME}/build-qemu/qemu_flash.bin bin/capitone.prg32
    qemu-system-riscv32 \
        -M esp32c3 \
        -m 4M \
        -drive file=${PRG32_HOME}/build-qemu/qemu_flash.bin,if=mtd,format=raw \
        -drive file=${PRG32_HOME}/build-qemu/qemu_efuse.bin,if=none,format=raw,id=efuse \
        -global driver=nvram.esp32c3.efuse,property=drive,value=efuse \
        -global driver=timer.esp32c3.timg,property=wdt_disable,value=true \
        -nic user,model=open_eth \
        -display sdl \
        -serial mon:stdio
