#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <Windows.h>
#include <thread>
#include "function.h"

using namespace std;

void FileScan(string base_dir, vector<string> *distBuf);
int CopySingleFile(string src_file, string dist_dir);
void CopyFileList(vector<string>* src_paths, string src_dir, string dist_dir, int start_index);

int main()
{
	
	string src_dir = "F:\\PyCharm";
	string dist_dir = "C:\\";
	vector <string> *dist = new vector<string>;
	FileScan(src_dir, dist);
	vector <string>* dist1 = new vector<string>;
	dist1->assign(dist->begin(), dist->end());
	vector <string>* dist2 = new vector<string>;
	dist2->assign(dist->begin(), dist->end());
	vector <string>* dist3 = new vector<string>;
	dist3->assign(dist->begin(), dist->end());
	CopyFileList(dist, src_dir, dist_dir, 0);
	thread* t2 = new thread(CopyFileList, dist1, src_dir, dist_dir, 1);
	thread* t3 = new thread(CopyFileList, dist2, src_dir, dist_dir, 2);
	thread* t4 = new thread(CopyFileList, dist3, src_dir, dist_dir, 3);
	//t1->join();
	t2->join();
	t3->join();
	t4->join();
	
	delete dist;
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
	if (CopyFile(src_file.c_str(), dist_file.c_str(), true) == 0)
	{
		return _mkdir(tempDirPath); //空文件夹仍然创建
	}
	return 1;
}

void CopyFileList(vector<string>* src_paths, string src_dir, string dist_dir, int start_index)
{
	if (start_index > src_paths->size() - 1)
	{
		return;
	}
	else
	{
		for (int i = start_index; i < src_paths->size(); i += 4)
		{
			string temp = (*src_paths)[i];
			CopySingleFile(temp, replace_all_distinct((*src_paths)[i], src_dir, dist_dir));
		}
	}
}
