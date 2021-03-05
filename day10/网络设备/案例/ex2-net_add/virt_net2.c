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
 *6.ping 3.3.3.4----->���������������÷��ͺ���
 *7.rmmod *----->ɾ��ģ�飬����remove-->stop[dmesg]
 */
static int count;       //�������ݴ������
//���������豸�ṹ��
struct net_device *vnet = NULL;

//��
int	net_open(struct net_device *dev)
{
	printk("%s%s%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
//ֹͣ
int	net_stop(struct net_device *dev)
{
	printk("%s%s%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
//����
netdev_tx_t hard_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	printk("%s%s%d,count:%d\n",__FILE__,__func__,__LINE__,count++);
	return NETDEV_TX_OK;
}
//����
int net_ioctl(struct net_device *dev,struct ifreq *ifr, int cmd)
{
	printk("%s%s%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
//�����豸������������
struct net_device_ops netdev_ops = {
	.ndo_open = net_open,
	.ndo_stop = net_stop,
	.ndo_start_xmit = hard_start_xmit,
	.ndo_do_ioctl = net_ioctl,

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




