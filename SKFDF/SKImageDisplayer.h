//SKImageDisplayer
//Display a image.
//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.11.9
//Use Ctrl+M,Ctrl+O to fold the code.

#pragma once

#include <iostream>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "SKDefinition.h"
using namespace std;


class SKImageDisplayer
{
public:
	SKImageDisplayer();
	~SKImageDisplayer();
	void display(IplImage **p);
	void display(IplImage **p, const char *name);
	void display(cv::Mat *m);
	void display(cv::Mat *m, const char *name);
	void hide();
private:
	class SKImageDisplayerImpl *_impl;
};