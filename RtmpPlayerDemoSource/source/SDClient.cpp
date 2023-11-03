//***************************************************************************//
//* ��Ȩ����  www.mediapro.cc
//*
//* ����ժҪ���ͻ��˶�����.
//*	
//* ��ǰ�汾��V1.0		
//* ��    �ߣ�mediapro
//* ������ڣ�2018-6-20
//**************************************************************************//
#include "SDClient.h"
#include "SDLog.h"
#include <time.h>

extern UINT g_unJitterBuffTime;
extern BOOL g_bKeepRatio;



CSDClient::CSDClient(const char* strRtmpUrl, void* pWindowHandle)
{
	m_bClosed = TRUE;

	//SDK
	m_pTerminal = NULL;
	
	strcpy(m_strRtmpUrl, strRtmpUrl);
	m_pWindowHandle = pWindowHandle;
}

CSDClient::~CSDClient()
{
	if (m_pTerminal)
	{
		RtmpPlayer_Delete(m_pTerminal);
		m_pTerminal = NULL;
	}
}


BOOL CSDClient::Start()
{

	m_pTerminal = RtmpPlayer_Create();

	BOOL bRet = RtmpPlayer_Start(m_pTerminal, m_strRtmpUrl, g_unJitterBuffTime, m_pWindowHandle);
	if (bRet == FALSE)
	{
		RtmpPlayer_Delete(m_pTerminal);
		m_pTerminal = NULL;
		return FALSE;
	}

	RtmpPlayer_SetDisplayMode(m_pTerminal, g_bKeepRatio == TRUE ? 0:1);

	m_bClosed = FALSE;
	return TRUE;

}


void CSDClient::Stop()
{
	m_bClosed = TRUE;

	if (m_pTerminal)
	{
		RtmpPlayer_Stop(m_pTerminal);
	}
}




