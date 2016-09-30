#include <time.h>
#include "kuRealSenseHandler.h"

using namespace std;
using namespace cv;

kuRealSenseHandler RSHandler;

int main()
{
	bool RSInitialed = RSHandler.kuRSInitDevice();

	if (RSInitialed)
	{
		RSHandler.kuStreamingStart();
	}

	for (;;)
	{
		if (RSHandler.isCopyDone)
		{
			imshow("Color", RSHandler.ColorCVImg);
			imshow("Depth", RSHandler.DepthCVImg);
			imshow("Aligned Depth", RSHandler.AlignedDepthCVImg);
			cvWaitKey(10);
		}
	}

	system("pause");
}