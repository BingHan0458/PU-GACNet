#!/usr/bin/env Python
# coding=utf-8

# This script has passed the test on Windows 10.

import os
import re

fileList = []


# 获取特定类型的文件
def getfiles(dirPath, fileType):
    files = os.listdir(dirPath)
    pattern = re.compile(".*\\" + fileType)  # 设置正则表达式，后缀为fileType
    for f in files:
        # 如果是文件夹，递归调用getfile
        if os.path.isdir(dirPath + '\\' + f):
            getfiles(dirPath + '\\' + f, fileType)
        #  如果是文件，看是否为所需类型
        elif os.path.isfile(dirPath + '\\' + f):
            matches = pattern.match(f)  # 判断f的文件名是否符合正则表达式，即是否为off后缀
            if matches is not None:
                fileList.append(dirPath + '\\' + matches.group())
        else:
            fileList.append(dirPath + '\\无效文件')

    return fileList


if __name__ == "__main__":
    # path为CAD模型的路径
    path = 'F:\\data\\Modelnet40\\modelnet40_manually_aligned'
    fType = '.off'
    res = getfiles(path, fType)
    print('提取结果：')
    print(len(res))
    # print(res)
    os.chdir(r"E:\\meshlab")  # 切换到meshlabserver.exe所在目录

    for f in res:
        print(f)
        ipath = f  # 输入off模型的路径
        opath = f[0:-3] + "xyz"  # 输出点云xyz模型的保存路径
        print(opath)
        # 保存顶点法线
        # os.system('meshlabserver -i ' + ipath + ' -o ' + opath + ' -m vn')
        # 只保存点云
        os.system('meshlabserver -i ' + ipath + ' -o ' + opath)
        os.system('exit()')  # 退出meshlabserver.exe
