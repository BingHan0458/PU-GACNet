#!/usr/bin/env bash
conda activate pugac

# RealScan Test on KITTI 3D object detection benchmark velodyne
# Test PUNet
source test_realscan.sh pretrain/punet-multi_cnn/ ./data/realscan_KITTI/ --model punet --upsampler multi_cnn
# Test PUGCN
source test_realscan.sh pretrain/pugcn-nodeshuffle/ ./data/realscan_KITTI/ --model pugcn --upsampler nodeshuffle --k 20
# Test MPU
source test_realscan.sh pretrain/mpu-duplicate/  ./data/realscan_KITTI/ --model mpu --upsampler duplicate
# Test PUGAC
source test_realscan.sh pretrain/pugac-ens/ ./data/realscan_KITTI/ --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 1
# Test PU-GAN
source test_realscan.sh pretrain/pugan/ ./data/realscan_KITTI/ --model pugan --more_up 2
# Test Dis-PU
# Here dis-pu is not supported. Please test it by using its source code.
