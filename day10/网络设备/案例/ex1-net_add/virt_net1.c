#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/netdevice.h>

#define NET_NAME "vnet"
/* ʹ��˵����
 *1.insmod *.ko
 *2.ifconfig----->�鿴ԭ����������Ϣ
 *3.ifconfig vnet 3.3.3.3---->����vnet��ip,(��ʱ���Զ�����open[dmesg])
 *4.ifconfig---->�鿴���ú��������Ϣ
 *5.ping 3.3.3.3---->ping������ʱ��û�н���������
 *6.ping 3.3.3.4----->����������,��Ϊû��ʵ�ַ��ͺ������ʴ�ʱ�ᵼ�³���
 */
//���������豸�ṹ��
struct net_device *vnet = NULL;
//�����豸������������[����д�����������Ϸ������]
struct net_device_ops netdev_ops = {
};
//ģ���ʼ��
static int __init net_init(void)//������ʼ��
{
	printk("%s%s%d\n",__FILE__,__func__,__LINE__);
	//��������ṹ��
	vnet = alloc_netdev(0,NET_NAME,ether_setup);
	if(vnet==NULL){
		printk("alloc netdev fail!\n");
		return -ENOMEM;
	}
	vnet->netdev_ops = &netdev_ops;
	register_netdev(vnet);//ע�������豸
	return 0;
}
//ģ���˳�
static void __exit net_exit(void)//�����˳�
{
	printk("%s%s%d\n",__FILE__,__func__,__LINE__);
	unregister_netdev(vnet);//ע�������豸
	free_netdev(vnet);// �ͷ������豸
}

module_init(net_init);//ע��
module_exit(net_exit);//ע��
MODULE_LICENSE("GPL");//���




