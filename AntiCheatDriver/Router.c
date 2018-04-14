#include "Router.h"
#include "AntiCheatDriver.h"
#include "AntiWorker.h"
#include "ProcessProtected.h"

extern int GetMsgSize(int nMsgNo);
//��ʼ���¼����
BOOLEAN HandleInitEvent(send_read_able_event_to_driver *pMsg);
//��ʼ����Ҫ�������̺߳ͽ���id
BOOLEAN HandleNeedProtectedThreadAndProcess(send_need_protected_process_to_driver *pMsg);

//��Ϣ·��
BOOLEAN Router(char* buff, int nSize)
{
	int *pMsgNo = (int*)(buff);
	int nMsgSize = GetMsgSize(*pMsgNo);
	buff += sizeof(int);
	if (*pMsgNo > 0 && nSize - (int)sizeof(int) == nMsgSize)
	{
		switch (*pMsgNo)
		{
		case SEND_READ_ABLE_EVENT_HANDLE:
			return HandleInitEvent((PVOID)buff);
		case SEND_NEED_PROTECTED_THREAD_PROCESS:
			return HandleNeedProtectedThreadAndProcess((PVOID)buff);
		default:
			break;
		}
		return TRUE;
	}
	
	return FALSE;
}

BOOLEAN HandleInitEvent(send_read_able_event_to_driver *pMsg)
{
	NTSTATUS status;
	status = ObReferenceObjectByHandle(
		(HANDLE)pMsg->event_handle,
		EVENT_MODIFY_STATE,
		*ExEventObjectType,
		KernelMode,
		(PVOID*)&g_pReadAbleEvent, NULL);
	if (status == STATUS_SUCCESS)
	{
		//���������߳�
		HANDLE hWorkerThread = NULL;
		PsCreateSystemThread(&hWorkerThread, 0, NULL, NULL, &g_workClientID, WorkerThread, NULL);
	}
	return status == STATUS_SUCCESS;
}

BOOLEAN HandleNeedProtectedThreadAndProcess(send_need_protected_process_to_driver *pMsg)
{
	SetProcessProtected(pMsg->rcv_msg_thread_id, pMsg->rcv_msg_thread_id, pMsg->process_id);
	return TRUE;
}
