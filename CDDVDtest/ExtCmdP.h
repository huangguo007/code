/************************************************************************/
/*	�t�@�C�����FCmdpst.h												*/
/*																		*/
/*	CommamndPassthough�pDLL�A�v���P�[�V������DLL���ʃw�b�_				*/
/*																		*/
/*	Copyright(C) TOSHIBA Corporation 2002. All right reserved.			*/
/*																		*/
/*		2002.04.15		(DME)		�V�K�쐬							*/
/************************************************************************/
#ifndef __PASSTEXT_H__
#define __PASSTEXT_H__

//#include <devioctl.h>

/************************************************************************/
/*	�萔��`															*/
/************************************************************************/
// �N��OS�̎��
#define				WIN9X_FAMILY			0x01					// Windows95,98,98SE,ME
#define				WIN2K_FAMILY			0x02					// Windows2000,XP

// �n���h�����ʗp�V�O�l�[�`��
#define				SIGNATURE				"PSTCMD  "				// �V�O�l�[�`���[�R�[�h

// �R�}���h�p�P�b�g
#define			 	CDB6					06						// 6�o�C�g�R�}���h
#define 			CDB10					10						// 10�o�C�g�R�}���h
#define 			CDB12					12						// 12�o�C�g�R�}���h

// �f�[�^�]���̕���
#define				DATA_OUT		        0						// �f�[�^�����o��
#define				DATA_IN					1						// �f�[�^�ǂݍ���
#define				DATA_NON				1						// �f�[�^�]���Ȃ�

// �^�C���A�E�g�l
#define				TIME_1S					1
#define				TIME_3S					3
#define				TIME_5S					5
#define				TIME_7S					7
#define				TIME_10S				10
#define				TIME_30S				30

// �f�o�C�X���
typedef struct _DEVICEINFO {
	UCHAR			DeviceType;										// �f�o�C�X�^�C�v
	UCHAR			VenderIdentify[8];								// �x���_�[��
	UCHAR			ProductIdentify[16];							// �v���_�N�g��
	UCHAR			Revision[4];									// ���r�W����
} DEVICEINFO, *PDEVICEINFO;

// �R�}���h���s�p�\����
typedef struct _USERPACKET {
	UCHAR			Packet[16];										// ���s����R�}���h
	UCHAR			PacketLength;									// �p�P�b�g�̒������w��
	PUCHAR			UserBuffer;										// ���[�U�[�o�b�t�@�ւ̃|�C���^
	ULONG			UserBufferLength;								// ���[�U�[�o�b�t�@�̒���
	ULONG			Timeout;										// �^�C���A�E�g�l���w��
	UCHAR			DataDirection;									// �f�[�^�̕���
	UCHAR			SenseData[24];									// �G���[���̏ڍ�
} USERPACKET,*PUSERPACKET;

// �G���[�R�[�h
#define				STATUS_SUCCESS			0x00000000				// ����I��
#define				ERROR_NOT_CONNECT		0x80000001				// �w�肳�ꂽ�f�o�C�X��������Ȃ�
#define				ERROR_HANDLE_INVALID	0x80000002				// �n���h�����Ⴄ�B
#define				ERROR_CMD_PACKET		0x80000021				// ���s�����R�}���h�ɃG���[������		
#define				ERROR_DEVIOCTL			0x800000FF				// �\�����ʃG���[

#define				ERROR_ASPI_DLL			0x80001000				// ASPI��DLL���ُ�

//#define				ERROR_HANDLE_INVALID	0x80000012				// �n���h�����Ⴄ�B
#define				ERROR_IO_CONTROL		0x80001000				// �R�}���h�G���[

#pragma warning(default:4200)
#endif

/*
** End-Of-File
*/
