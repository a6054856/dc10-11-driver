

#define wait_event_interruptible(wq, condition)				
({									
	int __ret = 0;	
	//�����˷���ֵ�ı���
	
	if (!(condition))						
		__wait_event_interruptible(wq, condition, __ret);	
	//�������û��׼���þͻ�ִ��
	//__wait_event_interruptible������������
	//Ӳ���������Ѿ�׼�����ˣ��������
	//�ͻᱻ������

	__ret;	
	//����ֵ���ɹ�����0��ʧ�ܷ��ش�����							\
})


#define __wait_event_interruptible(wq, condition, ret)			
	do {									
		DEFINE_WAIT(__wait);						
		/*
		wait_queue_t __wait = {						
			.private	= current,	
			//current���ǻ�ȡ��ǰ���̵�task_struct�ṹ��
			.func		= function, 	
			//����ʱ�õ��Ļ��Ѻ���
			.task_list	= LIST_HEAD_INIT((name).task_list), 
			//�ں˵�����
		}
		*/
		//����ȴ����������ʼ���ȴ�������

		
		for (;;) {							
			prepare_to_wait(&wq, &__wait, TASK_INTERRUPTIBLE);	
			//���ȴ���������뵽�ȴ�����ͷ��
			//����ʶ���̵�״̬Ϊ���жϵĵȴ�̬

			if (condition)	//�������׼�����ˣ���					
				break;		//�˳�ѭ��				

			if (!signal_pending(current)) { 	
			//�ж��Ƿ����źŹ�����
			//������źŵ�����signal_pending������
			//������źŵ��˱�ʾ����û��׼����
			//��������һ��������
			//�����ǰû���źŵ��ˣ����if��Ϊ��
			//����schedule������ߡ�
				schedule(); 			
			//schedule�������������cpu�ĺ���
				continue;					
			}							
			ret = -ERESTARTSYS; 				
			break;	

			
		}							
		finish_wait(&wq, &__wait);	
		//�����̵�״̬�����޸�Ϊ���е�״̬
		//�����ȴ�������ӵȴ�����ͷ��ɾ��
	} while (0)




	

