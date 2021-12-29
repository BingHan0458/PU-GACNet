#!/usr/bin/env bash

# Step0: install Anaconda3
cd ~/
wget https://repo.anaconda.com/archive/Anaconda3-2019.07-Linux-x86_64.sh
bash Anaconda3-2019.07-Linux-x86_64.sh
source ~/.bashrc

# Step1: install pugac environment
conda remove --name pugac --all
conda create -n pugac python=3.7.3 cudatoolkit=10.0 cudnn numpy=1.16.4
conda activate pugac
pip install matplotlib tensorflow-gpu==1.13.1 open3d==0.9 sklearn Pillow gdown plyfile vtk==8.2.0 mayavi==4.7.1+vtk82 opencv-python==4.3.0.36
# please do not install tensorflow gpu by conda. It may effect the following compiling.

# Step2: compile tf_ops (this is the hardest part and a lot of people encounter different problems here.)
# you may need some compiling help from here: https://github.com/yulequan/PU-Net
cd ./tf_ops
# Please remember to change the path and cuda in the tf_ops file!
source compile.sh # please look this file into detail if it does not work
source tf_compile.sh # please look this file into detail if it does not work
cd ../

# Step 3 Optional (compile evaluation code)
# install CGAL
sudo apt-get install libcgal-dev
cd evaluation_code
cmake .
make
./evaluation Icosahedron.off Icosahedron.xyz
