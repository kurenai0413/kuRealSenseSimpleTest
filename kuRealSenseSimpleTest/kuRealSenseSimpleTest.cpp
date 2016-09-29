#include <time.h>
#include "kuRealSenseHandler.h"

using namespace std;
using namespace cv;

kuRealSenseHandler RSHandler;

int main()
{
	//PXCSenseManager * psm = 0;
	//psm = PXCSenseManager::CreateInstance();
	//if (!psm) 
	//{
	//	wprintf_s(L"Unable to create the PXCSenseManager\n");
	//	system("pause");
	//}

	//psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480); // Enable color stream
	//psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480); // Enable depth stream
	//psm->Init();												// Initial device

	//PXCCapture::Device * device = psm->QueryCaptureManager()->QueryDevice();
	//pxcU16 invalid_value = device->QueryDepthLowConfidenceValue();
	//PXCProjection * projection = NULL;
	//projection = device->CreateProjection();

	bool aa = RSHandler.kuRSInitDevice();

	if (aa)
	{
		RSHandler.kuStreamingStart();
	}

	/*
	IplImage * image = 0;
	CvSize gab_size;
	gab_size.height = 480;
	gab_size.width = 640;
	image = cvCreateImage(gab_size, 8, 3);

	IplImage * depth = 0;
	depth = cvCreateImage(gab_size, 8, 1);

	IplImage * aligned_depth;
	aligned_depth = cvCreateImage(gab_size, 8, 1);

	
	PXCImage::ImageInfo color_info;
	PXCImage::ImageInfo depth_info;

	kuRSFrameBundle RSFrame;

	PXCImage::ImageData data_color;
	PXCImage::ImageData data_depth;
	PXCImage::ImageData data_aligned_depth;

	time_t StartT, EndT;

	for (;;)
	{
		StartT = clock();

		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) break;

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

		unsigned char	* rgb_data = data_color.planes[0];
		for (int y = 0; y<480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				int idx = 3 * (640 * y + x);

				image->imageData[idx]     = rgb_data[idx];
				image->imageData[idx + 1] = rgb_data[idx + 1];
				image->imageData[idx + 2] = rgb_data[idx + 2];
			}
		}

		
		short* depth_data = (short*)data_depth.planes[0]; //
		for (int y = 0; y<480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				depth->imageData[y * 640 + x] = depth_data[y * 640 + x];
			}
		}
		
		short * aligned_depth_data = (short*)data_aligned_depth.planes[0];
		for (int y = 0; y<480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				aligned_depth->imageData[y * 640 + x] = aligned_depth_data[y * 640 + x];
			}
		}

		cvShowImage("ColorImage", image);
		cvShowImage("DepthImage", depth);
		cvShowImage("AlignedDepthImage", aligned_depth);

		//cvSaveImage("ColorImage.jpg", image);
		//cvSaveImage("DepthImage.jpg", depth);

		RSFrame.ColorImg->ReleaseAccess(&data_color);
		RSFrame.DepthImg->ReleaseAccess(&data_depth);
		RSFrame.AlignedDepthImg->ReleaseAccess(&data_aligned_depth);

		EndT = clock();

		float pt = difftime(EndT, StartT)/ CLOCKS_PER_SEC;
		float fps = 1 / pt;

		cout << fps << endl;

		if (cvWaitKey(10) >= 0)
			break;

		psm->ReleaseFrame();
		RSFrame.AlignedDepthImg->Release();
	}

	cvReleaseImage(&image);
	cvReleaseImage(&depth);
	psm->Release();
	*/
	system("pause");
}