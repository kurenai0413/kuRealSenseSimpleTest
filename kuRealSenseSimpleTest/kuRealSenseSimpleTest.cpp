#include <time.h>
#include "kuRealSenseHandler.h"

using namespace std;
using namespace cv;

kuRealSenseHandler RSHandler;

int main()
{
	bool aa = RSHandler.kuRSInitDevice();

	if (aa)
	{
		RSHandler.kuStreamingStart();
	}

	system("pause");
}