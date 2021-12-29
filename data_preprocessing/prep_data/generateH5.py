import argparse
import numpy as np
import h5py
import os
import re

"""
Create an HDF5 file of point cloud for training GACNet model.
"""

parser = argparse.ArgumentParser()
parser.add_argument('--train_dir', default='', help="your path of train/poisson_256")
parser.add_argument('--val_dir', default='', help="your path of train/poisson_1024")
parser.add_argument('--output_file', default='', help="the output path of .h5")
parser.add_argument('--include_val', type=int, default=1)
args = parser.parse_args()


# 获取特定类型的文件
def getfiles(dirPath, fileType):
    fileList = []
    files = os.listdir(dirPath)
    pattern = re.compile(".*\\" + fileType)  # 设置正则表达式，后缀为fileType
    for f in files:
        # 如果是文件夹，递归调用getfile
        if os.path.isdir(dirPath + '/' + f):
            getfiles(dirPath + '/' + f, fileType)
        #  如果是文件，看是否为所需类型
        elif os.path.isfile(dirPath + '/' + f):
            matches = pattern.match(f)  # 判断f的文件名是否符合正则表达式，即是否为off后缀
            if matches is not None:
                fileList.append(dirPath + '/' + matches.group())
        else:
            fileList.append(dirPath + '/无效文件')

    return fileList


def add_data(h5_file, pc_dir, prefix, shape):
    # Make a list of all pc in the source directory
    fType = '.xyz'
    res = getfiles(pc_dir, fType)
    print(res)
    # 定义h5文件的数据形状和类型
    # datatype = np.zeros((2, pc_num, 3), dtype=np.float32)
    # 读取点云文件，解析XYZ坐标
    all_data = []
    for i in range(len(res)):
        pc_file = open(res[i], 'r+', encoding='utf8')
        lines = [line.rstrip() for line in pc_file]
        nd = []
        # print(lines)
        for j in range(len(lines)):
            d = [float(k) for k in lines[j].split(' ')]
            nd.append(d)
        # print(nd)
        all_data.append(nd)
        # 所有点云文件个数相同，所以无需进行随机采样
        # slice = random.sample(lines, 2018)
        # 写数据
    print(all_data)
    print(np.array(all_data).shape)
    h5_file.create_dataset(prefix, shape, np.float32)
    for j in range(len(all_data)):
        h5_file[prefix][j, ...] = all_data[j]


if __name__ == '__main__':
    # fileList = []

    with h5py.File(args.output_file, 'w') as f:
        add_data(f, args.train_dir, 'poisson_256', (28720, 256, 3))

        if args.include_val != 0:
            add_data(f, args.val_dir, 'poisson_1024', (28720, 1024, 3))
    f.close()
    print("generate pu1k_poisson_256_poisson_1024_patch5.h5 successfully!")
