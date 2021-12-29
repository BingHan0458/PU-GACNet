# This script has passed the test on Windows 10.
import open3d as o3d
import os
import re
from pathlib import Path
import time
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
    # path为off模型的路径
    path = 'F:\\data\\PU1K\\PU1K_raw_meshes\\test\\test_meshes'
    fType = '.off'
    res = getfiles(path, fType)
    print('提取结果：')
    print(len(res))
    # Format to open3d usable objects
    s_time = time.time()
    for i in range(len(res)):
        input = res[i]
        stem = Path(input).stem
        print("Processing: "+input)
        mesh = o3d.io.read_triangle_mesh(input)
        # the point num of Poisson disk sampling
        number_of_points = 8192
        # print(o3d.__version__)  # 0.13.0
        sampling_pc = o3d.geometry.TriangleMesh.sample_points_poisson_disk(mesh, number_of_points, 5)
        o3d.io.write_point_cloud("F:\\data\\PU1K\\PU1K_raw_meshes\\test\\gt_8192\\"+stem+".xyz",sampling_pc)
    e_time = time.time()
    print("Finished! Time Cost: ", e_time-s_time, "s")
