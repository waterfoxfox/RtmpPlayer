//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：跨平台的ini配置文件读写实现
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#include "SDCommon.h"
#include "SDIniFile.h"
#include "SDLog.h"
#include <fstream>
using namespace std;

#define MAX_COLS 260

#ifndef WIN32
#define MAX_PATH 256
#endif


CSDIniFile::CSDIniFile()
{
	memset(m_strFileName, 0, sizeof(m_strFileName));
	m_bSavesign = FALSE;
}

CSDIniFile::~CSDIniFile()
{
	ReleaseCache();
}

/*
功能
	读取INI文件
参数
	szIniFile	in	读入的INI文件名称
返回值
	E_OK				调用成功
	E_OPEN_FILE_FAILED	打开文件错我
*/
int CSDIniFile::ReadIniFile(char* szIniFile)
{

	if (szIniFile == NULL)
	{
		SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Point is NULL!");
		return E_OPEN_FILE_FAILED;
	}
	
	ifstream fIniFile(szIniFile);
	int len = (int)strlen(m_strFileName);
	if((!fIniFile.is_open()) || (len > 0))
	{
		//SDLOG_PRINTF("IniFile", SD_LOG_LEVEL_ERROR, "param is invalid:%d", fIniFile);
		return E_OPEN_FILE_FAILED;
	}
	char szLine[MAX_COLS] = {0};
	while (fIniFile.getline(szLine,MAX_COLS))
	{
		char* p = szLine;
		//是否为[]
		if( *p  == '[')
		{
			mfRemoveComment( p );
			char* pEnd = strchr( p ,']');
			if( pEnd == NULL)
				continue;
			*pEnd = 0;
			CIniSection is;
			is.m_isName = string( p + 1 );
			m_isSection.push_back(is);
			continue;
		}
		//是否为;
		mfTrim( p );
		if( *p == ';')
		{	
			if(m_isSection.size() > 0)
			{
				m_isSection[m_isSection.size() - 1].m_icComment.push_back(p + 1);
			}
			continue;
		}

		//空行过滤
		if (*p == 0)
		{
			continue;
		}
		
		//否则视为entry
		//p = szLine;
		char* pTemp = strchr( p,'=');
		if(pTemp == NULL)
		{
			SDLOG_PRINTF_U(0, SD_LOG_LEVEL_WARNING, "entry is invalid:%s", p);
			continue;
		}
		*pTemp = 0;
		//创建一个Entry
		CIniEntry ie;
		ie.m_strName =  p ;
		ie.m_strValue = pTemp + 1;
		//将Entry加入到响应的Section
		if(m_isSection.size() > 0)
		{
			m_isSection[m_isSection.size() - 1 ].m_ieEntry.push_back(ie);
		}
		memset(szLine,0,MAX_COLS);
	}
	fIniFile.close();
	strcpy(m_strFileName, szIniFile);
	m_bSavesign = FALSE;

	return E_OK;
}
/*
功能
	将CIniFile中的内容写入文件
参数
	szIniFile	in	生成的INI文件名称
返回值
	E_OK		调用成功
	E_OPEN_FILE_FAILED	打开文件错误
*/
int CSDIniFile::WriteIniFile(char* szIniFile)
{

	if (!m_bSavesign)
	{
		return E_OK;
	}
	
	if(szIniFile == NULL)
	{
		if (strlen(m_strFileName) < 1)
		{
			return E_OPEN_FILE_FAILED;
		}
		szIniFile = m_strFileName;
	}
	ofstream fIniFile(szIniFile);
	if (!fIniFile.is_open())
	{
		SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "fIniFile:%s open failed!", szIniFile);
		return E_OPEN_FILE_FAILED;
	}
	
	for (size_t i = 0; i < m_isSection.size();++i)
	{
		fIniFile.write("[",1);
		fIniFile.write(m_isSection[i].m_isName.c_str(),m_isSection[i].m_isName.length());
		fIniFile.write("]",1);
		fIniFile << endl;
		for (size_t j = 0; j < m_isSection[i].m_ieEntry.size();++ j)
		{
			fIniFile.write(m_isSection[i].m_ieEntry[j].m_strName.c_str(),m_isSection[i].m_ieEntry[j].m_strName.length());
			fIniFile.write("=",1);
			fIniFile.write(m_isSection[i].m_ieEntry[j].m_strValue.c_str(),m_isSection[i].m_ieEntry[j].m_strValue.length());
			fIniFile << endl;
		}
	}
	fIniFile.close();
	m_bSavesign = FALSE;

	return E_OK;
}

void CSDIniFile::ReleaseCache()
{

	m_bSavesign = FALSE;
	memset(m_strFileName, 0, sizeof(m_strFileName));
	m_isSection.clear();

}

