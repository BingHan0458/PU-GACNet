# This script has passed the test on Windows 10.

from pathlib import Path
import os
import re
import numpy as np
import pyvista as pv
import vtk


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


def split_mesh(selected_grid):
    """Adds a new mesh to the plotter each time cells are picked, and
    removes them from the original mesh"""

    # if nothing selected
    if not selected_grid.n_cells:
        return

    # remove the picked cells from main grid
    ghost_cells = np.zeros(grid.n_cells, np.uint8)
    ghost_cells[selected_grid['orig_extract_id']] = 1
    grid.cell_arrays[vtk.vtkDataSetAttributes.GhostArrayName()] = ghost_cells
    grid.RemoveGhostCells()

    # add the selected mesh this to the main plotter
    color = np.random.random(3)
    legend.append(['picked mesh %d' % len(picked), color])
    pl.add_mesh(selected_grid, color=color)
    pl.add_legend(legend)

    # track the picked meshes and label them
    selected_grid['picked_index'] = np.ones(selected_grid.n_points)*len(picked)
    picked.append(selected_grid)


if __name__ == "__main__":
    fileList = []
    # path为off模型的路径
    path = 'F:\\data\\PU1K\\PU1K_raw_meshes\\train\\train_meshes'
    out_path = 'F:\\data\\PU1K\\PU1K_raw_meshes\\train\\train_meshes_patch10\\'
    fType = '.off'
    res = getfiles(path, fType)
    print('提取结果：')
    print(len(res))
    for i in range(len(res)):
        ip = res[i]
        stem = Path(ip).stem
        print("Processing: " + ip)
        # 加载off模型
        off = pv.read(ip)
        grid = off.cast_to_unstructured_grid()
        pl = pv.Plotter()
        picked = []
        legend = []
        pl.add_mesh(grid, color='w')
        # enable cell picking with our custom callback
        pl.enable_cell_picking(mesh=grid, callback=split_mesh, show=False)
        pl.show()
        # convert these meshes back to surface meshes (PolyData)
        meshes = []
        for selected_grid in picked:
            meshes.append(selected_grid.extract_surface())
        # plot final separated meshes for fun
        # pv.plot(meshes)
        # save these meshes somewhere
        for i, mesh in enumerate(meshes):
            mesh.save(out_path+stem+"_%03d.stl" % i)
        print("Finished: " + ip)

