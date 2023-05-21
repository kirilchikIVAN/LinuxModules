#obj-m += simple_sym_dev.o
obj-m += phonebook.o
#obj-m += lastword.o

KDIR := /home/kirilchik/MIPT/Linux/initramfs/lib/modules/6.1.9/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
