#include "pxcprojection.h"
#include "pxcsensemanager.h"
#include "opencv2/opencv.hpp"

#pragma comment(lib,"opencv_world310d")

using namespace std;
using namespace cv;

int main()
{
	PXCSenseManager * psm = 0;
	psm = PXCSenseManager::CreateInstance();
	if (!psm) {
		wprintf_s(L"Unable to create the PXCSenseManager\n");
		system("pause");
	}

	psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480); // Enable color stream
	psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480); // Enable depth stream
	psm->Init();												// Initial device

	PXCCapture::Device * device = psm->QueryCaptureManager()->QueryDevice();
	pxcU16 invalid_value = device->QueryDepthLowConfidenceValue();
	PXCProjection * projection = NULL;
	projection = device->CreateProjection();

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

	PXCImage * color_image;
	PXCImage * depth_image;
	PXCImage * aligned_depth_image;

	PXCImage::ImageData data_color;
	PXCImage::ImageData data_depth;
	PXCImage::ImageData data_aligned_depth;

	for (;;)
	{
		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) break;

		PXCCapture::Sample * sample = psm->QuerySample();
		
		// retrieve the image or frame by type from the sample
		color_image = sample->color;
		depth_image = sample->depth;

		color_info = color_image->QueryInfo();
		depth_info = depth_image->QueryInfo();

		color_image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &data_color);
		depth_image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_depth);

		aligned_depth_image = projection->CreateDepthImageMappedToColor(depth_image, color_image);

		aligned_depth_image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_DEPTH, &data_aligned_depth);

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
		
		short * aligned_depht_data = (short*)data_aligned_depth.planes[0];
		for (int y = 0; y<480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				aligned_depth->imageData[y * 640 + x] = aligned_depht_data[y * 640 + x];
			}
		}

		cvShowImage("ColorImage", image);
		cvShowImage("DepthImage", depth);
		cvShowImage("AlignedDepthImage", aligned_depth);

		cvSaveImage("ColorImage.jpg", image);
		cvSaveImage("DepthImage.jpg", depth);

		color_image->ReleaseAccess(&data_color);
		depth_image->ReleaseAccess(&data_depth);
		aligned_depth_image->ReleaseAccess(&data_aligned_depth);

		if (cvWaitKey(10) >= 0)
			break;

		psm->ReleaseFrame();
	}

	cvReleaseImage(&image);
	cvReleaseImage(&depth);
	psm->Release();

	system("pause");
}