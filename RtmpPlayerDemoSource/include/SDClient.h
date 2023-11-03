//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：客户端对象类.
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2018-6-20
//**************************************************************************//

#if !defined(_SDCLIENT_H)
#define _SDCLIENT_H

#include "SDCommon.h"
#include "RtmpPlayerSdk.h"


class CSDClient
{
public:
	CSDClient(const char* strRtmpUrl, void* pWindowHandle);
	virtual ~CSDClient();

public:
	BOOL Start();
	void Stop();


private:
	char					m_strRtmpUrl[1024];
	void*					m_pWindowHandle;
	void*					m_pTerminal;
	BOOL					m_bClosed;

};

#endif // !defined(_SDCLIENT_H)
