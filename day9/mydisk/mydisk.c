#include <linux/init.h>
#include <linux/module.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>

#define BLKNAME "mydisk"
#define BLKSIZE (1*1024*1024) //1M
struct gendisk *mydisk;
int major = 0;
struct request_queue *q;
spinlock_t lock;

int mydisk_open(struct block_device *blkdev, fmode_t mod)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

int mydisk_close(struct gendisk *gendisk, fmode_t mod)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

int mydisk_getgeo(struct block_device *blkdev, struct hd_geometry *hd)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	//��ͷ���ŵ��������ĸ���
	return 0;

}

const struct block_device_operations fops = {
	.open = mydisk_open,
	.getgeo =mydisk_getgeo,
	.release = mydisk_close,

};

void parse_request_fn(struct request_queue *q)
{
	//��д���еĽ���
}

static int __init mydisk_init(void)
{
	//1.Ϊmydisk�����ڴ沢��ɱ�Ҫ�ĳ�ʼ��
	mydisk = alloc_disk(4); //�����4�ǿ��Է��ĸ�����
	if(mydisk == NULL){
		printk("alloc disk error\n");
		return -ENOMEM;
	}

	//2.�������豸��
	major = register_blkdev(0,BLKNAME);
	if(major < 0){
		printk("request block deivce number error\n");
		return major;
	}

	
	//3.��ʼ������
	spin_lock_init(&lock);
	q = blk_init_queue(parse_request_fn,&lock);
	if(q == NULL){
		printk("blk queue init error\n");
		return -EAGAIN;
	}

	//4.gendisk�ṹ���Ա�ĳ�ʼ��
	mydisk->major = major;
	mydisk->first_minor = 0;
	strcpy(mydisk->disk_name,BLKNAME);
	mydisk->fops = &fops;
	mydisk->queue = q;
	set_capacity(mydisk,BLKSIZE/512);

	//5.���ڴ��Ϸ���1M��С�ռ䵱�ɴ�����ʹ��


	//6.ע��
	add_disk(mydisk);
	
	return 0;
}

static void __exit mydisk_exit(void)
{
	del_gendisk(mydisk);
	blk_cleanup_queue(q);
	unregister_blkdev(major,BLKNAME);
	put_disk(mydisk);
}

module_init(mydisk_init);
module_exit(mydisk_exit);
MODULE_LICENSE("GPL");



