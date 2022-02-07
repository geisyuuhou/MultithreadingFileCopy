#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <Windows.h>
#include <thread>
#include <ctime>
#include <sys/stat.h>
#include "function.h"

using namespace std;

void FileScan(string base_dir, vector<string> *distBuf);
int CopySingleFile(string src_file, string dist_dir);
void CopyFileList(vector<string>* src_paths, string src_dir, string dist_dir, unsigned int start_index);
int file_count = 0, dir_count = 0;
int all = 0;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("MTCopy [src_dir] [dst_dir]");
		return -1;
	}
	string src_dir = argv[1];
	string dist_dir = argv[2];
	vector <string> *dist = new vector<string>;
	double start = clock();

	FileScan(src_dir, dist);
	all = dist->size();
	printf("总共有 %d 项, 正在复制...\n", all);
	vector <string>* dist1 = new vector<string>;
	dist1->assign(dist->begin(), dist->end());
	vector <string>* dist2 = new vector<string>;
	dist2->assign(dist->begin(), dist->end());
	vector <string>* dist3 = new vector<string>;
	dist3->assign(dist->begin(), dist->end());

	CopyFileList(dist, src_dir, dist_dir, 0);
	thread* t1 = new thread(CopyFileList, dist1, src_dir, dist_dir, 1);
	thread* t2 = new thread(CopyFileList, dist2, src_dir, dist_dir, 2);
	thread* t3 = new thread(CopyFileList, dist3, src_dir, dist_dir, 3);
	t1->join();
	t2->join();
	t3->join();
	
	delete dist;
	delete t1; delete t2; delete t3;
	double end = clock();
	printf("复制了 %d 个文件, %d 个空文件夹,共 %d 个, 用时%f秒\n",
		file_count,
		dir_count,
		(file_count + dir_count),
		(end - start) / 1000);
	
	return 0;
}

void FileScan(string base_dir, vector<string>* distBuf)
{
	intptr_t handle;
	_finddata_t *data = new _finddata_t;
	int layer_file_count = 0;
	handle = _findfirst((base_dir + "\\*.*").c_str(), data);
	if (handle == -1)
	{
		distBuf->push_back(base_dir);
		layer_file_count++;
	}
	else
	{
		layer_file_count++;
		while (_findnext(handle, data) != -1)
		{
			if (strcmp(data->name, ".") == 0 || strcmp(data->name, "..") == 0)
			{
				layer_file_count++;
				continue; //跳过 . 和 ..
			}
			else if (data->attrib == _A_SUBDIR)
			{
				layer_file_count++;
				FileScan(base_dir + "\\" + data->name, distBuf);
			}
			else
			{
				layer_file_count++;
				distBuf->push_back(base_dir + "\\" + data->name);
			}
		}
		if (layer_file_count <= 2)
		{
			distBuf->push_back(base_dir);
		}
		_findclose(handle);
	}
	delete data;
}

int CopySingleFile(string src_file, string dist_file)
{
	register struct stat s;
	if (stat(src_file.c_str(), &s) == 0)
	{
		//是文件夹，直接创建
		if (s.st_mode & S_IFDIR)
		{
			if (_mkdir(dist_file.c_str()) == -1)
			{
				printf("无法创建空文件夹: %s\n", dist_file.c_str());
			}
			else
			{
				dir_count++;
			}
		}
		else
		{
			//是文件，复制
			char tempDirPath[MAX_PATH] = { 0 };
			for (unsigned int i = 0; i < dist_file.size(); i++)
			{
				tempDirPath[i] = dist_file[i];
				if (tempDirPath[i] == '\\' || tempDirPath[i] == '/')
				{
					if (_access(tempDirPath, 0) == -1)
					{
						int ret = _mkdir(tempDirPath);
						if (ret == -1) return ret;
					}
				}
			}
			if (!CopyFile(src_file.c_str(), dist_file.c_str(), true))
			{
				printf("无法从 %s 复制到 %s\n", src_file.c_str(), dist_file.c_str());
			}
			else
			{
				file_count++;
			}
		}
		return 0;
	}
	else
	{
		printf("错误的文件: %s\n", src_file.c_str());
		return -1;
	}
}

void CopyFileList(vector<string>* src_paths, string src_dir, string dist_dir, unsigned int start_index)
{
	if (start_index > src_paths->size() - 1)
	{
		return;
	}
	else
	{
		for (unsigned int i = start_index; i < src_paths->size(); i += 4)
		{
			string temp = (*src_paths)[i];
			CopySingleFile(temp, replace_all_distinct((*src_paths)[i], src_dir, dist_dir));
		}
	}
}
