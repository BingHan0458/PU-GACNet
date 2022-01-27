# PU-GACNet: Graph Attention Convolution Network for Point Cloud Upsampling
This is the official implementation for paper [PU-GACNet: Graph Attention Convolution Network for Point Cloud Upsampling](https://doi.org/10.1016/j.imavis.2021.104371)

PU-GAC repo supports training our PU-GACNet, and previous methods [PU-Net](https://openaccess.thecvf.com/content_cvpr_2018/papers/Yu_PU-Net_Point_Cloud_CVPR_2018_paper.pdf), [MPU (3PU)](https://openaccess.thecvf.com/content_CVPR_2019/papers/Yifan_Patch-Based_Progressive_3D_Point_Set_Upsampling_CVPR_2019_paper.pdf), [PU-GAN](https://openaccess.thecvf.com/content_ICCV_2019/papers/Li_PU-GAN_A_Point_Cloud_Upsampling_Adversarial_Network_ICCV_2019_paper.pdf), [Dis-PU](https://openaccess.thecvf.com/content/CVPR2021/papers/Li_Point_Cloud_Upsampling_via_Disentangled_Refinement_CVPR_2021_paper.pdf), [PU-GCN](https://openaccess.thecvf.com/content/CVPR2021/papers/Qian_PU-GCN_Point_Cloud_Upsampling_Using_Graph_Convolutional_Networks_CVPR_2021_paper.pdf). Please kindly cite all of the methods. 

 
### Installation
This repository is based on Tensorflow (1.13.1) and the TF operators from PointNet++. Therefore, you need to install tensorflow and compile the TF operators. 

You can check the `env_install.sh` for details how to install the environment. In the second step, for compiling TF operators, please check `compile.sh` and `tf_compile.sh` in `tf_ops` folder, one has to manually change the path!!. 


### Usage

1. Clone the repository:

   ```shell
   https://github.com/BingHan0458/PU-GACNet
   cd PU-GAC
   ```
   
2. install the environment
   Once you have modified the path in `compile.sh` and `tf_compile.sh` under `tf_ops`, you can simply install `pugac` environment by:
   
   ```bash
   source env_install.sh
   conda activate pugac
   ```
   
3. Download PU1K dataset from [Google Drive](https://drive.google.com/drive/folders/1k1AR_oklkupP8Ssw6gOrIve0CmXJaSH3?usp=sharing)  

    You need place it into PU-GAC/data/PU1K.
    
    The directory tree of the data file is as follows:
    ```markdown
    data
       |__PU1K
           |__train
               |__pu1k_poisson_256_poisson_1024_pc_2500_patch50_addpugan.h5
           |__test
               |__original_meshes
                   |__*.off
               |__input_2048
                   |__*.xyz
               |__gt_8192
                   |__*.xyz
       |__realscan_KITTI
           |__0000001.xyz
           |__...
    ```
    
    Since PU1K benchmark data is used, no data preprocessing operations are required. If you want to use your own data set, 
    you can refer to `data_preprocessing/prep_data` for data processing.
    
4. Train models
   -  PU-GACNet
   ```shell
   python main.py --phase train --model pugac --upsampler edge-aware_nodeshuffle --k 20
   ```
    
   -  PU-GCN
   ```shell
   python main.py --phase train --model pugcn --upsampler nodeshuffle --k 20
   ```
   
   -  PU-Net
   ```shell
   python main.py --phase train --model punet --upsampler multi_cnn
   ```
   
   -  mpu
   ```shell
   python main.py --phase train --model mpu --upsampler duplicate
   ```

   -  PU-GAN
   ```shell
   python main.py --phase train --model pugan --more_up 2
   ```
   
4. Evaluate models:  
    Before testing, please copy the corresponding model to the `pretrain `folder. Then run the scripts `test_pu1k_allmodels.sh`.
    
   ```shell
   source test_pu1k_allmodels.sh # please look this file and `test_pu1k.sh` for details
   ```

5. Test on real-scanned dataset

    Before testing, please copy the corresponding model to the `pretrain `folder. Then run the scripts `test_realscan_allmodels.sh`.
    
   ```shell
    source test_realscan_allmodels.sh # please look this file and `test_realscan.sh` for details
    ```

6. Visualization. 

   You can use meshlab or cloudcompare software for visualization. Furthermore, mayavi is also a good choice.
   
    
## Citation

If PU-GACNet and the repo are useful for your research, please consider citing:

	@inproceedings{Yu2018Pu,
	  author  = "Lequan Yu and Xianzhi Li and Chi-Wing Fu and Daniel Cohen-Or and Pheng-Ann Heng.",
	  year    = 2018,
	  title   = "{Pu-net: Point cloud upsampling network}",
	  booktitle="Proceedings of IEEE Conference on Computer Vision and Pattern Recognition {(CVPR-18)}", 
	  pages   = "2790-2799",
	}

	@inproceedings{Wang2019Patch,
	  author  = "Yifan Wang and Shihao Wu and Hui Huang and Daniel Cohen-Or and Olga Sorkine-Hornung.",
	  year    = 2019,
	  title   = "{Patch-based progressive 3d point set upsampling}",
	  booktitle="Proceedings of IEEE Conference on Computer Vision and Pattern Recognition {(CVPR-19)}", 
	  pages   = "5958-5967",
	}

	@inproceedings{Li2019Pu,
	  author  = "Ruihui Li and Xianzhi Li and Chi-Wing Fu and Daniel Cohen-Or and Pheng-Ann Heng.",
	  year    = 2019,
	  title   = "{Pu-gan: A point cloud upsampling adversarial network}",
	  booktitle="Proceedings of IEEE International Conference on Computer Vision {(ICCV-19)}", 
	  pages   = "7203-7212",
	}

	@inproceedings{Li2021Point,
	  author  = "Ruihui Li and Xianzhi Li and PhengAnn Heng and ChiWing Fu.",
	  year    = 2021,
	  title   = "{Point cloud upsampling via disentangled refinement}",
	  booktitle="Proceedings of IEEE Conference on Computer Vision and Pattern Recognition {(CVPR-21)}", 
	  pages   = "344-353",
	}

	@inproceedings{Qian2021Pu,
	  author  = "Guocheng Qian and Abdulellah Abualshour and Guohao Li and Ali Thabet and Bernard Ghanem.",
	  year    = 2021,
	  title   = "{Pu-gcn: Point cloud upsampling using graph convolutional networks}",
	  booktitle="Proceedings of IEEE Conference on Computer Vision and Pattern Recognition {(CVPR-21)}", 
	  pages   = "11683-11692",
	}


    
### Acknowledgement
****

