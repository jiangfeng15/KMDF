#include <ntddk.h>

#include "clt_code.h"

#define INITCODE code_seg("INIT");
#define PAGECODE code_seg("PAGE");

NTSTATUS CreateMyDevice(IN PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status;
	UNICODE_STRING devName;  //�豸����
	UNICODE_STRING sysLinkName;  //ϵͳ����������
	PDEVICE_OBJECT pDevObject;    //���ڷ��ش����豸
	
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevObj");
	status = IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevObject);
	if (!NT_SUCCESS(status))
	{
		if (status == STATUS_INSUFFICIENT_RESOURCES)
		{
			KdPrint(("��Դ����\n"));
		}
		if (status == STATUS_OBJECT_NAME_EXISTS)
		{
			KdPrint(("ָ������������\n"));
		}
		if (status == STATUS_OBJECT_NAME_COLLISION)
		{
			KdPrint(("�������г�ͻ"));
		}
		return status;
	}
	KdPrint(("�豸�����ɹ�\n"));
	pDevObject->Flags |= DO_BUFFERED_IO; //��������ʽ��д
	RtlInitUnicodeString(&sysLinkName, L"\\??\\MySysData");
	IoDeleteSymbolicLink(&sysLinkName);
	status = IoCreateSymbolicLink(&sysLinkName, &devName); //�ж����ɷ��������Ƿ�ɹ�
	if (!NT_SUCCESS(status))
	{
		KdPrint(("���ɷ�������ʧ��\n"));
		IoDeleteDevice(pDevObject);
		return status;
	}
	KdPrint(("���ɷ������ӳɹ�"));

	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	PDEVICE_OBJECT pDevObject;
	UNICODE_STRING sysLinkName;

	pDevObject = pDriverObject->DeviceObject;
	IoDeleteDevice(pDevObject);	//ȡ���豸��ɾ��
	KdPrint(("�ɹ�ɾ���豸\n"));

	RtlInitUnicodeString(&sysLinkName, L"\\??\\MySysData");
	IoDeleteSymbolicLink(&sysLinkName);	//ȡ�÷������Ӳ�ɾ��
	KdPrint(("�ɹ�ɾ����������\n"));

	KdPrint(("�����ɹ�ж��\n"));
}


//NTSTATUS MyDispatchRoutine(IN PDEVICE_OBJECT pDevobj, IN PIRP pIrp)
//{
//	return STATUS_SUCCESS;
//}


NTSTATUS MyDispatchRoutine(IN PDEVICE_OBJECT pDevobj, IN PIRP pIrp)
{
	ULONG info;
	PIO_STACK_LOCATION psl = IoGetCurrentIrpStackLocation(pIrp);

	switch (psl->MajorFunction)
	{
		case IRP_MJ_CREATE:
			KdPrint(("exe Create file"));
			break;
		case IRP_MJ_CLOSE:
			KdPrint(("exe Close file"));
			break;
		case IRP_MJ_READ:
			break;
		case IRP_MJ_WRITE:
			break;
		case IRP_MJ_DEVICE_CONTROL:
		{
			//�ж��Ƿ�Ϊ32λ����
			if (IoIs32bitProcess(pIrp))
			{
				KdPrint(("����Ӧ��Ϊ 32 λ"));
			}
			else
			{
				KdPrint(("����Ӧ��Ϊ 64 λ"));
			}
			NTSTATUS status = STATUS_SUCCESS;
			ULONG cbin = psl->Parameters.DeviceIoControl.InputBufferLength; //��ȡ���뻺������С
			ULONG cbout = psl->Parameters.DeviceIoControl.OutputBufferLength; //��ȡ�����������С
			ULONG code = psl->Parameters.DeviceIoControl.IoControlCode; //�õ�IOCTL
			KdPrint(("Enter IRP_MJ_DEVICE_CONTROL\n"));
			switch (code)
			{
			case add_code:
			{
				int a, b, r;
				int * inputBuffer = (int*)pIrp->AssociatedIrp.SystemBuffer;
				int * outBuffer = (int *)pIrp->AssociatedIrp.SystemBuffer;
				KdPrint(("Enter add_code\n"));
				a = *inputBuffer;
				b = *(inputBuffer + 1);
				//_asm {							//��෽ʽȡ��a,bֵ,�ȼ���a = *inputBuffer; b = *(inputBuffer+1);
				//	mov eax, inputBuffer
				//	mov ebx, [eax]
				//	mov a, ebx
				//	mov ebx, [eax + 4]
				//	mov b, ebx
				//}
				KdPrint(("a=%d, b=%d\n", a, b));
				r = a + b;
				*outBuffer = r;
				//_asm {							//��෽ʽ����rֵ,�ȼ���*inputBuffer = r;
				//	mov eax, r
				//	mov ebx, outputBuffer
				//	mov[ebx], eax
				//}
				KdPrint(("a+b=%d\n", r));
				info = 4;
				break;
			}
			case sub_code:
			{
				break;
			}

			}
		}
		default:
		{
			KdPrint(("��������"));
			break;
		}
	}

	pIrp->IoStatus.Information = info;			//���ò������ֽ���Ϊ0��������ʵ������
	pIrp->IoStatus.Status = STATUS_SUCCESS;		//���سɹ�
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);	//ָʾ��ɴ�IRP
	KdPrint(("�뿪��ǲ����\n"));				//������Ϣ
	return STATUS_SUCCESS;
}
#pragma INITCODE
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING reg_path)
{
	KdPrint(("�������سɹ�\n"));
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = MyDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = MyDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = MyDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDispatchRoutine;
	CreateMyDevice(pDriverObject);
	pDriverObject->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}