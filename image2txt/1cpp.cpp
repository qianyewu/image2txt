#include "pch.h"
#include <iostream>  
#include <omp.h> // OpenMP编程需要包含的头文件  
#include <opencv2\opencv.hpp>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
using namespace cv;
/*
///...
void cv::putText(
	cv::Mat& img, // 待绘制的图像
	const string& text, // 待绘制的文字
	cv::Point origin, // 文本框的左下角
	int fontFace, // 字体 (如cv::FONT_HERSHEY_PLAIN)
	double fontScale, // 尺寸因子，值越大文字越大
	cv::Scalar color, // 线条的颜色（RGB）
	int thickness = 1, // 线条宽度
	int lineType = 8, // 线型（4邻域或8邻域，默认8邻域）
	bool bottomLeftOrigin = false // true='origin at lower left'
);
*/
/**/
int main0()
{


	Mat src = imread("F:/C/image2txt/resource/33.png");
	Mat srcROI = src(Rect(0, 0, src.cols / 2, src.rows / 2));
	int w, h;
	w = 16;
	h = 16;

	Mat mat(w, h, CV_8UC1);
	threshold(mat, mat, 0, 255, CV_THRESH_OTSU);
	imshow("mat0", mat);
	int i = cv::sum(mat).val[0];
	cout << "i  : " << i << endl;
	//randu(mat, Scalar::all(0), Scalar::all(255));

	cout << "【默认风格】" << endl << mat << endl << endl;


	Mat dstImage2;

	//获取文本框的长宽
	cv::Size text_size = cv::getTextSize("0", FONT_HERSHEY_SIMPLEX, 1, 1, 0);
	cv::Point origin;
	//将文本框居中绘制
//	origin.x = mat.cols / 2 - text_size.width / 2;
//	origin.y = mat.rows / 2 + text_size.height / 2;

	origin.x = 0.5;
	origin.y = mat.rows - 1;
	std::cout << origin.x << " - " << origin.y << std::endl;
	//CV_FONT_HERSHEY_COMPLEX 
	putText(mat, "03..  MMm@$", origin, CV_FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 0, 0), 1, 8);
	resize(mat, dstImage2, Size(mat.cols * 10, mat.rows * 10), 0, 0, INTER_LINEAR);
	cout << "【默认风格】" << endl << mat << endl << endl;
	imshow("放大", dstImage2);
	imshow("mat1", mat);
	int j = cv::sum(mat).val[0];
	cout << "j  : " << j << endl;


	//pt是mat类型，n行2列的矩阵

	//imshow("src", src);
	//imshow("srcRoi", srcROI);
	waitKey(0);





	/*
#pragma omp parallel  num_threads(4)
	{
#pragma omp for
		for (int i = 0; i < 100; ++i)
		{
			std::cout << i << "+" << std::endl;
		}
		//#pragma omp for
		//		for (int j = 0; j < 10; ++j)
		//		{
		//			std::cout << j << "-" << std::endl;
		//		}
	}
	*/

	system("pause");
	return 0;
}

