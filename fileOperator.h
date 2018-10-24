#pragma once
//windows system file operator class
//reference to: http://www.cnblogs.com/adong7639/p/3983359.html
//#include "dirent.h"

#include <string>
#include <vector>

class fileOperator
{
public:
	fileOperator();
	~fileOperator();

	//��ȡ���е��ļ���:������Ŀ¼����Ŀ¼�е��ļ�
	static bool GetAllFiles(std::vector<std::string>& files, std::string path);
	//��ȡ�ض���ʽ���ļ���
	static bool GetAllFormatFiles(std::vector<std::string>& files, std::string path, std::string format);
	//������Ŀ¼,��ȡ�����ļ�(�������ļ���)
	static bool GetDirAllFiles(std::vector<std::string>& files, std::string path);
	//�ҳ���ǰĿ¼�е��ļ����ļ���
	static bool GetDirFilesAndFolders(std::vector<std::string> &file, std::string path);
	//����Ŀ¼�µ�������Ŀ¼
	static bool findAllSubDir(std::vector<std::string> &res, std::string path);
};

