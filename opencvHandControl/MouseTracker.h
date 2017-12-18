#pragma once

//class for synchronizing the mouse position and the finger locations
//detected from the camera
class MouseTracker
{
private:
	static int screenWidth, screenHeight, imgWidth, imgHeight,speedConst,fitConst,clickDistConst;
	static float xCoeff, yCoeff;
public:
	MouseTracker();
	~MouseTracker();
	static bool syncMousePos(const std::vector<std::pair<cv::KeyPoint, int> >& points);
	static int pixelDist(int x1, int y1, int x2, int y2);
	static void leftClick();
	static void rightClick();
	static void setImgDim(int width, int height);
	static void initalize();
};

