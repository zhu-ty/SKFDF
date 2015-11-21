//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.11.9
//Use Ctrl+M,Ctrl+O to fold the code.

#pragma once

#include "SKImageDisplayer.h"
#include <mutex>
#include <Windows.h>

using namespace std;
using namespace cv;

class SKImageDisplayerImpl
{
public:
	struct Dthread
	{
		IplImage **p;
		cv::Mat *m;
		bool stop;
		string name;
		mutex lock;
	};

	bool shown;
	HANDLE hThread;
	Dthread dth;
};

namespace thread
{
	DWORD WINAPI Displayimg(LPVOID lpParamter)
	{
		SKImageDisplayerImpl::Dthread *dp = (SKImageDisplayerImpl::Dthread *)lpParamter;
		while (1)
		{
			IplImage *pth = *(dp->p);
			if (pth != NULL)
			{
				try
				{
					cvShowImage(dp->name.c_str(), pth);
					cvResizeWindow(dp->name.c_str(), pth->width, pth->height);
				}
				catch (...)
				{
				}
				cvWaitKey(100);
			}
			dp->lock.lock();
			if (dp->stop == true)
				break;
			dp->lock.unlock();
		}
		dp->lock.unlock();
		cvDestroyWindow(dp->name.c_str());
		return 0;
	}

	DWORD WINAPI Displaymat(LPVOID lpParamter)
	{
		SKImageDisplayerImpl::Dthread *dp = (SKImageDisplayerImpl::Dthread *)lpParamter;
		cvWaitKey(200);
		while (1)
		{
			Mat m= *(dp->m);
			if (!m.empty())
			{
				try
				{
					imshow(dp->name.c_str(), m);
				}
				catch (...)
				{
					dp->lock.lock();
					dp->stop = true;
					dp->lock.unlock();
				}
				cvWaitKey(200);
			}
			dp->lock.lock();
			if (dp->stop == true)
				break;
			dp->lock.unlock();
		}
		dp->lock.unlock();
		cv::destroyWindow(dp->name.c_str());
		return 0;
	}
}

SKImageDisplayer::SKImageDisplayer()
{
	_impl = new SKImageDisplayerImpl();
	_impl->shown = false;
	_impl->hThread = nullptr;
	_impl->dth.name = "My image";
	_impl->dth.p = nullptr;
	_impl->dth.stop = false;
}

SKImageDisplayer::~SKImageDisplayer()
{
	delete _impl;
}

void SKImageDisplayer::display(IplImage **p)
{
	display(p, "Image");
}

void SKImageDisplayer::display(cv::Mat *m)
{
	display(m, "Image");
}

void SKImageDisplayer::display(IplImage **p, const char *name)
{
	if (_impl->shown == false)
	{
		string c(name);
		_impl->dth.name = c;
		_impl->dth.p = p;
		_impl->dth.stop = false;
		_impl->hThread = CreateThread(NULL, 0, thread::Displayimg, &(_impl->dth), 0, NULL);
	}
	_impl->shown = true;
}

void SKImageDisplayer::display(cv::Mat *m, const char *name)
{
	if (_impl->shown == false)
	{
		string c(name);
		_impl->dth.name = c;
		_impl->dth.m = m;
		_impl->dth.stop = false;
		_impl->hThread = CreateThread(NULL, 0, thread::Displaymat, &(_impl->dth), 0, NULL);
	}
	_impl->shown = true;
}

void SKImageDisplayer::hide()
{
	if (_impl->shown == true)
	{
		_impl->dth.lock.lock();
		_impl->dth.stop = true;
		_impl->dth.lock.unlock();
		cvWaitKey(200);
	}
	_impl->shown = false;
}