#include<linux/module.h>
#include<linux/proc_fs.h>
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
char all_leds_on = 0xFF;
char blink_delay = 0;
unsigned char buffer[1024*1024];
#define RESTORE_LEDS 	0xFF
#define PROC_FILE_NAME "crypto_services"

/*ssize_t read_simple(struct file *filp, char *buf, size_t count, loff_t *offp){
	if(read_out){
		read_out=0;
		return;	
	}
	copy_to_user(buf, buffer, strlen(buffer));
	read_out = 1;
	return strlen(buffer);
}*/

unsigned int call_count = 0;
ssize_t write_simple(struct file *filp, const char *buf, size_t count, loff_t *offp){
	int i;
	strncpy(buffer, buf, count);
	buffer[count] = 0;
	printk("Write was called (%u). The user thinks they're going to write %s to a fil.\n", call_count++, buffer);
	if(buffer[0] == 'L'){
		all_leds_on = (int)(buffer[1] - '0');
	}
	else if (buffer[0] == 'D'){
		blink_delay = HZ /(int)(buffer[1]-'0');
	}
	printk("Enprypted: %s\n", buffer);
	return count;
}


struct file_operations proc_fops = {
//	read: read_simple,
	write: write_simple
};

static void my_timer_func(struct timer_list *timers){
	int *pstatus = (int *)&kbledstatus;
	if(*pstatus == all_leds_on){
		*pstatus = RESTORE_LEDS;
	}
	else{
		*pstatus = all_leds_on;
	}

	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, *pstatus);

	my_timer.expires = jiffies + blink_delay;
	add_timer(&my_timer);
}

static int __init kbleds_init(void){
	int i;
	proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);
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

	timer_setup(&my_timer, my_timer_func, 0);
	my_timer.expires = jiffies + blink_delay;
	add_timer(&my_timer);

	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, all_leds_on);

	return 0;
}

static void __exit kbleds_cleanup(void){
	remove_proc_entry(PROC_FILE_NAME, NULL);
	printk(KERN_INFO "kbleds: unloading...\n");
	del_timer(&my_timer);
	(my_driver -> ops -> ioctl) (vc_cons[fg_console].d -> port.tty, KDSETLED, RESTORE_LEDS);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);
