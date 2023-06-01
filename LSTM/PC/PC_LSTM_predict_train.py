#coding=gbk
"""
This program is part of the teaching materials for teacher Hao Xiaoli's experimental class of BJTU.

Copyright ? 2021 HAO xiaoli and Yang jian.
All rights reserved.
"""

import os
os.environ["CUDA_VISIBLE_DEVICES"] = "0" # ע����import keras/tensorflow֮ǰ
import keras.backend.tensorflow_backend as KTF

import pandas as pd
import csv
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf

#keras GPU��̬����
config = tf.ConfigProto()
config.gpu_options.allow_growth=True  #��ȫ��ռ���Դ�, �������
sess = tf.Session(config=config)
KTF.set_session(sess)

#����matplotlib��ͼ��ʾ����
import matplotlib as mpl
mpl.rcParams[u'font.sans-serif'] = ['SimHei']#����
mpl.rcParams['axes.unicode_minus'] = False #������š�-����ʾΪ���������
from tensorflow.python.framework import graph_util

#���ó���
rnn_unit=10       #���ز���Ԫ
input_size=7      #����ά��
output_size=1     #���ά��
lr=0.0006         #ѧϰ��
step=20      #ʱ�䲽

#�������������������������������������������ݡ�������������������������������������������
with open('./dataset/dataset_2.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    data = []
    i = 0
    for x in reader:
        if i != 0:
            x = x[2:10]  # ȡ��3-9��
            x = [float(k) for k in x]
            data.append(x)
        i = i + 1
    data = np.array(data)

#��ȡѵ����
def get_train_data(batch_size=60,time_step=step,train_begin=0,train_end=5800):
    batch_index=[]
    data_train=data[train_begin:train_end]
    normalized_train_data=(data_train-np.mean(data_train,axis=0))/np.std(data_train,axis=0)  #��׼��
    train_x,train_y=[],[]   #ѵ���� 
    for i in range(len(normalized_train_data)-time_step):
       if i % batch_size==0:
           batch_index.append(i)
       x=normalized_train_data[i:i+time_step,:7]
       y=normalized_train_data[i:i+time_step,7,np.newaxis]
       train_x.append(x.tolist())
       train_y.append(y.tolist())
    batch_index.append((len(normalized_train_data)-time_step))
    return batch_index,train_x,train_y

#�������������������������������������������������������������������������������������
weights={
         'in':tf.Variable(tf.random_normal([input_size,rnn_unit])),
         'out':tf.Variable(tf.random_normal([rnn_unit,1]))
        }
biases={
        'in':tf.Variable(tf.constant(0.1,shape=[rnn_unit,])),
        'out':tf.Variable(tf.constant(0.1,shape=[1,]))
       }

#���������������������������������������������硪����������������������������������
def lstm(X):     
    batch_size=tf.shape(X)[0]
    time_step=tf.shape(X)[1]
    w_in=weights['in']
    b_in=biases['in']
    input=tf.reshape(X,[-1,input_size])  #��Ҫ��tensorת��2ά���м��㣬�����Ľ����Ϊ���ز������
    input_rnn=tf.matmul(input,w_in)+b_in
    input_rnn=tf.reshape(input_rnn,[-1,time_step,rnn_unit])  #��tensorת��3ά����Ϊlstm cell������

    #cell=tf.nn.rnn_cell.BasicLSTMCell(rnn_unit)
    lstm = tf.nn.rnn_cell.BasicLSTMCell(rnn_unit)
    cell=tf.nn.rnn_cell.MultiRNNCell([lstm for _ in range(2)])

    init_state=cell.zero_state(batch_size,dtype=tf.float32)
    output_rnn,final_states=tf.nn.dynamic_rnn(cell, input_rnn,initial_state=init_state, dtype=tf.float32)
    output=tf.reshape(output_rnn,[-1,rnn_unit]) #��Ϊ����������
    w_out=weights['out']
    b_out=biases['out']
    pred=tf.matmul(output,w_out)+b_out
    return pred,final_states


#������������������������������������ѵ��ģ�͡�����������������������������������
def train_lstm(batch_size=80,time_step=step,train_begin=2000,train_end=5800):
    X=tf.placeholder(tf.float32, shape=[None,time_step,input_size])
    Y=tf.placeholder(tf.float32, shape=[None,time_step,output_size])
    batch_index,train_x,train_y=get_train_data(batch_size,time_step,train_begin,train_end)
    pred,_=lstm(X)
    #��ʧ����
    loss=tf.reduce_mean(tf.square(tf.reshape(pred,[-1])-tf.reshape(Y, [-1])))
    train_op=tf.train.AdamOptimizer(lr).minimize(loss)
    saver=tf.train.Saver(tf.global_variables(),max_to_keep=15)
    #module_file = tf.train.latest_checkpoint('./Model/')#����ѵ����Ҫע�͵�

    train_loss=[]
    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer()) #��ʼ�����б���
        #saver.restore(sess, module_file)#����ѵ����Ҫע�͵�
        #�ظ�ѵ������
        for i in range(1000):
            for step in range(len(batch_index)-1):
                _,loss_=sess.run([train_op,loss],feed_dict={X:train_x[batch_index[step]:batch_index[step+1]],Y:train_y[batch_index[step]:batch_index[step+1]]})
            print(i,loss_)
            train_loss.append(loss_)
        print("����ģ�ͣ�",saver.save(sess,'./Model/stock2.model',global_step=i))

    #����Train loss
    plt.plot(train_loss,label='train_loss')
    plt.legend()
    plt.show()  

train_lstm()