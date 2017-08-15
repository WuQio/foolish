obj-m := module2.o

KDIR := /lib/modules/`uname -r`/build
PWD := $(shell pwd)
default:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

