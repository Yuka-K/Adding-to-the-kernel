#include<linux/module.h>
#include<linux/init.h>
#include<linux/tty.h>
#include<linux/kd.h>
#include<linux/vt.h>
#include<linux/vt_kern.h>
#include<linux/console_struct.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_AUTHOR("Daniele Paolo Scarpazza");
MODULE_LICENSE("GPL");

struct timer_list my_timer;
struct tty_driver *my_driver;
char kbledstatus = 0;
char all_leds_on = 0x07;

#define BLINK_DELAY 	HZ/5
#define RESTORE_LEDS 	0xFF

/*
static void my_timer_func(struct timer_list *timers){
	int *pstatus = (int *)&kbledstatus;
	if(*pstatus == all_leds_on){
		*pstatus = RESTORE_LEDS;
	}
	else{
		*pstatus = all_leds_on;
	}

	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, *pstatus);

	my_timer.expires = jiffies + BLINK_DELAY;
	add_timer(&my_timer);
}*/

static int __init kbleds_init(void){
	int i;

	printk(KERN_INFO "kbleds: loading\n");
	printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
	for (i = 0; i < MAX_NR_CONSOLES;i++){\
		if(!vc_cons[i].d){
			break;
		}
	printk(KERN_INFO "poet_atkm: console[%i/%i] #%i, tty %lx\n", i, MAX_NR_CONSOLES, vc_cons[i].d -> vc_num, (unsigned long)vc_cons[i].d -> port.tty);
	}
	printk(KERN_INFO "kbleds: finished scanning consoles\n");

	my_driver = vc_cons[fg_console].d -> port.tty -> driver;
	printk(KERN_INFO "kbleds: tty driver magic %x\n", my_driver -> magic);

//	timer_setup(&my_timer, my_timer_func, 0);
//	my_timer.expires = jiffies + BLINK_DELAY;
//	add_timer(&my_timer);

	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, all_leds_on);

	return 0;
}

static void __exit kbleds_cleanup(void){
	printk(KERN_INFO "kbleds: unloading...\n");
	del_timer(&my_timer);
	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, RESTORE_LEDS);
}




module_init(kbleds_init);
module_exit(kbleds_cleanup);
