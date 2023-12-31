CURRENT	= $(shell uname -r)
TARGET	= asix
OBJS	= asix.o
MDIR	= drivers/net/usb
KDIR	= /lib/modules/$(CURRENT)/build
CC	:= gcc
SUBLEVEL= $(shell uname -r | cut -d '.' -f 3 | cut -d '.' -f 1 | cut -d '-' -f 1 | cut -d '_' -f 1)

ifneq (,$(filter $(SUBLEVEL),14 15 16 17 18 19 20 21))
MDIR = drivers/usb/net
endif

PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

obj-m      := $(TARGET).o

default:
	make -C $(KDIR) M=$(PWD) modules
	$(CC) ioctl.c -o ioctl

$(TARGET).o: $(OBJS)
	$(LD) $(LD_RFLAG) -r -o $@ $(OBJS)

install:
	su -c "cp -v $(TARGET).ko $(DEST) && /sbin/depmod -a"

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf *.mod ioctl

.PHONY: modules clean

-include $(KDIR)/Rules.make
