// image2txt.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <omp.h>

using namespace std;
using namespace cv;
//const 不可变
//在c++中，vector是一个十分有用的容器。
//作用：它能够像容器一样存放各种类型的对象，简单地说，vector是一个能够存放任意类型的动态数组，能够增加和压缩数据。
const string words_str = R"(1234567890-=qwertyuiop[]\|asdfghjkl;':"zxcvbnm,./<>?!@#$%^&*()_+Q WERTYUIOPASDFGHJKLZXCVBNM)";
vector<Mat>  words_pic;

void create_words_pic(const string&filename, vector<Mat>&words_pic, int w = 8, int h = 16) {
	Mat src_img = imread(filename);
	Mat gray_img, bin_img, xor_img(src_img.size(), CV_8UC1, Scalar(255));

	cvtColor(src_img, gray_img, CV_RGB2GRAY);
	threshold(gray_img, bin_img, 0, 255, CV_THRESH_OTSU);
	bitwise_xor(bin_img, xor_img, bin_img);
	//if (h <= 8)
		//图片放缩
		resize(bin_img, bin_img, Size(bin_img.cols, h), 0, 0, INTER_LINEAR);

	for (int i = 0; i < bin_img.cols / w; i++) {
		words_pic.push_back(bin_img(Rect(i * w, 0, w, h)).clone());//在words_pic尾部加入一个数据
	}
	//Mat m,n;
//	n = words_pic[3];
//	resize(n, m, Size(n.cols * 10, n.rows * 5), 0, 0, INTER_LINEAR);
	imshow("words_pic[]", words_pic[3]);
	/*	for (int i = 0; i < bin_img.cols / 8; i++) {
			words_pic.push_back(bin_img(Rect(i * 8, 0, 8, 16)).clone());
		}
		*/
}


//src1和src22个小图片做差，返回最后的到的图片的像素值总和（sum越小，则src1和src2越像）
int same_cmp(const Mat&src1, const Mat&src2) {
	Mat b, c;
	// cvAbsDiff( const CvArr* src1, const CvArr* src2, CvArr* dst );  计算两个数组差的绝对值
	//src1第一个原数组，src2第二个原数组，dst输出数组
	cv::absdiff(src1, src2, c);
	/*depth_image_src.convertTo(depth_image, CV_8U, 255./4096.);

将depth_image_src转换为depth_image，其格式为Mat1=convertTo(Mat2,CV_64FC1,1/255.0)*/

/*矩阵数据类型:
– CV_<bit_depth>(S|U|F)C<number_of_channels>
S = 符号整型 U = 无符号整型 F = 浮点型     number_of_channels通道数*/
	c.convertTo(b, CV_32SC1);
	int i;
	//cout <<"type sum[b]: "<< typeid(sum(b)).name() << endl;
	i = cv::sum(b).val[0];
	//cout << "i  : " << i << endl;
	return i;
}

//循环遍历words_pic，寻找哪个小图片和src最接近（）
int get_index(const Mat& src) {
	typedef std::pair<int, int> ss;
	vector<ss> ary;
	ary.reserve(128);//reserve(n)预分配n个元素的存储空间。

	/*匿名函数*/
	//用处何在？
/*	auto func = [&](const ss&a, const ss&b) {
		return a.second < b.second;
	};*/
	int j;
	for (int i = 0; i < words_pic.size(); i++) {
		j = same_cmp(src, words_pic[i]);
		//cout << "j: " << j << endl;
		ary.push_back(ss(i, j));

	}

	int index = -1;
	int min_val = INT_MAX;
	for (auto& s : ary) {
		if (min_val > s.second) {
			min_val = s.second;
			index = s.first;
			//cout << "index: " << index << endl;
		}
	}

	/*    */
	//cout << "index2  : " << index << endl;


	return index;


}

void pic2txt(const Mat& src, Mat &dst, int w = 8, int h = 16) {
	int c_w = src.cols%w;
	//cout << "c_w：  " << c_w << "  src.cols  " << src.cols << endl;
	int c_h = src.rows%h;
	//cout << "c_h：  " << c_h << "  src.rows  " << src.rows << endl;
	if (c_w || c_h) {
		//resize为图像缩放函数，src为原图像，dst为输出图像
		resize(src, dst, Size(src.cols - c_w, src.rows - c_h));
		//cout << "dst0 cols- " << dst.cols << "  dst0 row- " << dst.rows << endl;
	}
	else {
		dst = src;
	}
	/*  如果使用  #pragma omp parallel   这条简单的指令去运行并行计算的话，程序的线程数将由运行时
	系统决定（这里使用的算法十分复杂），典型的情况下，系统将在每一个核上运行
	一个线程。如果需要执行使用多少个线程来执行我们的并行程序，就得为parallel指
	令增加num_threads子句，这样的话，就允许程序员指定执行后代码块的线程数。
	*/
#pragma omp parallel num_threads(4)
	{
		Mat roi;
		int index;
		//cout << omp_get_thread_num() << endl;
#pragma omp for nowait
		for (int j = 0; j < src.rows / h; j++) {
			for (int i = 0; i < src.cols / w; i++) {
				//Rect类成员变量x、y、width、height，分别为左上角点的坐标和矩形的宽和高
				//Mat operator () ( const Rect&roi ) const，矩形roi指定了兴趣区
				//roi的数据与源图像dst共享存储区，所以此后在roi上的操作也会作用在源图像dst上。
				roi = dst(Rect(i * w, j * h, w, h));
				//imshow("roi", roi);
				index = get_index(roi);
				//cout << "index wei：  " << index << endl;
				words_pic[index].copyTo(roi);//将words_pic[index]复制到roi矩阵中
			}
		}
	}
}


