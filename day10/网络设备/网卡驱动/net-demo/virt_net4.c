#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/etherdevice.h>
#include <linux/types.h>

/*ʵ��˵����
 * �����ݵĽ����뷢��������û��ռ���Եģ�
 *��ʵ����net2�ļ����µĳ�����Ƚ����������MAC��ַ�Լ��������ݰ��ļ�����
 *ʵ�鲽�裺
 *��1��ifconfig,  �鿴��ǰ��������Ϣ��
 *��2��ifconfig vent0 3.3.3.3 ��������������IP��
 *��3��ifconfig�鿴��ǰ����������MAC��ַ�Լ��շ���������������
 *��4��PING 3.3.3.5���ݷ���
 *��5��ctrl + cֹͣ���ͣ�ifconfig�鿴��ǰ�������������ݷ��� ���������������
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
static int rx_packet(struct sk_buff *skb,struct net_device *dev)
{
	unsigned char * type;
	struct iphdr* ih;
	__be32 *saddr, *daddr, tmp;//ԴIP��Ŀ��IP��·��IP 
	unsigned char tmp_dev_addr[ETH_ALEN];
	struct ethhdr* ethhdr;

	struct sk_buff* rx_skb;

	/*��Ӳ������/��������*/
	/*�Ե� macͷ�е� Դ��Ŀ��,skb���ݰ���ʽ��MAC�� Ŀ��MAC+ԴMAC��+��������Ϣ��IPͷ��Ŀ��IP+ԴIP��+TYPE+����*/
	ethhdr = (struct ethhdr*)skb->data;//skb-��data�տ�ʼʱMAC��ַ��Ϣ
	memcpy(tmp_dev_addr, ethhdr->h_dest, ETH_ALEN);//��Ϊ��ʱ��Ҫ�����յ������ݰ����ͻ�ȥ������Ҫ��Ŀ��MAC��ԴMAC�Ե�λ�ã�
	memcpy(ethhdr->h_dest,ethhdr->h_source , ETH_ALEN);
	memcpy(ethhdr->h_source, tmp_dev_addr, ETH_ALEN);

	/*�Ե� ipͷ�е�Դ��Ŀ��*/
	ih = (struct iphdr*)(skb->data + sizeof(struct ethhdr));//SKB-��DATA + STRUCT ETHHDR ��ʱƫ�Ƶ�IP��ַ��
	saddr = &ih->saddr;
	daddr = &ih->daddr;                                                                                                                                                                                 

	tmp = *saddr;
	*saddr = *daddr;
	*daddr = tmp;

	type = skb->data + sizeof(struct ethhdr) + sizeof(struct iphdr);//SKB-��DATA   + SKB-��STRUCT ETHHDR + STRUCT IPHDR��ʱƫ�Ƶ�type
	*type = 0;  //0x80��ʾping�� 0��ʾreply

	ih->check = 0;//����У���룬ip��Ҫ����
	ih->check = ip_fast_csum((unsigned char*)ih, ih->ihl);
	//�˴��ο�LDD3   17.6
	//����һ��sk_buff
	rx_skb  = dev_alloc_skb(skb->len + 2);
	skb_reserve(rx_skb,2);//����ip��16B�߽�
	memcpy(skb_put(rx_skb, skb->len),skb->data, skb->len );

	rx_skb->dev = dev;
	rx_skb->protocol = eth_type_trans(rx_skb, dev);
	rx_skb->ip_summed = CHECKSUM_UNNECESSARY;
	//����ͳ����Ϣ
	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += skb->len;

	//�ύsk_buff
	netif_rx(rx_skb);
	
	//�ͷ�sk_buff
	return 0;
}
//����������ݵķ���
netdev_tx_t	net_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	/***********************************/
	//���Ͱ���������ʵ����������ݰ�ͨ����������
	netif_stop_queue(dev);//ֹͣ�������Ͷ���
	rx_packet(skb,dev);//����ظ���������һ���ٵķ�������
	dev_kfree_skb(skb);//�ͷ�SKB
    netif_wake_queue(dev);//��������е�����������Ѷ���
	/***********************************/
	count++;//���͵Ĵ���
	ndev->stats.tx_packets = count;//���㷢�Ͱ�������
	ndev->stats.tx_bytes += skb->len;//���㷢�͵��ֽ���
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
	//���������豸�ṹ��
	ndev = alloc_netdev(0,DEV_NAME,ether_setup);
	if(ndev == NULL){
		printk("ndev alloc fail!\n");
		return -ENOMEM;
	}
	ndev->netdev_ops = &net_ops;// ��䷽������
   //���������������ͨ��ping
    ndev->flags |= IFF_NOARP;
    ndev->features |= NETIF_F_V6_CSUM;
	//ע�������豸
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







