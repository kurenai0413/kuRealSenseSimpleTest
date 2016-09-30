#include "kuRealSenseHandler.h"

kuRSFrameBundle::kuRSFrameBundle()
{
	ColorImg		= NULL;
	DepthImg		= NULL;
	AlignedDepthImg = NULL;
}

kuRSFrameBundle::~kuRSFrameBundle()
{
	releaseBundle();
}

void kuRSFrameBundle::releaseBundle()
{
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
}

kuRealSenseHandler::kuRealSenseHandler()
{
	psm			       = NULL;
	device		       = NULL;
	projection	       = NULL;
	isInitialized      = false;
	isCamStreamStarted = false;

	RSFrame = new kuRSFrameBundle;

	ColorCVImg.create(ImgResHeight, ImgResWidth, CV_8UC3);
	DepthCVImg.create(ImgResHeight, ImgResWidth, CV_8UC1);
	AlignedDepthCVImg.create(ImgResHeight, ImgResWidth, CV_8UC1);
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
	while (isCamStreamStarted)
	{
		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) 
			break;

		PXCCapture::Sample * sample = psm->QuerySample();

		// retrieve the image or frame by type from the sample
		RSFrame->ColorImg = sample->color;
		RSFrame->DepthImg = sample->depth;

		RSFrame->ColorImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &data_color);
		RSFrame->DepthImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_depth);

		RSFrame->AlignedDepthImg = projection->CreateDepthImageMappedToColor(RSFrame->DepthImg, RSFrame->ColorImg);
		RSFrame->AlignedDepthImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_aligned_depth);

		/* Change PXCImage to IplImage */
		//PXCImgData2IplImg(data_color, RSFrame.ColorIplImg);

		isCopyDone = false;

		// 這 樣 寫 對 效 能 幾 乎 沒 影 響 啦 幹 你 喵 喵 的
		thread ColorIplThread		 = thread(&kuRealSenseHandler::ColorImgData2IplImg, this, data_color, ColorCVImg);
		thread DepthIplThread		 = thread(&kuRealSenseHandler::DepthImgData2IplImg, this, data_depth, DepthCVImg);
		thread AlignedDepthIplThread = thread(&kuRealSenseHandler::DepthImgData2IplImg, this, data_aligned_depth, AlignedDepthCVImg);
		ColorIplThread.join();
		DepthIplThread.join();
		AlignedDepthIplThread.join();

		isCopyDone = true;

		RSFrame->ColorImg->ReleaseAccess(&data_color);
		RSFrame->DepthImg->ReleaseAccess(&data_depth);
		RSFrame->AlignedDepthImg->ReleaseAccess(&data_aligned_depth);
		
		psm->ReleaseFrame();
		RSFrame->AlignedDepthImg->Release();
	}
}

void kuRealSenseHandler::ColorImgData2IplImg(const PXCImage::ImageData &ColorData, Mat &CVImg)
{
	unsigned char	* rgb_data = ColorData.planes[0];
	memcpy(CVImg.data, rgb_data, 3 * ImgResWidth * ImgResHeight * sizeof(uchar));
}

void kuRealSenseHandler::DepthImgData2IplImg(const PXCImage::ImageData & DepthData, Mat &CVImg)
{
	short* depth_data = (short*)DepthData.planes[0]; //
	for (int y = 0; y < ImgResHeight; y++)
	{
		for (int x = 0; x < ImgResWidth; x++)
		{
			CVImg.data[y * ImgResWidth + x] = depth_data[y * ImgResWidth + x];
		}
	}
}


