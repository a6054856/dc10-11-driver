#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>

typedef struct ADC_REG{
	unsigned int  ADCCON;
	unsigned int  ADCDAT;
	unsigned int  ADCINTENB; 
	unsigned int  ADCINTCLR;
	unsigned int  PRESCALERCON;
}adc_t;

	
#define ADC_PHY_BASE 0xc0053000
#define ADCNAME "myadc"
int major;
volatile adc_t *adc;
struct class *cls;
struct device *dev;
int myadc_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
ssize_t myadc_read (struct file *file,
	char __user *ubuf, size_t size, loff_t *offs)
{
	unsigned int data;
	int ret;
	//1.����adc
	adc->ADCCON |= (1<<0);
	
	//2.�ȴ�adcת�����
	while(adc->ADCCON & 0x1);

	//3.������
	data = (adc->ADCDAT)&0xfff;
	
	//4.�����ݿ������û��ռ�
	if(size > sizeof(data))size = sizeof(data);
	ret = copy_to_user(ubuf,&data,size);
	if(ret){
		printk("copy data to user error\n");
		return -EINVAL;
	}
	
	return size;
}

int myadc_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
const struct file_operations fops = {
	.open = myadc_open,
	.read = myadc_read,
	.release = myadc_close,
};

static int __init myadc_init(void)
{
	//1.ע���ַ��豸����
	major = register_chrdev(0,ADCNAME,&fops);
	if(major < 0){
		printk("register char device driver error\n");
		return major;
	}
	
	//2.adc��ַӳ��
	adc = (adc_t *)ioremap(ADC_PHY_BASE,20);
	if(adc == NULL){
		printk("adc addr ioreamp error\n");
		return -ENOMEM;
	}

	//3.adc�ĳ�ʼ��
	adc->ADCCON = 0;
	adc->ADCCON |= 6<<6;
	adc->PRESCALERCON = 0;
	adc->PRESCALERCON |= 149<<0;
	adc->PRESCALERCON |= (1<<15);

	//4.�Զ������豸�ڵ�
	cls = class_create(THIS_MODULE,"hello");
	if(IS_ERR(cls)){
		printk("class create error\n");
		return PTR_ERR(cls);
	}

	dev = device_create(cls,NULL,MKDEV(major,0),NULL,"myadc0");
	if(IS_ERR(dev)){
		printk("device create error\n");
		return PTR_ERR(dev);
	}

	return 0;
}
static void __exit myadc_exit(void)
{
	device_destroy(cls,MKDEV(major,0));
	class_destroy(cls);
	iounmap(adc);
	unregister_chrdev(major,ADCNAME);
}
module_init(myadc_init);
module_exit(myadc_exit);
MODULE_LICENSE("GPL");



