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

	system("pause");
}