/*
功能
	删除前后的空格（' ','\t','\r','\n'）
参数
	szString		in	传入的字符串
					out 去除空格后的字符串
返回值
	E_OK	调用成功
*/
int CSDIniFile::mfTrim(char* &szString)
{
	char* p = szString;
	while (*p == ' ' || *p == '\t')
	{
		p ++;
	}
	szString = p;

	p = szString + strlen(szString) - 1;
	while ( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
	{
		-- p;
	}
	*( p + 1 ) = 0;


	return E_OK;
}
/*
功能
	删除注释
参数
	szLine	in	传入的字符串
			out 删除注释后的字符串
返回值
	E_OK	调用成功
*/
int CSDIniFile::mfRemoveComment(char* szLine)
{
	char* p = strchr(szLine,';');
	if( p == NULL)
		return 0;
	*p = 0;
	return 0;
}


int   CSDIniFile::SDGetProfileString(const char *section, const char *key,char *value, int size)
{

	const char *lstrVal = NULL;
	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					lstrVal = m_isSection[i].m_ieEntry[y].m_strValue.c_str();
					if (lstrVal == NULL)
					{
						SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "value of:%s is NULL!", key);
						return 0;
					}
					
					if ((UINT)strlen(lstrVal)>=(UINT)size)
					{
						SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "buff is not long enough when call SDGetProfileString:%s!", lstrVal);
						memcpy(value, lstrVal, size-1);
						value[size] = '\0';
					}
					else if (strlen(lstrVal) > 0)
					{
						strcpy(value, lstrVal);
					}
					return (int)strlen(value);
				}
			}
		}
	}

	return 0;
}

int   CSDIniFile::SDGetProfileInt( const char *section, const char *key,int default_value)
{

	const char *lstrVal = NULL;
	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					lstrVal = m_isSection[i].m_ieEntry[y].m_strValue.c_str();
					if (lstrVal == NULL)
					{
						SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "value of:%s is NULL!", key);
						return default_value;
					}
					
					return atoi(lstrVal);
				}
			}
		}
	}

	return default_value;
}

float CSDIniFile::SDGetProfileFloat( const char *section, const char *key,float default_value)
{

	const char *lstrVal = NULL;
	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					lstrVal = m_isSection[i].m_ieEntry[y].m_strValue.c_str();
					if (lstrVal == NULL)
					{
						SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "value of:%s is NULL!", key);
						return default_value;
					}

					return (float)atof(lstrVal);
				}
			}
		}
	}

	return default_value;
}

int   CSDIniFile::SetProfileString(char *section, char *key,char *value)
{

	const char *lstrVal = NULL;

	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					lstrVal = m_isSection[i].m_ieEntry[y].m_strValue.c_str();
					if ((lstrVal ==  NULL) || (strcmp(lstrVal, value) != 0))
					{
						m_isSection[i].m_ieEntry[y].m_strValue = value;
						m_bSavesign = TRUE;
					}
					return E_OK;
				}
			}

			//创建一个Entry
			CIniEntry ie;
			ie.m_strName =  key;
			ie.m_strValue = value;
			m_isSection[i].m_ieEntry.push_back(ie);
			m_bSavesign = TRUE;
			return E_OK;
		}
	}
	CIniSection is;
	is.m_isName = string( section );
	m_isSection.push_back(is);

	CIniEntry ie;
	ie.m_strName =  key;
	ie.m_strValue = value;
	m_isSection[0].m_ieEntry.push_back(ie);
	m_bSavesign = TRUE;

	return E_OK;
}

int   CSDIniFile::SetProfileInt(const char *section, const char *key,const int nvalue)
{

	char strVal[256];
	const char *lstrVal = NULL;

	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					sprintf(strVal, "%d", nvalue);
					lstrVal = m_isSection[i].m_ieEntry[y].m_strValue.c_str();

					if ((lstrVal ==  NULL) || (strcmp(lstrVal, strVal) != 0))
					{
						m_isSection[i].m_ieEntry[y].m_strValue = strVal;
						m_bSavesign = TRUE;
					}
					return E_OK;
				}
			}

			//创建一个Entry
			CIniEntry ie;
			sprintf(strVal, "%d", nvalue);
			ie.m_strName =  key;
			ie.m_strValue = strVal;
			m_isSection[i].m_ieEntry.push_back(ie);
			m_bSavesign = TRUE;
			return E_OK;
		}
	}
	CIniSection is;
	is.m_isName = string( section );
	m_isSection.push_back(is);

	CIniEntry ie;
	sprintf(strVal, "%d", nvalue);
	ie.m_strName =  key;
	ie.m_strValue = strVal;
	m_isSection[0].m_ieEntry.push_back(ie);
	m_bSavesign = TRUE;

	return E_OK;
}

