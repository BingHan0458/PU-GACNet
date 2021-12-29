import tensorflow as tf
import os
import sys
import logging
import shutil
import numpy as np

os.environ["CUDA_VISIBLE_DEVICES"] = "0"

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(BASE_DIR, "Upsampling"))
from model import Model
from configs import FLAGS, configure_logger


def run():
    if FLAGS.phase == 'train':
        FLAGS.train_file = "./data/PU1K/train/pu1k_poisson_256_poisson_1024_pc_2500_patch50_addpugan.h5"
        logging.info('train_file: {}'.format(FLAGS.train_file))
    else:
        FLAGS.test_data = os.path.join(FLAGS.data_dir, '*.xyz')
        FLAGS.out_folder = os.path.join("evaluation_code/result")
        if os.path.exists(FLAGS.out_folder):
            shutil.rmtree(FLAGS.out_folder)
        os.makedirs(FLAGS.out_folder)
        logging.info('test_data: {}'.format(FLAGS.test_data))
        logging.info('checkpoints:'.format(FLAGS.log_dir))

    logging.info('loading config: \n {} \n'.format(FLAGS))
    # open session
    run_config = tf.ConfigProto()
    run_config.gpu_options.allow_growth = True
    with tf.Session(config=run_config) as sess:
        model = Model(FLAGS, sess)
        if FLAGS.phase == 'train':
            model.train()
        else:
            model.test()


def main(unused_argv):
    run()


if __name__ == '__main__':
    tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)
    np.random.seed(FLAGS.seed)
    tf.set_random_seed(FLAGS.seed)
    logging.info('setting random seed to: {}'.format(FLAGS.seed))
    tf.app.run()
