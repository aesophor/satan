KVERSION = $(shell uname -r)
ROOTKIT = satan

obj-m = $(ROOTKIT).o

$(ROOTKIT)-y += main.o
$(ROOTKIT)-y += dev.o
$(ROOTKIT)-y += hide.o
$(ROOTKIT)-y += syscall_hijack.o

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
