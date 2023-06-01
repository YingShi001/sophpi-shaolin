/*
*	kk 2022/11/2 
*	bmcv_weight.cpp //���ļ���Ҫʵ���˶�ͼƬ���м�Ȩ��Ӳ�����
*                         ʵ������ͼƬ���ں�Ч�� 
*/ 

#include <iostream>
#include <vector>
#include "bmcv_api.h"
#include "common.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


static void usage(char *program_name)
{
    av_log(NULL, AV_LOG_ERROR, "Usage: \n\t%s [input_filename1] [input_filename2] [bmcv or opencv] \n", program_name);
    av_log(NULL, AV_LOG_ERROR, "\t%s exampel1.jpeg exampel2.jpeg bmcv.\n", program_name);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        usage(argv[0]);
        return -1;
    }
    
    if(0 == strcmp("bmcv",argv[3])) {
        //��ȡ��� 
        bm_handle_t handle;
        bm_dev_request(&handle, 0);
        //��ȡͼƬ1 2 
        cv::Mat Input1,Out,Input2;
        Input1 = cv::imread(argv[1], 0);
        Input2 = cv::imread(argv[2],0);
        int width = 1000;
        int height = 1000;
        int channel = 3; 
	
        // ����ָ�� 
        std::unique_ptr<unsigned char[]> src1_data(
            new unsigned char[width * height * channel]);
        std::unique_ptr<unsigned char[]> src2_data(
            new unsigned char[width * height * channel]);
        std::unique_ptr<unsigned char[]> res_data(
            new unsigned char[width * height * channel]);
		
        // ����ͼ����
        //����bmcv���� 
        bm_image input1, input2, output;
        bm_image_create(handle, height, width, FORMAT_RGB_PLANAR, DATA_TYPE_EXT_1N_BYTE, &input1);
        bm_image_alloc_contiguous_mem(1, &input1, 1);
		
        bm_image_create(handle, height, width, FORMAT_RGB_PLANAR, DATA_TYPE_EXT_1N_BYTE,&input2);
        bm_image_alloc_contiguous_mem(1, &input2, 1);
	
        //����������� 
        bm_image_create(handle, height, width, FORMAT_RGB_PLANAR, DATA_TYPE_EXT_1N_BYTE,&output);
        bm_image_alloc_contiguous_mem(1, &output, 1);
        cv::bmcv::toBMI(Input1,&input1);
        cv::bmcv::toBMI(Input2,&input2);
        //����bmcv���� 
        if (BM_SUCCESS != bmcv_image_add_weighted(handle, input1, 0.5, input2, 0.5, 0,output)) {
            std::cout << "bmcv add_weighted error !!!" << std::endl;
            bm_image_destroy(input1);
            bm_image_destroy(input2);
            bm_image_destroy(output);
            bm_dev_free(handle);
            exit(-1);
        }
        //���������� 
        cv::bmcv::toMAT(&output, Out);
        cv::imwrite("out.jpg", Out);
        //�����ڴ� 
        bm_image_free_contiguous_mem(1, &input1);
        bm_image_free_contiguous_mem(1, &input2);
        bm_image_free_contiguous_mem(1, &output);
        bm_image_destroy(input1);
        bm_image_destroy(input2);
        bm_image_destroy(output);
        bm_dev_free(handle);
        return 0; 
        
    } 
    
    if (0 == strcmp("opencv",argv[3])) {
        //����ͼ��ļ�Ȩ�ں�
        Mat m6 = imread(argv[1], 1);
        Mat m7 = imread(argv[2], 1);
        //����m7��m6��С��ͬ
        resize(m7, m7, m6.size(), 0, 0);
        Mat m8;
        //��Ȩ�ںϡ�����
        addWeighted(m6, 0.5, m7, 0.5,0, m8, -1);
        cv::imwrite("out.jpg", m8);

        return 0;
    }
    
    if (argc != 4) {
        usage(argv[0]);
        return -1;
    }
}

