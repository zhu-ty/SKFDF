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

#pragma warning (disable:4244)

namespace SKfft
{
	/*fft�㷨������ͼ��Ϊ8U���Ҷ�ͼ�������Ϊ2*32F��ͼ��ʵ�����鲿��δ�����Ľ�У����*/
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

	/*fft-inv�㷨������ͼ��Ϊ��ʵ�����鲿��2*32Fͼ�����Ϊ8U�Ҷ�ͼ*/
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

	/*ʵ���鲿�ϳ��㷨������Ϊ2*32Fͼ�����Ϊ32FͼƬ���������˷�ֵ������Ҫ��ʾ�����Ӧ����׼��һ��(normalize)*/
	Mat mixmatrix(Mat &src)
	{
		Mat dst;
		vector<Mat_<float>> planes;
		split(src, planes);
		magnitude(planes[0], planes[1], dst);
		return dst;
	}

	/*�Ľ�У����opencv��fft����Ľ�����ĸ����ϣ�Ӧ���ԼӴ�����swap��*/
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

	/*��ʾͼƬ���ȴ�*/
	void show_and_wait(Mat *m,SKImageDisplayer *idis)
	{
		idis->hide();
		idis->display(m);
		SKCommand::wait_till_end("Wait to hide the image.\n");
		idis->hide();
		return;
	}
}

namespace image_split
{
	inline int myabs(int x)
	{
		return (x > 0) ? x : (-1 * x);
	}
	void flood_fill(const Mat &src,Mat &mask, Point &p)
	{
		//if(mask.at
	}
}

