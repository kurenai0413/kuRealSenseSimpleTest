#include "kuRealSenseHandler.h"



kuRealSenseHandler::kuRealSenseHandler()
{
	isInitialized = false;
}


kuRealSenseHandler::~kuRealSenseHandler()
{
}


bool kuRealSenseHandler::kuRSInitDevice()
{
	if (!isInitialized)
	{
		psm = PXCSenseManager::CreateInstance();
		if (!psm) {
			wprintf_s(L"Unable to create the PXCSenseManager\n");
			system("pause");
		}

		psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480); // Enable color stream
		psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480); // Enable depth stream
		psm->Init();
	}

	return isInitialized;
}
