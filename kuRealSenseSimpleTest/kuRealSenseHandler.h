#pragma once
#include "pxcprojection.h"
#include "pxcsensemanager.h"
#include "opencv2/opencv.hpp"

#pragma comment(lib, "opencv_world310d")

using namespace cv;

struct kuRSFrameBundle
{
	PXCImage * ColorImg;
	PXCImage * DepthImg;
	PXCImage * AlignedDepthImg;
};

class kuRealSenseHandler
{
private:
	PXCSenseManager * psm;

public:
	kuRealSenseHandler();
	~kuRealSenseHandler();

	bool	isInitialized;
	bool	kuRSInitDevice();
};

