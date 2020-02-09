KVERSION = $(shell uname -r)
ROOTKIT = satan

obj-m = $(ROOTKIT).o

$(ROOTKIT)-y += main.o
$(ROOTKIT)-y += cdev.o
$(ROOTKIT)-y += command.o
$(ROOTKIT)-y += file.o
$(ROOTKIT)-y += module.o
$(ROOTKIT)-y += port.o
$(ROOTKIT)-y += privesc.o
$(ROOTKIT)-y += proc.o
$(ROOTKIT)-y += syscall.o
$(ROOTKIT)-y += util.o
$(ROOTKIT)-y += wp.o


all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

strip:
	strip --strip-debug $(ROOTKIT).ko

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
