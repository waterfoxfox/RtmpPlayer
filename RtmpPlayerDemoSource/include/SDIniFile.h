//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：跨平台的ini配置文件读写实现
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#ifndef __CINI_FILE__
#define __CINI_FILE__
#include <string>
#include <vector>
using namespace std;

#define E_OK				0x00L
#define E_OPEN_FILE_FAILED	0x01L

/*
程序说明
1.从INI文件中读取参数
2.将参数写入INI格式文件
3.[sectin]必须第一个字符为'[',并以']'结束
4.';'后的字符串全部为注释
5.参数以name=value的方式表示，每一行只能设置一个参数，'='不能含有空格
6.没有对程序中注释做额外的处理，在写入文件时，没有考虑将注释写入INI文件
*/
//参数行
struct CIniEntry
{
	CIniEntry(){}
	CIniEntry(char* szName,char* szValue):m_strName(szName),m_strValue(szValue){}
	string m_strName;
	string m_strValue;
};
//注释行
struct CIniComment
{
	CIniComment(){}
	CIniComment(char* strCmt):m_strComment(strCmt){}
	string m_strComment;

};
//Section
struct CIniSection
{
	string m_isName;
	vector<CIniEntry> m_ieEntry;
	vector<CIniComment> m_icComment;
};

class CSDIniFile
{
public:
	CSDIniFile();
	~CSDIniFile();

public:
	//从INI文件中读取参数，如果执行完一次之后，不能重复执行，只有在调用ReleaseCache函数之后才可以进行第二次读入
	int ReadIniFile(char* szIniFile);
	//将参数写入INI文件
	int WriteIniFile(char* szIniFile = NULL);
	// 释放所有资源，可重新读取新的配置文件
	void ReleaseCache();
	BOOL IsOpen() { return strlen(m_strFileName) > 0 ? TRUE : FALSE; }

	int   SDGetProfileString(const char *section, const char *key,char *value, int size);
	int   SDGetProfileInt( const char *section, const char *key,int default_value);
	float SDGetProfileFloat( const char *section, const char *key,float default_value);

	int   SetProfileString(char *section, char *key,char *value);
	int   SetProfileInt(const char *section, const char *key,const int nvalue);
	int   SetProfileFloat(const char *section, const char *key,const float fvalue);

	void  DeleteSection(const char *section);
	void  DeleteKey(const char *section, const char *key);

private:
	//删除字符串两端的空格
	int mfTrim(char* &szString);
	//去除字符串中的注释
	int mfRemoveComment(char* szLine);
private:
	vector<CIniSection> m_isSection;
	char                m_strFileName[256];
	// 表示缓冲是否修改
	BYTE                m_bSavesign; 
};

void sd_WriteStringIniConfig(const char *strSection, const char *strKey, char *strValue);
void sd_WriteIntIniConfig(const char *strSection, const char *strKey, int nValue);
BOOL sd_ReadStringIniConfig(const char *strSection, const char *strKey, char *strValue, char *strDefaultValue = NULL);
BOOL sd_ReadIntIniConfig(const char *strSection, const char *strKey, int &nValue, int nDefaultValue = 0);

#endif
