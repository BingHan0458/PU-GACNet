import os
import sys

BASE_DIR = os.path.dirname(__file__)
sys.path.append(BASE_DIR)
sys.path.append(os.path.join(BASE_DIR, '../Common'))
sys.path.append(os.path.join(BASE_DIR, '../tf_ops'))
sys.path.append(os.path.join(BASE_DIR, "../tf_ops/grouping"))
from tf_grouping import knn_point_2
import tensorflow as tf
from GACnet_util import build_graph, graph_coarse, graph_attention_layer, graph_pooling_layer, point_upsample_layer, \
    crf_layer, graph_attention_layer_for_featurerefine
from ops import conv2d, conv1d
from configs import FLAGS


def get_edge_feature(xyz, k=16, idx=None):
    """ Builds a pyramid of graphs and pooling operations corresponding to progressively coarsened point cloud.
    Inputs:
        xyz: (batchsize, num_point, 1, nfeature)
        graph_inf: parameters for graph building (see configs.py)
    Outputs:
        graph_prd: graph pyramid contains the vertices and their edges at each layer
        coarse_map: record the corresponding relation between two close graph layers (for graph coarseing/pooling)
    """
    graph_prd = []
    graph = {}  # save subsampled points and their neighbor indexes at each level

    if idx is None:
        _, idx = knn_point_2(k + 1, xyz, xyz, unique=True, sort=True)
        idx = idx[:, :, 1:, :]
    graph['vertex'], graph['adjids'] = xyz, idx
    graph_prd.append(graph.copy())

    print("xyz: ", xyz.shape)  # (64, 256, 24)
    print("idx: ", idx.shape)  # (64, 256, 20, 2)

    # [N, P, K, Dim]
    point_cloud_neighbors = tf.gather_nd(xyz, idx)

    print("point_cloud_neighbors: ", point_cloud_neighbors.shape)  # (64, 256, 20, 24)

    point_cloud_central = tf.expand_dims(xyz, axis=-2)

    point_cloud_central = tf.tile(point_cloud_central, [1, 1, k, 1])

    edge_feature = tf.concat(
        [point_cloud_central, point_cloud_neighbors - point_cloud_central], axis=-1)

    return graph_prd, idx, edge_feature


def graph_attention_conv(feature, n=3, growth_rate=64, k=16, scope='dense_conv', is_training=True, bn_decay=None,
                         bn=False, ibn=False, **kwargs):
    with tf.variable_scope(scope, reuse=tf.AUTO_REUSE):
        graph, idx, y = get_edge_feature(feature, k=k, idx=None)  # [B N K 2*C]

        print("y: ", y.shape)  # (64, 256, 20, 48)
        print("idx: ", idx.shape)  # (64, 256, 20, 2)

        for i in range(n):
            if i == 0:
                # att_1_feature = graph_attention_layer(graph[0], y, [32, 32, 64], [64], is_training, bn_decay, scope, bn=True)
                # y = graph_attention_layer(graph[0], y, [32, 32, 64],
                #                                  [64], is_training,
                #                                  bn_decay, scope='attention_%d' % (i), bn=True)
                # y = tf.concat([
                #     conv2d(y, growth_rate, [1, 1], padding='VALID', scope='l%d' % i, **kwargs),
                #     tf.tile(tf.expand_dims(feature, axis=2), [1, 1, k, 1])], axis=-1)
                y = tf.concat([
                    conv2d(y, growth_rate, [1, 1], padding='VALID', scope='l%d' % i, **kwargs),
                    tf.tile(tf.expand_dims(feature, axis=2), [1, 1, k, 1])], axis=-1)
            elif i == n - 1:
                # att_2_feature = graph_attention_layer(graph[0], y, [64, 64, 128], [128], is_training, bn_decay, scope, bn=True)
                # y = graph_attention_layer(graph[0], y, [64, 64, 128],
                #                           [128], is_training,
                #                           bn_decay, scope='attention_%d' % (i), bn=True)
                y = tf.concat([
                    conv2d(y, growth_rate, [1, 1], padding='VALID', scope='l%d' % i, activation_fn=None, **kwargs),
                    y], axis=-1)
                # y = tf.concat([att_2_feature,y], axis=-1)
            else:
                # att_3_feature = graph_attention_layer(graph[0], y, [128, 128, 256], [256], is_training, bn_decay, scope, bn=True)
                # y = graph_attention_layer(graph[0], y, [128, 128, 256],
                #                           [256], is_training,
                #                           bn_decay, scope='attention_%d' % (i), bn=True)
                # y = tf.concat([att_3_feature, y], axis=-1)
                y = tf.concat([
                    conv2d(y, growth_rate, [1, 1], padding='VALID', scope='l%d' % i, activation_fn=None, **kwargs),
                    y], axis=-1)
        y = tf.reduce_max(y, axis=-2)
        return y, idx


