obj-m += keyboard_stat.o

KDIR := /home/kirilchik/MIPT/Linux/initramfs/lib/modules/6.1.9/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
