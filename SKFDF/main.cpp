//SKFrequencyDomainFiltering
//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.11.19
//Use Ctrl+M,Ctrl+O to fold the code.

#include <cv.h>
#include"SKCommand.h"
#include"SKImageDisplayer.h"
#include"SKDefinition.h"
using namespace std;
using namespace cv;

//Mat cvdft(Mat src)
//{
//	Mat padded;                            //expand input image to optimal size
//	int m = getOptimalDFTSize(src.rows);
//	int n = getOptimalDFTSize(src.cols); // on the border add zero values
//	copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));
//
//	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
//	Mat complexI;
//	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
//
//	dft(complexI, complexI);            // this way the result may fit in the source matrix
//
//	// compute the magnitude and switch to logarithmic scale
//	// => log(1 + sqrt(Re(DFT(src))^2 + Im(DFT(src))^2))
//	split(complexI, planes);                   // planes[0] = Re(DFT(src), planes[1] = Im(DFT(src))
//	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude  
//	Mat magI = planes[0];
//
//	magI += Scalar::all(1);                    // switch to logarithmic scale
//	log(magI, magI);
//
//	// crop the spectrum, if it has an odd number of rows or columns
//	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
//
//	// rearrange the quadrants of Fourier image  so that the origin is at the image center        
//	int cx = magI.cols / 2;
//	int cy = magI.rows / 2;
//
//	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant 
//	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
//	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
//	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
//
//	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
//	q0.copyTo(tmp);
//	q3.copyTo(q0);
//	tmp.copyTo(q3);
//
//	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
//	q2.copyTo(q1);
//	tmp.copyTo(q2);
//
//	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a 
//	// viewable image form (float between values 0 and 1).
//	return magI;
//}

namespace SKfft
{
	/*fft算法，输入图像为8U（灰度图），输出为2*32F的图像（实部与虚部，未进行四角校正）*/
	Mat mydft(Mat &src)
	{
		Mat src2;
		src.convertTo(src2, CV_32F);
		Mat planes[] = { Mat_<float>(src2), Mat::zeros(src2.size(), CV_32F) };
		Mat complexI;
		merge(planes, 2, complexI);
		dft(complexI, complexI);
		return complexI;
	}

	/*fft-inv算法，输入图像为有实部和虚部的2*32F图像，输出为8U灰度图*/
	Mat myinvdft(Mat &src)
	{
		dft(src, src, DFT_INVERSE);
		Mat dst;
		vector<Mat_<float>> planes;
		split(src, planes);
		magnitude(planes[0], planes[1], planes[0]);
		src = planes[0];
		normalize(src, src, 0, 255, CV_MINMAX);
		src.convertTo(dst, CV_8U);
		return dst;
	}

	/*实部虚部合成算法，输入为2*32F图，输出为32F图片（仅留下了幅值），若要显示结果，应当标准归一化(normalize)*/
	Mat mixmatrix(Mat &src)
	{
		Mat dst;
		vector<Mat_<float>> planes;
		split(src, planes);
		magnitude(planes[0], planes[1], dst);
		return dst;
	}

	/*四角校正，opencv的fft输出的结果在四个角上，应当稍加处理。（swap）*/
	void reswap4p(Mat &src)
	{
		int cx = src.cols / 2;
		int cy = src.rows / 2;

		Mat q0(src, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant 
		Mat q1(src, Rect(cx, 0, cx, cy));  // Top-Right
		Mat q2(src, Rect(0, cy, cx, cy));  // Bottom-Left
		Mat q3(src, Rect(cx, cy, cx, cy)); // Bottom-Right

		Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
		q2.copyTo(q1);
		tmp.copyTo(q2);
	}
}

using namespace SKfft;
//FFT: cv_32f->fft->mix(Re and Im)->normalize->reswap4p
int main()
{
	Mat input,output,output2,mixoutput;
	input = imread("data\\HW2.jpg");
	SKImageDisplayer idis;
	idis.display(&input);
	SKCommand::wait_seconds(5);
	idis.hide();
	cvtColor(input, input, CV_BGR2GRAY);
	output = mydft(input);
	{
		mixoutput = mixmatrix(output);
		normalize(mixoutput, mixoutput, 0, 255, CV_MINMAX);
		reswap4p(mixoutput);
		idis.display(&mixoutput);
	}
	SKCommand::wait_till_end("Input anything to continue\n");
	idis.hide();
	output2 = myinvdft(output);
	idis.display(&output2);
	SKCommand::wait_till_end("Input anything to continue\n");
	return 0;
}