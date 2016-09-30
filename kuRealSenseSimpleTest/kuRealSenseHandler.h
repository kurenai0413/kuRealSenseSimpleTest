#pragma once
#include "pxcprojection.h"
#include "pxcsensemanager.h"
#include "opencv2/opencv.hpp"
#include <thread>

#pragma comment(lib, "opencv_world310d")

using namespace cv;
using namespace std;

#define ImgResWidth  640
#define ImgResHeight 480

class kuRSFrameBundle
{
public:

	PXCImage * ColorImg;
	PXCImage * DepthImg;
	PXCImage * AlignedDepthImg;

	kuRSFrameBundle();
	~kuRSFrameBundle();

	void releaseBundle();
};

class kuRealSenseHandler
{
private:

	PXCSenseManager		*	psm;
	PXCCapture::Device	*	device;
	PXCProjection		*	projection;
	pxcU16					invalid_value;

	PXCImage::ImageData		data_color;
	PXCImage::ImageData		data_depth;
	PXCImage::ImageData		data_aligned_depth;

	thread					CamStreamThread;

	bool					isCamStreamStarted;
	
	void					CamStreamProc();
	void					ColorImgData2IplImg(const PXCImage::ImageData &ColorData, Mat &CVImg);
	void					DepthImgData2IplImg(const PXCImage::ImageData &DepthData, Mat &IplImg);

public:

	kuRealSenseHandler();
	~kuRealSenseHandler();

	kuRSFrameBundle		*	RSFrame;

	Mat						ColorCVImg;
	Mat						DepthCVImg;
	Mat						AlignedDepthCVImg;

	bool					isInitialized;
	bool					isCopyDone;

	bool					kuRSInitDevice(void);
	bool					kuStreamingStart(void);
	void					kuStreamingStop(void);
};