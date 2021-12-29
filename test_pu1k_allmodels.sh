#!/usr/bin/env bash

## test single module on PU1K benchmark
# test 1-GAC PU-GACNet
source test_pu1k.sh pretrain/pugac-ens/ 0 --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 1
# test PUGCN
source test_pu1k.sh pretrain/pugcn-nodeshuffle/ 0 --model pugcn --upsampler nodeshuffle --k 20
# test PUNet
source test_pu1k.sh pretrain/punet-multi_cnn/ 0 --model punet --upsampler multi_cnn
# test MPU
source test_pu1k.sh pretrain/mpu-duplicate/ 0 --model mpu --upsampler duplicate
# test Dis-PU
# Here dis-pu is not supported. Please test it by using its source code.

## ablation for feature expansion module
# test PU-GACNet with duplicate
source test_pu1k.sh pretrain/pugac-duplicate/ 0 --model pugac --upsampler duplicate --k 20 --k_in_gac 1
# test PU-GACNet with nodeshuffle
source test_pu1k.sh pretrain/pugac-nodeshuffle/ 0 --model pugac --upsampler nodeshuffle --k 20 --k_in_gac 1
# test PUGCN with ENS
source test_pu1k.sh pretrain/pugcn-ens/ 0 --model pugcn --upsampler edge-aware_nodeshuffle --k 20
# test PUNet with ENS
source test_pu1k.sh pretrain/punet-ens/ 0 --model punet --upsampler edge-aware_nodeshuffle
# test MPU with ENS
source test_pu1k.sh pretrain/mpu-ens/ 0 --model mpu --upsampler edge-aware_nodeshuffle

## ablation for layers of GAC module on feature extraction module
# 1-GAC
source test_pu1k.sh pretrain/pugac-ens/ 0 --model pugac --upsampler edge-aware_nodeshuffle --k 20
# 2-GACs
source test_pu1k.sh pretrain/pugac-2-ens/ 0 --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 2
# 3-GACs
source test_pu1k.sh pretrain/pugac-3-ens/ 0 --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 3
