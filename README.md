# 四线程文件复制

### 多线程复制

对于大量位于不同物理磁盘间零碎文件(文件多，但每个文件大小都较小)，本程序一般来说快于Windows的“复制粘贴”

### 注意事项

```C++
MTCopy.exe src_dir dst_dir
```

命令行调用：从src_dir复制到dist_dir

路径格式应统一为绝对路径或相对路径
在相同物理磁盘直接可能由于I\O限制不会显示出优势来
