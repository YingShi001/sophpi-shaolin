/*
*	kk 2022/11/2 
*	bmcv_canny.cpp //���ļ���Ҫʵ���˶�ͼƬ�Ľ���canny��Ե��⣬
*                        ������Ե�������� 
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
    av_log(NULL, AV_LOG_ERROR, "Usage: \n\t%s [input_filename]  [bmcv or opencv] \n", program_name);
    av_log(NULL, AV_LOG_ERROR, "\t%s exampel.jpeg bmcv.\n", program_name);
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        usage(argv[0]);
        return -1;
    }
    
    //����BMCV���д���
    if(0 == strcmp("bmcv", argv[2])) {
        
        // ��ȡ��� 
        bm_handle_t handle;
        bm_dev_request(&handle, 0);
        // ����ͼƬ��ʽ 
        int width = 600;
        int height = 600;
        // opencv����ͼƬ 
        cv::Mat Input,Out,Test;
        Input = cv::imread(argv[1], 0);
	
        std::unique_ptr<unsigned char[]> src_data(
            new unsigned char[width * height]);
        std::unique_ptr<unsigned char[]> res_data(
            new unsigned char[width * height]);
		memset(src_data.get(), 0x11, width * height);
		memset(res_data.get(), 0, width * height);
        
		// ����bmcv���� 
        bm_image input, output;
        bm_image_create(handle, height, width, FORMAT_GRAY, DATA_TYPE_EXT_1N_BYTE, &input);
        bm_image_alloc_contiguous_mem(1, &input, 1);
		
		unsigned char * input_img_data = src_data.get();
		bm_image_copy_host_to_device(input, (void **)&input_img_data);
		
        bm_image_create(handle, height, width, FORMAT_GRAY, DATA_TYPE_EXT_1N_BYTE, &output);	
        bm_image_alloc_contiguous_mem(1, &output, 1);
        
        // mat����ת��Ϊbmcv���� 
        cv::bmcv::toBMI(Input,&input);
        // bmcvͼ����canny 
        if (BM_SUCCESS != bmcv_image_canny(handle, input, output, 0, 200)) {
            std::cout << "bmcv sobel error !!!" << std::endl;
            bm_image_destroy(input);
            bm_image_destroy(output);
            bm_dev_free(handle);
            exit(-1);
        }
        unsigned char *res_img_data = res_data.get();
		bm_image_copy_device_to_host(output, (void **)&res_img_data);
        // �������� 
        cv::bmcv::toMAT(&output, Out);
        cv::imwrite("out.jpg", Out);
        // �����ڴ� 
        bm_image_free_contiguous_mem(1, &input);
        bm_image_free_contiguous_mem(1, &output);
        bm_image_destroy(input);
        bm_image_destroy(output);
        bm_dev_free(handle);
        return 0; 
        
        
    } 
    
    // ����OPENCV���д���
    if(0 == strcmp("opencv",argv[2])) {
        // opencvͼ���� 
        cv::Mat srcImage = cv::imread(argv[1], 1);
        cv::Mat grayImage; 
        cv::Mat srcImage1 = srcImage.clone();
        cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
        Mat dstImage, edge;
	 
        blur(grayImage, grayImage, Size(3,3));
	// opencv canny 
        Canny(grayImage, edge, 150, 100, 3);
	 
        dstImage.create(srcImage1.size(), srcImage1.type());
        dstImage = Scalar::all(0);
        srcImage1.copyTo(dstImage, edge);
	
        if (dstImage.empty()) {
            cout << "Warning! decoded image is empty" << endl;
        } else {
            cv::imwrite("out.jpg", dstImage);
        }
        
        return 0;
     }
     
     usage(argv[0]);
     
     return -1;
}

