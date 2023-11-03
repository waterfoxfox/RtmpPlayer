//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：控制台的命令行接口类.
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//

#if !defined(SDCONSOLEIFACE_H)
#define SDCONSOLEIFACE_H

/*
 * 查看当前支持的命令
 * 查看某个命令的帮助
 *    1，支持帮助
 *    2，支持命令解析
 *    3，支持命令动态扩展
 */

#include "SDCommon.h"
#include "SDIniFile.h"


#define  CMD_NOT_EXSIT_COMMAND     "Does not exist command!!!"
#define  CMD_SYNTAX_ERROR          "Syntax error in parameters!!!"
#define  CMD_NOT_SUPPORT_COMMAND   "Does not support command!!!"


typedef struct stServerBaseRunPar ServerBaseRunPar;
class CSDConsleIFace  
{
public:
	CSDConsleIFace();
	virtual ~CSDConsleIFace();
	
public:
	// 系统命令行系统注册命令类，告诉系统自己有命令可以执行
	static void Register (CSDConsleIFace *pIFace);

	// 系统命令行命令注销类
	static void UnRegister (CSDConsleIFace *pIFace);

	// 进入本地控制台方式的命令行系统接口, strCommandPrompt为提示符
	static int  RunCommandLine(const char *strCommandPrompt);

	//获取命令行名字
	char  *GetCmdName() { return m_strCmd; }

	//设置命令行名字
	void SetCmdName(const char *strName);

public:
	// 需要子类实现的命令帮助接口
	virtual void Help() = 0;
	// 需要子类实现的命令解释接口，返回TRUE表示解释成功，否则失败
	virtual int Interpreter(char *strCmd) = 0;
	
private:
	// 字符串解释接口，FALSE为解释失败，否则成功
	static int  CmdInterpreter(char *strCmd);
	// 查找是否为当前命令
	int   Equal(char *strName);

	
private:
	char   m_strCmd[64];
};

#endif // !defined(SDCONSOLEIFACE_H)