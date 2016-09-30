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

	IplImage * ColorIplImg;
	IplImage * DepthIplImg;
	IplImage * AlignedDepthIplImg;

	kuRSFrameBundle();
	~kuRSFrameBundle();

	void createIplImgs();
	void releaseBundle();
	//void releasePXCImg(PXCImage * Img);
};

class kuRealSenseHandler
{
private:

	PXCSenseManager		*	psm;
	PXCCapture::Device	*	device;
	PXCProjection		*	projection;
	pxcU16					invalid_value;

	PXCImage::ImageInfo		color_info;
	PXCImage::ImageInfo		depth_info;

	PXCImage::ImageData		data_color;
	PXCImage::ImageData		data_depth;
	PXCImage::ImageData		data_aligned_depth;

	thread					CamStreamThread;

	bool					isCamStreamStarted;
	
	void					CamStreamProc();
	void					ColorImgData2IplImg(const PXCImage::ImageData &ColorData, IplImage * IplImg);
	void					DepthImgData2IplImg(const PXCImage::ImageData &DepthData, IplImage * IplImg);

public:

	kuRealSenseHandler();
	~kuRealSenseHandler();

	kuRSFrameBundle		*	RSFrame;

	bool	isInitialized;
	bool	isCopyDone;

	bool	kuRSInitDevice(void);
	bool	kuStreamingStart(void);
	void	kuStreamingStop(void);
};