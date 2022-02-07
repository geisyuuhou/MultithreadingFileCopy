# 四线程文件复制

### 多线程复制

对于大量零碎文件(文件多，但每个文件大小都较小)，本程序快于Windows的“复制粘贴”

### 注意事项

```C++
int main()
{
	string src_dir = "F:\\PyCharm";
	string dist_dir = "C:\\";
	......
}
```

主函数内填写内容：从src_dir复制到dist_dir

一定要使用绝对路径！！！