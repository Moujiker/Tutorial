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
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
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
	// \\* ����Ҫ�������е�����
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
		return false;

	do
	{
		//�ж��ļ����������ļ��л����ļ�
		std::string  curpath = fileInfo.name;
		if (curpath != "." && curpath != "..")
		{
			curpath = path + "\\" + curpath;
			//�ļ����е��ļ�
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
	//* ����Ҫ�������е�����
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
		return false;
	}
	do
	{
		//�ж��ļ����������ļ��л����ļ�
		std::string  curpath = fileInfo.name;
		if (curpath != "." && curpath != "..")
		{
			curpath = path + "\\" + curpath;
			//�����ļ����е�png�ļ�
			if (fileInfo.attrib&_A_SUBDIR)
			{
				//��ǰ�ļ���Ŀ¼
				file.push_back(curpath);
			}
			else
			{
				//����ĳ����Ŀ¼��
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
	//cout << "��Ŀ¼  " << srcpath << endl;
	_finddata_t file;
	long lf;
	std::string pathName, exdName;
	//�޸�����ѡ��·����Ҫ���ҵ��ļ�����  
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
					//cout << "  ��ͨ�ļ�  ";
				}
				else if (file.attrib == _A_RDONLY)
				{
					//cout << "  ֻ���ļ�  ";
				}
				else if (file.attrib == _A_HIDDEN)
				{
					//cout << "  �����ļ�  ";
				}
				else if (file.attrib == _A_SYSTEM)
				{
					//cout << "  ϵͳ�ļ�  ";
				}
				else if (file.attrib == _A_SUBDIR)
				{
					//cout << "  ��Ŀ¼  ";
					curpath = srcpath + "\\" + curpath;
					//cout << curpath << endl;
					res.push_back(curpath);
					//������Ŀ¼
					//cout << "         ";
					findAllSubDir(res, curpath);
				}
				else
				{
					//cout << "  �浵�ļ�  ";
					//cout << curpath << endl;
					res.push_back(curpath);
				}
			}
		} while (_findnext(lf, &file) == 0);
		//int __cdecl _findnext(long, struct _finddata_t *);����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1  
	}
	_findclose(lf);
	return true;
}