//阈值二值化
void Wellner(const cv::Mat& src, cv::Mat &dst) {
	assert(!src.empty());
	dst = cv::Mat(src);
	uchar *GrayImg;
	uchar *BinImg;
	int width = src.cols, height = src.rows;

	int i, j, x1, x2, y1, y2;
	int index;
	int T;
	int S, s2;
	size_t count;
	size_t *integralImg;
	size_t sum;

	GrayImg = src.data;
	BinImg = dst.data;
	integralImg = new(std::nothrow) size_t[width*height];
	if (integralImg == nullptr) {
		exit(-22);
	}
	S = width >> 3;
	T = 10;
	s2 = S / 2;
	sum = 0;

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			index = j * width + i;
			sum += GrayImg[index];
			if (i == 0)
				integralImg[index] = sum;
			else
				integralImg[index] = integralImg[index - 1] + sum;
		}
	}

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			index = j * width + i;
			x1 = i - s2;
			x2 = i + s2;
			y1 = j - s2;
			y2 = j + s2;

			if (x1 < 0) x1 = 0;
			if (x2 >= width) x2 = width - 1;
			if (y1 < 0) y1 = 0;
			if (y2 >= height) y2 = height - 1;

			count = (x2 - x1) * (y2 - y1)*GrayImg[index];
			sum = integralImg[y2 * width + x2] -
				integralImg[y1 * width + x2] -
				integralImg[y2 * width + x1] +
				integralImg[y1 * width + x1];
			sum = sum * (100 - T) / 100;

			if (count < sum) {
				BinImg[index] = 0;
			}
			else {
				BinImg[index] = 255;
			}
		}
	}
	delete[]integralImg;
}


int main() {
	int w = 8, h = 16;//文字块的大小  w*h
	create_words_pic("F:/C/image2txt/resource/word.png", words_pic, w, h);

	Mat src_img, dst, gray_img, bin_img, bin_img0;
	//putText();
	//src_img = imread(R"(F:/C/image2txt/resource/33.png)");
	src_img = imread(R"(C:\Users\qianyewu\Desktop\千夜舞\33.png)");
	cout << "cols- " << src_img.cols << "  row- " << src_img.rows << endl;
	//imshow("src_img", src_img);
	cvtColor(src_img, gray_img, CV_RGB2GRAY);
	//imshow("gray_img", gray_img);
//	threshold(gray_img, bin_img0, 0, 255, CV_THRESH_OTSU);
//	imshow("bin_img0", bin_img0);
	Wellner(gray_img, bin_img);
	//cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
	imshow("bin_img", bin_img);
	//imwrite("bin_img.png", bin_img);
	pic2txt(bin_img, dst, w, h);//dst表示由二值图片生成的字画图

	cout << "dst cols- " << dst.cols << "  dst row- " << dst.rows << endl;
	imshow("dst", dst);
	//imwrite("det.png", dst);
	waitKey(0);
	//	destroyWindow("MyWindow");
	return 0;
}




//
//int main() {
//	create_words_pic("f:/c/image2txt/resource/word.png", words_pic);
//
//	cv::tickmeter t;
//	t.start();
//	videocapture mp4("f:/c/image2txt/resource/in.avi");	//输入视频
//	videowriter  wit("f:/c/image2txt/resource/out.avi", cv_fourcc('m', 'j', 'p', 'g'), 23.0, size(1280, 720), false); //输出视频
//	mat src_img, dst;//= imread("c:/users/yd/pictures/1.png");
//	long totalframenumber = mp4.get(cv_cap_prop_frame_count);
//	long inc = 0;
//	while (mp4.read(src_img)) {
//		cout << totalframenumber << "\\" << inc++ << endl;
//		mat gray_img, bin_img;
//		cvtcolor(src_img, gray_img, cv_rgb2gray);
//		//threshold(gray_img, bin_img, 0, 255, cv_thresh_otsu);
//		wellner(gray_img, bin_img);
//		pic2txt(bin_img, dst);
//		wit.write(dst);
//		//imshow("src", dst);
//		//waitkey(1);
//	}
//	wit.release();
//	t.stop();
//	cout << t.gettimesec() << endl;
//	waitkey(0);
//	return 0;
//}

