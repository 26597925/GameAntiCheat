#include "AntiWorker.h"
#include "Util.h"
#include "ProcessProtected.h"
#include "AntiArkTool.h"


//֪ͨӦ�ò��д�¼�
PKEVENT g_pReadAbleEvent; //Ӧ�ò������������ݵ�ͬ���¼�
//�����̵߳Ŀ��Ʊ���
KSPIN_LOCK g_spinWorkState;
BOOLEAN g_bWorkState = TRUE;
CLIENT_ID g_workClientID;

AntiCheatMsgQue g_outQue;

//�������߳�
VOID WorkerThread(IN PVOID StartContext)
{
	while (TRUE)
	{
		BOOLEAN bWorking = DirtyReadWorkState();
		if (!bWorking) break;
		if (!IsMsgQueEmpty(&g_outQue))
		{
			if (g_needProtectObj.uGameProcessID)
			{
				//֪ͨ3����ȡ����
				KeSetEvent(g_pReadAbleEvent, 0, FALSE);
			}
		}
		//debug port ����
		if (g_needProtectObj.uGameProcessID)
		{
			CleanDebugportByPID((HANDLE)g_needProtectObj.uGameProcessID);
			CleanTebDebugHandle();
			KdDisableDebugger();
			//KillArk();
			KdDisableDebugger();
		}

		Sleep(1000);
	}
	//�����������
	CleanMsgQue(&g_outQue);
	PsTerminateSystemThread(STATUS_SUCCESS);
}

VOID SetWorkState(BOOLEAN bWorkState)
{
	g_bWorkState = bWorkState;
}

BOOLEAN DirtyReadWorkState()
{
	BOOLEAN bRet = FALSE;
	KIRQL oldIrql;
	KeAcquireSpinLock(&g_spinWorkState, &oldIrql);
	bRet = g_bWorkState;
	KeReleaseSpinLock(&g_spinWorkState, oldIrql);
	return bRet;
}
