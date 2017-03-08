.PHONY: clean all

# Output directory for each submakefiles
OUTPUT := out
export OUTPUT

#
# Some build tools need to be explicitely defined before building. The toolchain
# creates the following platform tools configuration file before it allows the
# toolchain to build.
#
PLATFORM_TOOLS := $(OUTPUT)/platform-tools.mk
export PLATFORM_TOOLS

#Rajouté :
QEMU = /usr/bin/qemu-system-i386
QEMUOPTS = -cpu pentium -rtc base=localtime -m 64M -gdb tcp::1234 -S -kernel kernel/kernel.bin

GO_QEMUOPTS = -cpu pentium -rtc base=localtime -m 64M -gdb tcp::1234 -kernel kernel/kernel.bin

#fin rajout


all: | kernel/$(PLATFORM_TOOLS) user/$(PLATFORM_TOOLS)
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

kernel/$(PLATFORM_TOOLS):
	$(MAKE) -C kernel/ $(PLATFORM_TOOLS)

user/$(PLATFORM_TOOLS):
	$(MAKE) -C user/ $(PLATFORM_TOOLS)

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

#rajouté :
run:
	$(QEMU) $(QEMUOPTS)

go:
	$(QEMU) $(GO_QEMUOPTS)

gdb:
	gdb -d kernel -ex "tar rem :1234" kernel/kernel.bin

ddd:
	ddd -d kernel -ex "tar rem :1234" kernel/kernel.bin
