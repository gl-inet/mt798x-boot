# ATF and u-boot for GL.iNet products based on mt798x

![](/u-boot.gif)

# Quickstart

## Prepare

	sudo apt install gcc-aarch64-linux-gnu device-tree-compiler bison bc
	export CROSS_COMPILE=aarch64-linux-gnu-

# For MT3000

## Build u-boot

	make gl_mt3000_defconfig
	make

## Build ATF

	cp ../uboot-mtk-20220606/u-boot.bin .
	make gl_mt3000_defconfig
	make

# For MT6000

## Build u-boot

	make gl_mt6000_defconfig
	make

## Build ATF

	cp ../uboot-mtk-20220606/u-boot.bin .
	make gl_mt6000_defconfig
	make

## Flash fip.bin

	build/mt7981/release/fip.bin

### Warning

**Flashing may result in bricking, proceed with caution. It is not necessary to proceed if it is not required.**
