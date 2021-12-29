#/bin/bash
TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')
TF_LIB=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_lib())')
/usr/local/cuda-10.0/bin/nvcc cuda_ulits.cu -o cuda_ulits.cu.o -c -O2 -DGOOGLE_CUDA=1 -x cu -Xcompiler -fPIC # O2(是O2，不是02)
g++ -std=c++11 tf_sampling.cpp tf_gather.cpp tf_interpolate.cpp cuda_ulits.cu.o -o tf_op_so.so -shared -fPIC -I $TF_INC/external/nsync/public/ -I $TF_INC -I /usr/local/cuda-10.0/include -lcudart -L /usr/local/cuda-10.0/lib64/ -L$TF_LIB -ltensorflow_framework -O2 #-D_GLIBCXX_USE_CXX11_ABI=0
