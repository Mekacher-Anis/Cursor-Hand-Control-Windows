#pragma once
#include "stdafx.h"


class Blob
{
private:
	int Xs[50] = { 0 };
	int Ys[50] = { 0 };
	static int SIZETHRESHOLD;
	static int numberOfBlobs;
	int num = 0;
	int id = 0;
public:
	int centerX;
	int centerY;
	Blob() = default;
	Blob(int _x, int _y) : id(numberOfBlobs++)
	{
		centerX = _x;
		centerY = _y;
	}
	int getId()const { return id; }
	int getNum()const { return num; }
	void setId(int _id) { id = _id; }
	static void reset() { numberOfBlobs = 0; }
	void addXY(int _x, int _y)
	{
		if (num >= SIZETHRESHOLD) return;
		centerX = (centerX + _x) / 2;
		Xs[num] = _x;
		centerY = (centerY + _y) / 2;
		Ys[num++] = _y;
	}
};

class BlobDetector
{
private:
	static int SEGMENTATIONCONST, COLORTHRESHOLD, MAXDISTTHRESHOLD, MINDISTTHRESHOLD, MINPONTSOFBLOB, HUETHRESHOLD, REFRESHDIST, AVRGDIST;
	static std::vector<Blob> oldBlobs;
public:
	static int R, G, B;
	static float H, L, S;
	BlobDetector();
	~BlobDetector();
	static std::vector<std::pair<cv::KeyPoint, int> > detect(cv::Mat& img);
	static int colorDif(const cv::Vec3b& pixel);
	static void convertPixel2HLS(cv::Vec3b& pixel, const cv::Mat& img);
	static void scanSqr(cv::Mat& img, int x, int y);
	static int pixelDist(int x1, int y1, int x2, int y2);
	static void add2Blob(int x, int y, std::vector<Blob>& blobs);
	static void syncBlobs(std::vector<Blob>& blobs);
	static void cleanBlobs(std::vector<Blob>& blobs);
	static void DrawTriangle(cv::Mat& img, const std::vector<std::pair<cv::KeyPoint, int> >& points);
	static bool hasDesiredColor(const cv::Vec3b& pixel);
};

