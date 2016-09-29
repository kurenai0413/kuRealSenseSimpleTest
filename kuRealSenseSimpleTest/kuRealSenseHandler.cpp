#include "kuRealSenseHandler.h"

kuRSFrameBundle::kuRSFrameBundle()
{
	ColorImg		= NULL;
	DepthImg		= NULL;
	AlignedDepthImg = NULL;

	ColorIplImg		   = NULL;
	DepthIplImg		   = NULL;
	AlignedDepthIplImg = NULL;

	createIplImgs();
}

kuRSFrameBundle::~kuRSFrameBundle()
{
	releaseBundle();
}


void kuRSFrameBundle::createIplImgs()
{
	CvSize gab_size;
	gab_size.width  = ImgResWidth;
	gab_size.height = ImgResHeight;
	
	ColorIplImg		   = cvCreateImage(gab_size, 8, 3);
	DepthIplImg		   = cvCreateImage(gab_size, 8, 1);
	AlignedDepthIplImg = cvCreateImage(gab_size, 8, 1);
}

void kuRSFrameBundle::releaseBundle()
{
	//releasePXCImg(ColorImg);
	//releasePXCImg(DepthImg);
	//releasePXCImg(AlignedDepthImg);

	if (ColorImg != NULL)
	{
		ColorImg->Release();
		ColorImg = NULL;
	}

	if (DepthImg != NULL)
	{
		DepthImg->Release();
		DepthImg = NULL;
	}

	if (AlignedDepthImg != NULL)
	{
		AlignedDepthImg->Release();
		AlignedDepthImg = NULL;
	}

	cvReleaseImage(&ColorIplImg);
	cvReleaseImage(&DepthIplImg);
	cvReleaseImage(&AlignedDepthIplImg);
}

void kuRSFrameBundle::releasePXCImg(PXCImage * Img)
{
	if (Img != NULL)
	{
		Img->Release();
		Img = NULL;
	}
}


kuRealSenseHandler::kuRealSenseHandler()
{
	psm			       = NULL;
	device		       = NULL;
	projection	       = NULL;
	isInitialized      = false;
	isCamStreamStarted = false;
}


kuRealSenseHandler::~kuRealSenseHandler()
{
	if (isCamStreamStarted)
	{
		kuStreamingStop();
	}

	if (isInitialized)
	{
		isInitialized = false;
	}
}

bool kuRealSenseHandler::kuRSInitDevice(void)
{
	pxcStatus sts;

	if (!isInitialized)
	{
		psm = PXCSenseManager::CreateInstance();
		if (!psm) 
		{
			wprintf_s(L"Unable to create the PXCSenseManager.\n");
			return false;
		}

		sts = psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, ImgResWidth, ImgResHeight); // Enable color stream
		if (sts != PXC_STATUS_NO_ERROR)
		{
			wprintf_s(L"Failed to enable color stream.\n");
			return false;
		}
		sts = psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, ImgResWidth, ImgResHeight); // Enable depth stream
		if (sts != PXC_STATUS_NO_ERROR)
		{
			wprintf_s(L"Failed to enable depth stream.\n");
			return false;
		}
		sts = psm->Init();
		if (sts != PXC_STATUS_NO_ERROR)
		{
			wprintf_s(L"Failed to initial PXCSenseManager.\n");
			return false;
		}

		device	      = psm->QueryCaptureManager()->QueryDevice();
		invalid_value = device->QueryDepthLowConfidenceValue();
		projection	  = device->CreateProjection();

		if (sts == PXC_STATUS_NO_ERROR)
		{
			isInitialized = true;
		}
		else
		{
			isInitialized = false;
		}
	}

	return isInitialized;
}

bool kuRealSenseHandler::kuStreamingStart(void)
{
	if (!isCamStreamStarted)
	{
		isCamStreamStarted = true;
	
		CamStreamThread = thread(&kuRealSenseHandler::CamStreamProc, this);
	}

	return isCamStreamStarted;
}

void kuRealSenseHandler::kuStreamingStop(void)
{
	isCamStreamStarted = false;
	CamStreamThread.join();
}

void kuRealSenseHandler::CamStreamProc()
{
	time_t StartT, EndT;

	while (isCamStreamStarted)
	{
		StartT = clock();
	
		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) 
			break;

		PXCCapture::Sample * sample = psm->QuerySample();

		// retrieve the image or frame by type from the sample
		RSFrame.ColorImg = sample->color;
		RSFrame.DepthImg = sample->depth;

		color_info = RSFrame.ColorImg->QueryInfo();
		depth_info = RSFrame.DepthImg->QueryInfo();

		RSFrame.ColorImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &data_color);
		RSFrame.DepthImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_depth);

		RSFrame.AlignedDepthImg = projection->CreateDepthImageMappedToColor(RSFrame.DepthImg, RSFrame.ColorImg);
		RSFrame.AlignedDepthImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_aligned_depth);

		/* Change PXCImage to IplImage  */
		//PXCImgData2IplImg(data_color, RSFrame.ColorIplImg);

		thread ColorIplThread		 = thread(&kuRealSenseHandler::ColorImgData2IplImg, this, data_color, RSFrame.ColorIplImg);
		thread DepthIplThread		 = thread(&kuRealSenseHandler::DepthImgData2IplImg, this, data_depth, RSFrame.DepthIplImg);
		thread AlignedDepthIplThread = thread(&kuRealSenseHandler::DepthImgData2IplImg, this, data_aligned_depth, RSFrame.AlignedDepthIplImg);
		ColorIplThread.join();
		DepthIplThread.join();
		AlignedDepthIplThread.join();

		cvShowImage("Color", RSFrame.ColorIplImg);
		cvShowImage("Depth", RSFrame.DepthIplImg);
		cvShowImage("Aligned Depth", RSFrame.AlignedDepthIplImg);
		waitKey(10);

		RSFrame.ColorImg->ReleaseAccess(&data_color);
		RSFrame.DepthImg->ReleaseAccess(&data_depth);
		RSFrame.AlignedDepthImg->ReleaseAccess(&data_aligned_depth);
		
		psm->ReleaseFrame();
		RSFrame.AlignedDepthImg->Release();

		EndT = clock();

		float pt = difftime(EndT, StartT) / CLOCKS_PER_SEC;
		float fps = 1 / pt;

		cout << fps << endl;
	}
}

void kuRealSenseHandler::ColorImgData2IplImg(const PXCImage::ImageData &ColorData, IplImage * IplImg)
{
	unsigned char	* rgb_data = ColorData.planes[0];
	for (int y = 0; y<ImgResHeight; y++)
	{
		for (int x = 0; x<ImgResWidth; x++)
		{
			int idx = 3 * (ImgResWidth * y + x);

			IplImg->imageData[idx]     = rgb_data[idx];
			IplImg->imageData[idx + 1] = rgb_data[idx + 1];
			IplImg->imageData[idx + 2] = rgb_data[idx + 2];
		}
	}
}

void kuRealSenseHandler::DepthImgData2IplImg(const PXCImage::ImageData & DepthData, IplImage * IplImg)
{
	short* depth_data = (short*)DepthData.planes[0]; //
	for (int y = 0; y < ImgResHeight; y++)
	{
		for (int x = 0; x < ImgResWidth; x++)
		{
			IplImg->imageData[y * ImgResWidth + x] = depth_data[y * ImgResWidth + x];
		}
	}
}


