#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
/*ʵ��˵����
 *��ʵ����net2�ļ����µĳ�����Ƚ����������MAC��ַ�Լ��������ݰ��ļ�����
 *ʵ�鲽�裺
 *��1��ifconfig,  �鿴��ǰ��������Ϣ��
 *��2��ifconfig vent0 3.3.3.3 ��������������IP��
 *��3��ifconfig�鿴��ǰ����������MAC��ַ�Լ��շ���������������
 *��4��PING 3.3.3.5���ݷ���
 *��5��ctrl + cֹͣ���ͣ�ifconfig�鿴��ǰ�������������ݷ��������
 *��6) rmmod virt_net1, ж��ģ�飻��ʱ�鿴�ں˴�ӡ��Ϣ������stop;
 */
#define DEV_NAME "vnet0"		//�����豸����
static struct net_device *ndev = NULL; //�����豸�ṹ�嶨��
static int count = 0;
//����Ӧ����������
int	net_open(struct net_device *dev)
{
	ndev->dev_addr[0] = 00;	/*���������MAC��ַ*/
	ndev->dev_addr[1] = 01;
	ndev->dev_addr[2] = 02;
	ndev->dev_addr[3] = 03;
	ndev->dev_addr[4] = 04;
	ndev->dev_addr[5] = 05;
	printk("%s%d\n",__func__,__LINE__);
	return 0;
}
//�ر���Ӧ����������
int	net_stop(struct net_device *dev)
{
	printk("%s%d\n",__func__,__LINE__);
	return 0;
}
//����������ݵķ���
netdev_tx_t	net_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	count++;
	ndev->stats.tx_packets = count;
	ndev->stats.tx_bytes += skb->len;
	printk("%s%d,count:%d\n",__func__,__LINE__,count);
	return NETDEV_TX_OK;
}
//�����豸������������
struct net_device_ops net_ops = {
	.ndo_open = net_open,
	.ndo_stop = net_stop,
	.ndo_start_xmit = net_start_xmit,
};

//ģ���ʼ��
static int __init netdev_init(void)
{
	ndev = alloc_netdev(0,DEV_NAME,ether_setup);
	if(ndev == NULL){
		printk("ndev alloc fail!\n");
		return -ENOMEM;
	}
	ndev->netdev_ops = &net_ops;
	register_netdev(ndev);
	printk("net_init ok !\n");
	return 0;
}
//ģ���˳���ע����ע���豸֮�����ͷſռ䣬����ж�ػ����
static void __exit netdev_exit(void)
{
	unregister_netdev(ndev);//ע�������豸
	free_netdev(ndev);		//�ͷ�ndev�Ŀռ�
	printk("net_exit ok !\n");
}

module_init(netdev_init);
module_exit(netdev_exit);
MODULE_LICENSE("GPL");







