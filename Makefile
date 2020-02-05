KVERSION = $(shell uname -r)
ROOTKIT = satan

obj-m = $(ROOTKIT).o

$(ROOTKIT)-y += main.o
$(ROOTKIT)-y += dev.o
$(ROOTKIT)-y += file.o
$(ROOTKIT)-y += module.o
$(ROOTKIT)-y += syscall.o
$(ROOTKIT)-y += util.o

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

strip:
	strip --strip-debug $(ROOTKIT).ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
