obj-m += katsuki_yuka_p2.o

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

	
