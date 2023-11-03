//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：控制台的命令行接口类.
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#include "SDConsoleIFace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static vector<CSDConsleIFace*>     lConsleMap;


CSDConsleIFace::CSDConsleIFace()
{
}

CSDConsleIFace::~CSDConsleIFace()
{
	
}

int CSDConsleIFace::Equal(char *strName) 
{ 
	return !strcmp(m_strCmd, strName); 
}

//设置命令行名字
void CSDConsleIFace::SetCmdName(const char *strCmd)
{
	if (strlen(strCmd) >= 64)
	{
		memcpy(m_strCmd, strCmd, 64);
		m_strCmd[63] = '\0';
	}
	else
	{
		strcpy(m_strCmd, strCmd);
	}
}


void CSDConsleIFace::Register(CSDConsleIFace *pIFace)
{
	vector<CSDConsleIFace*>::iterator p;
	for (p=lConsleMap.begin(); p != lConsleMap.end(); p++ )
	{
		if (*p == pIFace)
		{
			return;
		}
	}

	lConsleMap.push_back(pIFace);

}

void CSDConsleIFace::UnRegister(CSDConsleIFace *pIFace)
{

	vector<CSDConsleIFace*>::iterator p;
	for (p=lConsleMap.begin(); p != lConsleMap.end(); p++ )
	{
		if (*p == pIFace)
		{
			lConsleMap.erase(p);
			return;
		}
	}

}

int CSDConsleIFace::CmdInterpreter(char *strCmd)
{

	char *strParlist = NULL;
	char strCmdTag[64];
	vector<CSDConsleIFace*>::iterator p;
    BYTE unIndex = 0;
		
	if (strlen(strCmd) < 1)
	{
		return TRUE;
	}

	while(unIndex < 64 && strCmd[unIndex] != ' ' && strCmd[unIndex] != '\0')
	{
		strCmdTag[unIndex] = strCmd[unIndex];
		unIndex++;
	}
	strCmdTag[unIndex] = '\0';

	if (unIndex > 0 && unIndex < 64)
	{
		if (strcmp(strCmdTag, "help") == 0 || strcmp(strCmdTag, "?") == 0)
		{
			strParlist = strCmd+unIndex;
			while (*strParlist == ' ') strParlist++;
			if (*strParlist == '\0')
			{
				for (p=lConsleMap.begin(); p != lConsleMap.end(); p++ )
				{
					printf("\n\n********* %s *********\n", (*p)->GetCmdName());
					(*p)->Help();
				}
				return TRUE;
			}
			else
			{
				unIndex = 0;
				while (strParlist[unIndex] != '\0')
				{
					if ( !((strParlist[unIndex] >= 'a' && strParlist[unIndex] <= 'z')
						 || (strParlist[unIndex] >= 'A' && strParlist[unIndex] <= 'Z')
						 || (strParlist[unIndex] >= '0' && strParlist[unIndex] <= '9')) )
					{
						printf(CMD_SYNTAX_ERROR"\n");
						return FALSE;
					}
					unIndex++;
				}

				for (p=lConsleMap.begin(); p != lConsleMap.end(); p++ )
				{
					if ((*p)->Equal(strParlist))
					{
						(*p)->Help();
						return TRUE;
					}
				}
			}
		}
		else
		{
			for (p=lConsleMap.begin(); p != lConsleMap.end(); p++ )
			{
				if ((*p)->Equal(strCmdTag))
				{
					return (*p)->Interpreter(strCmd + unIndex);
				}
			}
		}
	}
	
	printf(CMD_NOT_EXSIT_COMMAND"\n");

	return FALSE;
}


int CSDConsleIFace::RunCommandLine(const char *strCommandPrompt)
{
	int   nRet = 0;
	char  strcmd[128];
	char  strShowCmdPrompt[32];

	// 提取命令行提示符
	while (nRet < 30)
	{
		if ( (strCommandPrompt[nRet] >= 'a' && strCommandPrompt[nRet] <= 'z')
			|| (strCommandPrompt[nRet] >= 'A' && strCommandPrompt[nRet] <= 'Z')
			|| (strCommandPrompt[nRet] >= '0' && strCommandPrompt[nRet] <= '9'))
		{
			strShowCmdPrompt[nRet] = strCommandPrompt[nRet];
		}
		else
		{
			break;
		}
		nRet++;
	}
	strShowCmdPrompt[nRet] = '/';
	strShowCmdPrompt[nRet+1] = '\0';

	while (1)
	{
		memset(strcmd, 0, 128);
		printf("%s", strShowCmdPrompt);

		if ( fgets(strcmd, sizeof(strcmd), stdin) != NULL )
		{
			//过滤空格
			int i = 0;
			BOOL bEmpty = TRUE;
			for (i = 0; i < (int)strlen(strcmd); i++)
			{
				if ((strcmd[i] != ' ') && (strcmd[i] != '\n'))
				{
					bEmpty = FALSE;
					break;
				}
			}

			if (bEmpty == TRUE)
			{
				continue;
			}

			for (i = 0; i < (int)strlen(strcmd); i++)
			{
				if (strcmd[i] == '\n')
				{
					strcmd[i] = 0;
				}
			}

			// 解释命令
			if (strcmp(strcmd, "exit") == 0)
			{
				return TRUE;
			}
			CSDConsleIFace::CmdInterpreter(strcmd);
		}
		else
		{
			break;
		}
	}

	return FALSE;
}