# PU-GACNet: point cloud feature extraction module
def GAC_feature_extraction(inputs,
                           growth_rate=12,
                           dense_n=3,
                           knn=16,
                           scope='feature_extraction2', is_training=True, bn_decay=None):
    with tf.variable_scope(scope, reuse=tf.AUTO_REUSE):

        print(inputs.shape)  # (64, 256, 3)
        use_bn = False
        use_ibn = False

        comp = growth_rate * 2
        l0_features = tf.expand_dims(inputs, axis=2)
        print("l0_1: ", l0_features.shape)  # (64, 256, 1, 3)

        l0_features = conv2d(l0_features, 24, [1, 1],
                             padding='VALID', scope='layer0',
                             is_training=is_training, bn=use_bn, ibn=use_ibn,
                             bn_decay=bn_decay, activation_fn=None)
        l0_features = tf.squeeze(l0_features, axis=2)
        print("l0_2: ", l0_features.shape)  # (64, 256, 24)

        if FLAGS.k_in_gac == 1:

            # encoding layer-1
            l1_features, l1_idx = graph_attention_conv(l0_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                                       scope="layer1", is_training=is_training, bn=use_bn, ibn=use_ibn,
                                                       bn_decay=bn_decay)
            print("l1----: ", l1_features.shape)  # (64, 256, 60)
            l1_features = tf.concat([l1_features, l0_features], axis=-1)
            print("l1: ", l1_features.shape)  # (64, 256, 84)
            l1_features = tf.expand_dims(l1_features, axis=2)
            print("l1_1: ", l1_features.shape)  # (64, 256, 1, 84)
            return l1_features

        elif FLAGS.k_in_gac == 2:

            # encoding layer-1
            l1_features, l1_idx = graph_attention_conv(l0_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                                       scope="layer1", is_training=is_training, bn=use_bn, ibn=use_ibn,
                                                       bn_decay=bn_decay)
            print("l1----: ", l1_features.shape)  # (64, 256, 60)
            l1_features = tf.concat([l1_features, l0_features], axis=-1)
            print("l1: ", l1_features.shape)  # (64, 256, 84)

            # encoding layer-2
            l2_features = conv1d(l1_features, comp, 1,
                                 padding='VALID', scope='layer2_prep', is_training=is_training, bn=use_bn, ibn=use_ibn,
                                 bn_decay=bn_decay)
            l2_features, l2_idx = graph_attention_conv(l2_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                                       scope="layer2", is_training=is_training, bn=use_bn,
                                                       bn_decay=bn_decay)
            l2_features = tf.concat([l2_features, l1_features], axis=-1)
            print("l2: ", l2_features.shape)  # (64, 256, 144)
            l2_features = tf.expand_dims(l1_features, axis=2)
            print("l2_1: ", l2_features.shape)  # (64, 256, 1, 144)
            return l2_features

        elif FLAGS.k_in_gac == 3:

            # encoding layer-1
            l1_features, l1_idx = graph_attention_conv(l0_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                                       scope="layer1", is_training=is_training, bn=use_bn, ibn=use_ibn,
                                                       bn_decay=bn_decay)
            print("l1----: ", l1_features.shape)  # (64, 256, 60)
            l1_features = tf.concat([l1_features, l0_features], axis=-1)
            print("l1: ", l1_features.shape)  # (64, 256, 84)

            # encoding layer-2
            l2_features = conv1d(l1_features, comp, 1,
                                 padding='VALID', scope='layer2_prep', is_training=is_training, bn=use_bn, ibn=use_ibn,
                                 bn_decay=bn_decay)
            l2_features, l2_idx = graph_attention_conv(l2_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                                       scope="layer2", is_training=is_training, bn=use_bn,
                                                       bn_decay=bn_decay)
            l2_features = tf.concat([l2_features, l1_features], axis=-1)
            print("l2: ", l2_features.shape)  # (64, 256, 144)

            # encoding layer-3
            l3_features = conv1d(l2_features, comp, 1,
                                padding='VALID', scope='layer3_prep', is_training=is_training, bn=use_bn, ibn=use_ibn,
                                bn_decay=bn_decay)
            l3_features, l3_idx = graph_attention_conv(l3_features, growth_rate=growth_rate, n=dense_n, k=knn,
                                            scope="layer3", is_training=is_training, bn=use_bn, bn_decay=bn_decay)
            l3_features = tf.concat([l3_features, l2_features], axis=-1)

            print("l3: ", l3_features.shape)  # (64, 256, 204)
            l3_features = tf.expand_dims(l3_features, axis=2)
            print("l3_1: ", l3_features.shape)  # (64, 256, 1, 204)
            return l3_features

        else:
            raise Exception('Invalid parameter value k_in_gac! k_in_gac in [1,2,3]')