int   CSDIniFile::SetProfileFloat(const char *section, const char *key,const float fvalue)
{

	char strVal[256];
	for (size_t i = 0; i < m_isSection.size();++i)
	{
		if (strcmp(m_isSection[i].m_isName.c_str(), section) == 0)
		{
			for (UINT y=0; y<m_isSection[i].m_ieEntry.size(); y++)
			{
				if (strcmp(m_isSection[i].m_ieEntry[y].m_strName.c_str(), key) == 0)
				{
					sprintf(strVal, "%f", fvalue);

					if (strcmp(m_isSection[i].m_ieEntry[y].m_strValue.c_str(), strVal) != 0)
					{
						m_isSection[i].m_ieEntry[y].m_strValue = strVal;
						m_bSavesign = TRUE;
					}
					return E_OK;
				}
			}

			//创建一个Entry
			CIniEntry ie;
			sprintf(strVal, "%f", fvalue);
			ie.m_strName =  key;
			ie.m_strValue = strVal;
			m_isSection[i].m_ieEntry.push_back(ie);
			m_bSavesign = TRUE;
			return E_OK;
		}
	}
	CIniSection is;
	is.m_isName = string( section );
	m_isSection.push_back(is);

	CIniEntry ie;
	sprintf(strVal, "%f", fvalue);
	ie.m_strName =  key;
	ie.m_strValue = strVal;
	m_isSection[0].m_ieEntry.push_back(ie);
	m_bSavesign = TRUE;

	return E_OK;
}

void  CSDIniFile::DeleteSection(const char *section)
{

	vector<CIniSection>::iterator p;
	for (p=m_isSection.begin(); p != m_isSection.end(); p++ )
	{
		if (strcmp(p->m_isName.c_str(), section) == 0)
		{
			m_isSection.erase(p);
			m_bSavesign = TRUE;
			break;
		}
	}

}

void  CSDIniFile::DeleteKey(const char *section, const char *key)
{

	vector<CIniSection>::iterator p;
	for (p=m_isSection.begin(); p != m_isSection.end(); p++ )
	{
		if (strcmp(p->m_isName.c_str(), section) == 0)
		{
			vector<CIniEntry>::iterator p1;
			for (p1=p->m_ieEntry.begin(); p1 != p->m_ieEntry.end(); p1++)
			{
				if (strcmp(p1->m_strName.c_str(), key) == 0)
				{
					p->m_ieEntry.erase(p1);
					m_bSavesign = TRUE;
					return;
				}
			}
			break;
		}
	}

}

void sd_WriteStringIniConfig(const char *strSection, const char *strKey, char *strValue)
{
	char strIniFileName[MAX_PATH];
	GetSameExeFile((char*)strIniFileName, ".ini");
	CSDIniFile* pIniFile = new CSDIniFile();
	pIniFile->ReadIniFile(strIniFileName);
	pIniFile->SetProfileString((char*)strSection, (char*)strKey, strValue);
	pIniFile->WriteIniFile(strIniFileName);
	delete pIniFile;
	pIniFile = NULL;
}

void sd_WriteIntIniConfig(const char *strSection, const char *strKey, int nValue)
{
	char str[256];
	memset(str, 0x0, sizeof(str));
	sprintf(str, "%d", nValue);
	sd_WriteStringIniConfig(strSection, strKey, str);
}

BOOL sd_ReadStringIniConfig(const char *strSection, const char *strKey, char *strValue, char *strDefaultValue)
{
	int nRetLen = 0;
	char strIniFileName[MAX_PATH];
	GetSameExeFile((char*)strIniFileName, ".ini");
	CSDIniFile* pIniFile = new CSDIniFile();
	pIniFile->ReadIniFile(strIniFileName);
	*strValue = '\0';
	nRetLen = pIniFile->SDGetProfileString(strSection, strKey, strValue, 64);
	delete pIniFile;
	pIniFile = NULL;
	if (strlen(strValue) < 1 || nRetLen < 1)
	{
		if (strDefaultValue)
		{
			strcpy(strValue, strDefaultValue);
		}
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL sd_ReadIntIniConfig(const char *strSection, const char *strKey, int &nValue, int nDefaultValue)
{
	char strValue[64];
	char strIniFileName[MAX_PATH];
	GetSameExeFile((char*)strIniFileName, ".ini");
	CSDIniFile* pIniFile = new CSDIniFile();
	pIniFile->ReadIniFile(strIniFileName);
	memset(strValue, 0, sizeof(strValue));
	pIniFile->SDGetProfileString(strSection, strKey, strValue, 64);
	delete pIniFile;
	pIniFile = NULL;
	if (strlen(strValue) < 1)
	{
		nValue = nDefaultValue;
		return FALSE;
	}
	else
	{
		nValue = atoi(strValue);
		return TRUE;
	}
}
