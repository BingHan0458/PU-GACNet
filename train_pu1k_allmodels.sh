#!/usr/bin/env bash

## train single model on PU1K benchmark
# train PU-GACNet
python main.py --phase train --model pugac --upsampler edge-aware_nodeshuffle --k 20
# train PU-Net
python main.py --phase train --model punet --upsampler multi_cnn
# train MPU
python main.py --phase train --model mpu --upsampler duplicate
# train PU-GAN
python main.py --phase train --model pugan --more_up 2
# train PU-GCN
python main.py --phase train --model pugcn --upsampler nodeshuffle --k 20

## ablation for feature expansion module
# punet with ENS
python main.py --phase train --model punet --upsampler edge-aware_nodeshuffle
# mpu with ENS
python main.py --phase train --model mpu --upsampler edge-aware_nodeshuffle
# pugcn with ENS
python main.py --phase train --model pugcn --upsampler edge-aware_nodeshuffle --k 20
# PU-GACNet with duplicate
python main.py --phase train --model pugac --upsampler duplicate --k 20
# PU-GACNet with nodeshuffle
python main.py --phase train --model pugac --upsampler nodeshuffle --k 20

## ablation for layers of GAC module in feature extraction module
python main.py --phase train --model pugac --upsampler edge-aware_nodeshuffle --k 20
python main.py --phase train --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 2
python main.py --phase train --model pugac --upsampler edge-aware_nodeshuffle --k 20 --k_in_gac 3