from eig_feature import FPS
import numpy as np
import pdb
from plyfile import PlyData, PlyElement


def read_xyzrgbIL_ply(filename):
    """ read XYZ point cloud from filename PLY file """
    plydata = PlyData.read(filename)
    pc = plydata['vertex'].data
    # print(pc)
    pc_array = np.array([[x, y, z] for x, y, z in pc])
    return pc_array


if __name__=='__main__':
    datafilename = './test/ant.ply'
    org_data = read_xyzrgbIL_ply(datafilename)
    
    xyz = org_data[:,0:3]
    #pdb.set_trace()
    idxs = FPS(xyz, 1000)
    new_xyz = xyz[idxs,...]
    # pdb.set_trace()
    np.savetxt('./test/ant.txt', new_xyz)
