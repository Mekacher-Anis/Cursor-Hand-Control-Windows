#include "stdafx.h"
#include "MouseTracker.h"


MouseTracker::MouseTracker()
{
}


MouseTracker::~MouseTracker()
{
}

//synchronizes the blobs positions with the mouse
bool MouseTracker::syncMousePos(const std::vector<std::pair<cv::KeyPoint, int> >& points)
{
	//if it dtects 3 fingers sync mouse position
	if (points.size() >= 3) {
		//check if there's is click
		cv::KeyPoint thumb = std::find_if(points.begin(), points.end(), [](const std::pair<cv::KeyPoint, int>& pt) { return pt.second == 0; })->first;
		cv::KeyPoint index = std::find_if(points.begin(), points.end(), [](const std::pair<cv::KeyPoint, int>& pt) { return pt.second == 1; })->first;
		cv::KeyPoint middle = std::find_if(points.begin(), points.end(), [](const std::pair<cv::KeyPoint, int>& pt) { return pt.second == 2; })->first;
		if (pixelDist(thumb.pt.x, thumb.pt.y, index.pt.x, index.pt.y) < clickDistConst)
			leftClick();
		else if (pixelDist(thumb.pt.x, thumb.pt.y, middle.pt.x, middle.pt.y) < clickDistConst)
			rightClick();


		//calculate center position
		int centerPosX = points[0].first.pt.x, centerPosY = points[0].first.pt.y;
		for (size_t i = 1; i < points.size(); i++)
		{
			centerPosX = (centerPosX + points[i].first.pt.x) / 2;
			centerPosY = (centerPosY + points[i].first.pt.y) / 2;
		}
		//removes a constant from the center value so the muse cursor reaches the corner
		//before the center of the triangle (blobs) reaches the corner 
		centerPosX = (centerPosX - fitConst < 0) ? 0 : centerPosX - fitConst;
		centerPosY = (centerPosY - fitConst < 0) ? 0 : centerPosY - fitConst;
		//set the cursor position
		SetCursorPos(centerPosX*xCoeff*speedConst, centerPosY*yCoeff*speedConst);
	}
	else {
		//std::printf("Couldn't detect three fingers !! u must have at least three fingers\n");
		return false;
	}
}

//returns the difference between two pixels squared
int MouseTracker::pixelDist(int x1, int y1, int x2, int y2)
{
	return ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

void MouseTracker::leftClick()
{
	INPUT Input = { 0 };

	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
	std::printf("left Click !!\n");
}

void MouseTracker::rightClick()
{
	INPUT Input = { 0 };

	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));
	std::printf("right Click !!\n");
}

//sets the webcam dimensions
void MouseTracker::setImgDim(int width, int height)
{
	imgWidth = width;
	imgHeight = height;
	std::printf("Webcam Resolution : %d x %d\n", imgWidth, imgHeight);
	std::printf("Screen Resolution : %d x %d\n", screenWidth, screenHeight);
}

//gets the screen dimensions and initialzes teh static variables
void MouseTracker::initalize()
{
	xCoeff = ((float)screenWidth) / (imgWidth - 2 * fitConst);
	yCoeff = ((float)screenHeight) / (imgHeight - 2 * fitConst);
	std::printf("xCoeff = %g yCoeff = %g\n", xCoeff, yCoeff);
}

int MouseTracker::screenWidth = GetSystemMetrics(SM_CXSCREEN);
int MouseTracker::screenHeight = GetSystemMetrics(SM_CYSCREEN);
int MouseTracker::imgWidth;
int MouseTracker::imgHeight;
int MouseTracker::speedConst = 1;
int MouseTracker::fitConst = 110;
int MouseTracker::clickDistConst = 3700;
float MouseTracker::xCoeff;
float MouseTracker::yCoeff;
