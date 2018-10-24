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

	//获取所有的文件名:包括子目录及子目录中的文件
	static bool GetAllFiles(std::vector<std::string>& files, std::string path);
	//获取特定格式的文件名
	static bool GetAllFormatFiles(std::vector<std::string>& files, std::string path, std::string format);
	//遍历子目录,获取所有文件(不包含文件夹)
	static bool GetDirAllFiles(std::vector<std::string>& files, std::string path);
	//找出当前目录中的文件和文件夹
	static bool GetDirFilesAndFolders(std::vector<std::string> &file, std::string path);
	//查找目录下的所有子目录
	static bool findAllSubDir(std::vector<std::string> &res, std::string path);
};

