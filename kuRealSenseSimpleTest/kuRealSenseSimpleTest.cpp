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

	IplImage *image = 0;
	CvSize gab_size;
	gab_size.height = 480;
	gab_size.width = 640;
	image = cvCreateImage(gab_size, 8, 3);

	IplImage *depth = 0;
	depth = cvCreateImage(gab_size, 8, 1);

	PXCImage::ImageData ColorData;
	PXCImage::ImageData data_depth;

	unsigned char	*	rgb_data;
	//float			*	depth_data;

	PXCImage::ImageInfo rgb_info;
	PXCImage::ImageInfo depth_info;

	for (;;)
	{
		if (psm->AcquireFrame(true)<PXC_STATUS_NO_ERROR) break;

		PXCCapture::Sample *sample = psm->QuerySample();
		PXCImage *colorIm, *depthIm;

		// retrieve the image or frame by type from the sample
		colorIm = sample->color;
		depthIm = sample->depth;

		PXCImage *color_image = colorIm;
		PXCImage *depth_image = depthIm;

		PXCImage::ImageInfo dinfo = depth_image->QueryInfo();
		PXCPointF32 * uvmap = new PXCPointF32[dinfo.width*dinfo.height];

		color_image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB24, &ColorData);
		depth_image->AcquireAccess(PXCImage::ACCESS_READ, &data_depth);

		rgb_data = ColorData.planes[0];
		int cwidth = ColorData.pitches[0] / sizeof(int); /* aligned width */
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
		int dpitch = data_depth.pitches[0] / sizeof(short); /* aligned width */
		for (int y = 0; y<480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				depth->imageData[y * 640 + x] = depth_data[y * 640 + x];
			}
		}
		
		int uvpitch = depth_image->QueryInfo().width;

		pxcStatus sts = projection->QueryUVMap(depth_image, uvmap);

		IplImage * AlignedImage;
		AlignedImage = cvCreateImage(CvSize(640,480),IPL_DEPTH_8U,1);
		
		for (int y = 0; y < AlignedImage->height; y++)
		{
			for (int x = 0; x < AlignedImage->width; x++)
			{
				short d = depth_data[dpitch * y + x];

				if (d != invalid_value)
				{
					float uvx = uvmap[y*uvpitch + x].x;
					float uvy = uvmap[y*uvpitch + x].y;

					if (uvx != -1 && uvy != -1)
					{
						cout << "DD" << endl;
					}
				}
			}
		}

		cvShowImage("ColorImage", image);
		cvShowImage("DepthImage", depth);

		cvSaveImage("ColorImage.jpg", image);
		cvSaveImage("DepthImage.jpg", depth);

		color_image->ReleaseAccess(&ColorData);
		depth_image->ReleaseAccess(&data_depth);

		if (cvWaitKey(10) >= 0)
			break;

		psm->ReleaseFrame();

	}

	//CalibrateColorAndDepthStream(PXCMCapture.Device device, PXCMImage depth, PXCMImage color, out PXCMImage output)
	//{
	//	PXCMProjection projection = device.CreateProjection();
	//	output = projection.CreateDepthImageMappedToColor(depth, color);
	//	projection.Dispose();
	//}



	cvReleaseImage(&image);
	cvReleaseImage(&depth);
	psm->Release();

	system("pause");
}