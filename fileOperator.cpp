#include "fileOperator.h"
#include <iostream>
#include <io.h>

fileOperator::fileOperator()
{
}


fileOperator::~fileOperator()
{
}

bool fileOperator::GetAllFiles(std::vector<std::string>& files, std::string path)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(files, p.assign(path).append("\\").append(fileinfo.name));
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
		return true;
	}
	return false;
}

bool fileOperator::GetAllFormatFiles(std::vector<std::string>& files, std::string path, std::string format)
{
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetAllFormatFiles(files, p.assign(path).append("\\").append(fileinfo.name), format);
				}

			}
			else if (fileinfo.attrib & _A_ARCH)
			{
				if (strstr(fileinfo.name, format.c_str()) != NULL)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
		return true;
	}
	return false;
}

bool fileOperator::GetDirAllFiles(std::vector<std::string>& files, std::string path)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;
	// \\* 代表要遍历所有的类型
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
		return false;

	do
	{
		//判断文件的属性是文件夹还是文件
		std::string  curpath = fileInfo.name;
		if (curpath != "." && curpath != "..")
		{
			curpath = path + "\\" + curpath;
			//文件夹中的文件
			if (fileInfo.attrib&_A_SUBDIR)
			{
				GetDirAllFiles(files, curpath);
			}
			else
			{
				files.push_back(curpath);
			}
		}
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return true;
}

bool fileOperator::GetDirFilesAndFolders(std::vector<std::string> &file, std::string path)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;
	//* 代表要遍历所有的类型
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
		return false;
	}
	do
	{
		//判断文件的属性是文件夹还是文件
		std::string  curpath = fileInfo.name;
		if (curpath != "." && curpath != "..")
		{
			curpath = path + "\\" + curpath;
			//变量文件夹中的png文件
			if (fileInfo.attrib&_A_SUBDIR)
			{
				//当前文件是目录
				file.push_back(curpath);
			}
			else
			{
				//进入某个子目录了
				file.push_back(curpath);
			}
		}
		//cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]") << endl;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return true;
}

bool fileOperator::findAllSubDir(std::vector<std::string> &res, std::string srcpath)
{
	//cout << "父目录  " << srcpath << endl;
	_finddata_t file;
	long lf;
	std::string pathName, exdName;
	//修改这里选择路径和要查找的文件类型  
	if ((lf = _findfirst(pathName.assign(srcpath).append("\\*").c_str(), &file)) == -1l)
	{
		return false;
	}
	else
	{
		do {
			std::string  curpath = file.name;
			if (curpath != "." && curpath != "..")
			{
				if (file.attrib == _A_NORMAL)
				{
					//cout << "  普通文件  ";
				}
				else if (file.attrib == _A_RDONLY)
				{
					//cout << "  只读文件  ";
				}
				else if (file.attrib == _A_HIDDEN)
				{
					//cout << "  隐藏文件  ";
				}
				else if (file.attrib == _A_SYSTEM)
				{
					//cout << "  系统文件  ";
				}
				else if (file.attrib == _A_SUBDIR)
				{
					//cout << "  子目录  ";
					curpath = srcpath + "\\" + curpath;
					//cout << curpath << endl;
					res.push_back(curpath);
					//变量子目录
					//cout << "         ";
					findAllSubDir(res, curpath);
				}
				else
				{
					//cout << "  存档文件  ";
					//cout << curpath << endl;
					res.push_back(curpath);
				}
			}
		} while (_findnext(lf, &file) == 0);
		//int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1  
	}
	_findclose(lf);
	return true;
}
