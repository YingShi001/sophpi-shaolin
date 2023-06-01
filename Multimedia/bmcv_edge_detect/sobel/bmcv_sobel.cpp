/*
*	kk 2022/11/2 
*	bmcv_sobel.cpp //���ļ���Ҫʵ���˶�ͼƬ�Ľ���sobel��Ե��⣬
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
        bm_handle_t handle;					  //��ȡ��� 
        bm_dev_request(&handle, 0);
        int width =  600;					  //����ͼƬ���� 
        int height = 600;
        cv::Mat Input,Out,Test; 				  // opencv��ȡͼƬ 
        Input = cv::imread(argv[1], 0);	
		
        // ����ָ���ȡ�����ڴ����� 
        std::unique_ptr<unsigned char[]> src_data(
            new unsigned char[width * height]);
        std::unique_ptr<unsigned char[]> res_data(
	    	new unsigned char[width * height]);
		memset(src_data.get(), 0x11, width * height);
		memset(res_data.get(), 0, width * height);
        // bmcv���� 
	bm_image input, output;
	
	//v���������BMCV IMAGE
	bm_image_create(handle, height, width, FORMAT_GRAY, DATA_TYPE_EXT_1N_BYTE, &input);

	// bm_image_alloc_dev_mem��input��;                      //�����������Ҳ���������豸�ڴ�
        bm_image_alloc_contiguous_mem(1, &input, 1); 	        // ����device memory 
	
	unsigned char * input_img_data = src_data.get();
	bm_image_copy_host_to_device(input, (void **)&input_img_data);	
	
	// ���������BMCV IMAGE
        bm_image_create(handle, height, width, FORMAT_GRAY, DATA_TYPE_EXT_1N_BYTE, &output);
		
	bm_image_alloc_contiguous_mem(1, &output, 1);	
		
	cv::bmcv::toBMI(Input,&input);                         //�Զ������ڴ�ͬ��
	// cv::bmcv::toBMI(Input,&input,true);                 //�������ͬ
	// bm_image_copy_host_to_device                        //Ҳ����ʹ���������
		
	// bmcvͼ���� 
	if (BM_SUCCESS != bmcv_image_sobel(handle, input, output, 0, 1)) {
	    std::cout << "bmcv sobel error !!!" << std::endl;
	    bm_image_destroy(input);
	    bm_image_destroy(output);
	    bm_dev_free(handle);
	    return -1;
	}
	unsigned char *res_img_data = res_data.get();
	bm_image_copy_device_to_host(output, (void **)&res_img_data);
	// ��������ת��mat���ݲ����� 
	cv::bmcv::toMAT(&output, Out);
	cv::imwrite("out.jpg", Out);
		
	bm_image_free_contiguous_mem(1, &input);
	bm_image_free_contiguous_mem(1, &output);
	bm_image_destroy(input);
	bm_image_destroy(output);
	bm_dev_free(handle);

	return 0; 

      
    } 
    //����OPENCV���д���
    if(0 == strcmp("opencv", argv[2])) {
	Mat src1,src,dst;
	src1 = cv::imread(argv[1], 1);
	resize(src1, src, Size(src1.cols/2, src1.rows/2));
	if (!src.data) {
	    printf("cannot load image ...");
	    return -1;
	} 

	GaussianBlur(src, dst, Size(3,3), 0);
	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	    
        //sobel����
	Mat dst_x, dst_y;
	Sobel(src_gray, dst_x, -1, 1, 0); 		    // x�����������������ȡ0,1,2
	Sobel(src_gray, dst_y, -1, 0, 1); 		    // y�������������
	addWeighted(dst_x, 0.5, dst_y, 0.5, 0, dst);
	cv::imwrite("out.jpg", dst);

	return 0;
    }
    
    usage(argv[0]);
    return -1;
}

