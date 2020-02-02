KVERSION = $(shell uname -r)

obj-m = satan.o
medusa-objs = 

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