//FFT: cv_32f->fft->mix(Re and Im)->normalize->reswap4p
/*
Rect(x,y,w,h)
        w(cols)
    <----------->
h^  ------------y
(|  |
r|  |
o|  |
w|  |
s|  |
)��  x
*/
int main()
{
	using namespace SKfft;
	Mat input,inputb,tmp1,tmp2,output;
	input = imread("data\\HW2.jpg");
	SKImageDisplayer idis;
	idis.display(&input);
	SKCommand::wait_seconds(5);
	idis.hide();

	cvtColor(input, input, CV_BGR2GRAY);
	threshold(input, inputb, 70, 255, CV_THRESH_BINARY);

	{
		//FFT���˲�
		tmp1 = mydft(inputb);
		reswap4p(tmp1);
		int cx = tmp1.cols / 2;
		int cy = tmp1.rows / 2;
		Mat up(tmp1, Rect(cx - cx*CROP_WIDTH_PERCENT, 0, 2 * cx*CROP_WIDTH_PERCENT, cy*CROP_LENGTH_PERCENT));
		Mat down(tmp1, Rect(cx - cx*CROP_WIDTH_PERCENT, 2 * cy - cy*CROP_LENGTH_PERCENT, 2 * cx*CROP_WIDTH_PERCENT, cy*CROP_LENGTH_PERCENT));
		up.setTo(Scalar::all(0));
		down.setTo(Scalar::all(0));
		reswap4p(tmp1);
		tmp1 = myinvdft(tmp1);
		//show_and_wait(&tmp1,&idis);
		tmp1 = mydft(tmp1);
		reswap4p(tmp1);
		cx = tmp1.cols / 2;
		cy = tmp1.rows / 2;
		Mat left(tmp1, Rect(0,cy-cy*CROP_WIDTH_PERCENT,cx*CROP_LENGTH_PERCENT,2*cy*CROP_WIDTH_PERCENT));
		Mat right(tmp1, Rect(2 * cx - cx*CROP_LENGTH_PERCENT, cy - cy*CROP_WIDTH_PERCENT, cx*CROP_LENGTH_PERCENT, 2 * cy*CROP_WIDTH_PERCENT));
		left.setTo(Scalar::all(0));
		right.setTo(Scalar::all(0));
		reswap4p(tmp1);
	}

#ifdef SHOW_FFT
	Mat	mixoutput = mixmatrix(tmp1);
	normalize(mixoutput, mixoutput, 0, 255, CV_MINMAX);
	reswap4p(mixoutput);
	show_and_wait(&mixoutput, &idis);
#endif

	tmp2 = myinvdft(tmp1);
	//show_and_wait(&tmp2, &idis);
	//adaptiveThreshold(tmp2, tmp2, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,3,5);
	threshold(tmp2, tmp2, 95, 255, CV_THRESH_BINARY);
	//show_and_wait(&tmp2, &idis);
	Mat mask = Mat::zeros(tmp2.rows, tmp2.cols, CV_8U);
	{
		//�ҵ���������
		//dilate(mask, mask, Mat());
		//erode(tmp2, tmp2, Mat());
		Mat tmp2r(tmp2, Rect(1.0 / 3 * tmp2.cols, 0, 2.0 / 3 * tmp2.cols, tmp2.rows));
		Mat tmp2ur(tmp2, Rect(0, 0, 1.0 / 3 * tmp2.cols, 1.0/5*tmp2.rows));
		erode(tmp2r, tmp2r, Mat());
		erode(tmp2ur, tmp2ur, Mat());
		vector<vector<Point>> contours;
		findContours(tmp2, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		if (!contours.empty())
		{
			int max = 0;
			for (int i = 0; i < contours.size(); i++)
				if (contourArea(contours[i]) > contourArea(contours[max]))
					max = i;
			drawContours(mask, contours, max, Scalar(255), -1);
		}
		
		erode(mask, mask, Mat());
		erode(mask, mask, Mat());
		erode(mask, mask, Mat());
		//erode(mask, mask, Mat());
		//erode(mask, mask, Mat());
		//dilate(mask, mask, Mat());
		//dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		dilate(mask, mask, Mat());
		erode(mask, mask, Mat());
		erode(mask, mask, Mat());
		erode(mask, mask, Mat());
		dilate(mask, mask, Mat());
	}
	Mat ehinput;
	Mat ehoutput;
	equalizeHist(input, ehinput);
	input.copyTo(output, mask);
	ehinput.copyTo(ehoutput, mask);
	show_and_wait(&output, &idis);
	Mat spiltans[3];
	Mat spiltmask[3];
	{
		//blur(output, output, Size(3, 3));
		for (int i = 0; i < 3; i++)
		{
			ehoutput.copyTo(spiltmask[i]);
			if (i > 0)
				spiltmask[i].setTo(Scalar::all(0));
		}
		GaussianBlur(spiltmask[0], spiltmask[0], Size(7, 7), 0, 0);
		//equalizeHist(output, output);
		//floodfill
		Mat mask2;
		bitwise_not(mask, mask2);
		resize(mask2, mask2, Size(mask2.cols + 2, mask2.rows + 2));
		cv::floodFill(spiltmask[0], mask2, cv::Point(spiltmask[0].cols / 2, spiltmask[0].rows / 2), Scalar(255), (Rect *)0, Scalar(1), Scalar(1));
		threshold(spiltmask[0], spiltmask[0], 254, 255, CV_THRESH_BINARY);

		//find_mid
		vector<vector<Point>> contours;
		findContours(spiltmask[0], contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		if (!contours.empty())
		{
			int max = 0;
			for (int i = 0; i < contours.size(); i++)
				if (contourArea(contours[i]) > contourArea(contours[max]))
					max = i;
			spiltmask[0].setTo(Scalar::all(0));
			drawContours(spiltmask[0], contours, max, Scalar(255), -1);
		}
		dilate(spiltmask[0], spiltmask[0], Mat());
		erode(spiltmask[0], spiltmask[0], Mat());
		erode(spiltmask[0], spiltmask[0], Mat());
		dilate(spiltmask[0], spiltmask[0], Mat());
		dilate(spiltmask[0], spiltmask[0], Mat());
		dilate(spiltmask[0], spiltmask[0], Mat());
		dilate(spiltmask[0], spiltmask[0], Mat());

		//spiltmask
		Mat spilt12;
		bitwise_xor(mask, spiltmask[0], spilt12);
		contours.clear();

		findContours(spilt12, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		if (!contours.empty())
		{
			int max = 0;
			for (int i = 0; i < contours.size(); i++)
				if (contourArea(contours[i]) > contourArea(contours[max]))
					max = i;
			int max2 = 0;
			for (int i = 0; i < contours.size(); i++)
				if (contourArea(contours[i]) > contourArea(contours[max2]) && i != max)
					max2 = i;
			drawContours(spiltmask[1], contours, max, Scalar(255), -1);
			drawContours(spiltmask[2], contours, max2, Scalar(255), -1);
		}
		dilate(spiltmask[1], spiltmask[1], Mat());
		erode(spiltmask[1], spiltmask[1], Mat());
		erode(spiltmask[1], spiltmask[1], Mat());
		dilate(spiltmask[1], spiltmask[1], Mat());
		dilate(spiltmask[2], spiltmask[2], Mat());
		erode(spiltmask[2], spiltmask[2], Mat());
		erode(spiltmask[2], spiltmask[2], Mat());
		dilate(spiltmask[2], spiltmask[2], Mat());
	}
	for (int j = 0; j < 3; j++)
	{
		input.copyTo(spiltans[j], spiltmask[j]);
		stringstream ss;
		ss << "ans";
		ss << j;
		imshow(ss.str(), spiltans[j]);
	}
	waitKey(0);
	return 0;
}

#pragma warning (default:4244)