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
			cvShowImage("Color", RSHandler.RSFrame->ColorIplImg);
			cvShowImage("Depth", RSHandler.RSFrame->DepthIplImg);
			cvShowImage("AlignedDepthImage", RSHandler.RSFrame->AlignedDepthIplImg);
			cvWaitKey(10);
		}
	}

	system("pause");
}