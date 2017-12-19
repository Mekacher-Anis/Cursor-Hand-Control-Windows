#include "stdafx.h"
#include "BlobDetector.h"


BlobDetector::BlobDetector()
{
}


BlobDetector::~BlobDetector()
{
}

//main blob detetction function, takes img and detects the blobs 
//based on the RGB values set in the static 
//variables of the class

std::vector<std::pair<cv::KeyPoint, int> > BlobDetector::detect(cv::Mat& img) {
	std::vector<std::pair<cv::KeyPoint, int> > points; //holds the result
	std::vector<Blob> newBlobs; //holds the blobs found
	//scans a grid of pixels based on the segmentation constant
	for (size_t y = 0; y < img.rows; y += SEGMENTATIONCONST)
	{
		for (size_t x = 0; x < img.cols; x += SEGMENTATIONCONST)
		{
			//cv::Vec3b& pixel = img.at<cv::Vec3b>(y, x); //the rgb values of the pixel
			cv::Vec3b& pixel = img.at<cv::Vec3b>(y, x);
			convertPixel2HLS(pixel, img);
			//if (colorDif(pixel) <= COLORTHRESHOLD) {
			//	add2Blob(x, y, newBlobs); //add pixel if it has the same color
			//}
			if (hasDesiredColor(pixel)) {
				//std::printf("Found pixel at (%d , %d)\n", x, y);
				add2Blob(x, y, newBlobs); //add pixel if it has the same color
				pixel[0] = pixel[1] = pixel[2] = 0;
			}
		}
	}
	cleanBlobs(newBlobs); //remove any small blobs
	syncBlobs(newBlobs); //sync the old blobs with the new blobs
	Blob::reset(); //resets the counter of blobs
	//std::printf("found %d blobs\n", newBlobs.size());
	//convert blobs to keypoints
	for each (Blob blob in newBlobs) {
		points.push_back(std::make_pair(*(new cv::KeyPoint(blob.centerX, blob.centerY, MAXDISTTHRESHOLD / 75)), blob.getId()));
	}
	//only sync the old blobs if the new detected blobs are more than 3
	if (points.size() >= 3) {
		DrawTriangle(img, points);
		oldBlobs = std::move(newBlobs);
	}
	
	return points;
}

//uses euclidean distance to return the color deffirence sqrd

int BlobDetector::colorDif(const cv::Vec3b & pixel)
{
	return abs((pixel[0] - B)*(pixel[0] - B) + (pixel[1] - G)*(pixel[1] - G) + (pixel[2] - R)*(pixel[2] - R));
}

void BlobDetector::convertPixel2HLS(cv::Vec3b & pixel,const cv::Mat& img)
{
	cv::Vec3b pixelImg[1][1] = { {pixel} };
	cv::Mat newImg(1, 1, img.type(), pixelImg);
	cv::Mat temp;
	cv::cvtColor(newImg, temp, cv::COLOR_BGR2HLS);
	pixel = temp.at<cv::Vec3b>(0, 0);
}

//NLIU function : sacns the sqr spicefied by "bx" and "by"
//for the pixels that are part of the blob

void BlobDetector::scanSqr(cv::Mat& img, int by, int bx)
{
	for (int y = by - MAXDISTTHRESHOLD / 2; y < by + MAXDISTTHRESHOLD && y < img.rows; y++)
	{
		for (int x = bx - MAXDISTTHRESHOLD / 2; x < bx + MAXDISTTHRESHOLD && x < img.cols; x++)
		{
			cv::Vec3b& pixel = img.at<cv::Vec3b>(y, x);
			if (colorDif(pixel) <= COLORTHRESHOLD) {
				pixel[0] = pixel[1] = pixel[2] = 0;
				//				add2Blob(x, y);
			}
		}
	}
}

//returns the difference between two pixels squared

int BlobDetector::pixelDist(int x1, int y1, int x2, int y2)
{
	return ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

//searches for the best blob match for the selecetd pixerl based on distance

void BlobDetector::add2Blob(int x, int y, std::vector<Blob>& blobs) {
	//check if there are blobs already
	if (blobs.size() != 0) {
		//search for the closest blob
		std::vector<Blob>::iterator selected;
		int bestDist = 100000000;
		for (std::vector<Blob>::iterator blob = blobs.begin(); blob != blobs.end(); ++blob) {
			int dist = pixelDist(x, y, blob->centerX, blob->centerY);
			if (dist < bestDist) {
				selected = blob;
				bestDist = dist;
			}
		}
		//add to blob if it's close enough
		if (bestDist < MAXDISTTHRESHOLD) {
			selected->addXY(x, y);
			return;
		}
	}
	//if there are no blobs or the pixel isn't part of any blob create new one
	if (blobs.size() >= 3) return;
	Blob newBlob(x, y);
	newBlob.addXY(x, y);
	blobs.push_back(newBlob);
}

//function to sync old blobs IDs with the new ones
void BlobDetector::syncBlobs(std::vector<Blob>& blobs)
{
	//std::printf("num of old blobs = %d num of new blobs = %d\n", oldBlobs.size(), blobs.size());
	if (oldBlobs.size() >= 3 && blobs.size()>=3) {
		for (std::vector<Blob>::iterator blob = blobs.begin(); blob != blobs.end(); ++blob) {
			std::vector<Blob>::iterator selected;
			int bestDist = 100000000;
			for(std::vector<Blob>::iterator oldBlob = oldBlobs.begin(); oldBlob != oldBlobs.end(); ++oldBlob)
			{
				int dist = pixelDist(oldBlob->centerX, oldBlob->centerY, blob->centerX, blob->centerY);
				if (dist < bestDist) {
					selected = oldBlob;
					bestDist = dist;
				}
			}
			blob->setId(selected->getId());
			oldBlobs.erase(selected);
		}
	}
}

//removes any blobs that have less the chosen number of pixels

void BlobDetector::cleanBlobs(std::vector<Blob>& blobs)
{
	for (std::vector<Blob>::iterator pt = blobs.begin(); pt != blobs.end();)
		if (pt->getNum() < MINPONTSOFBLOB)
			pt = blobs.erase(pt);
		else ++pt;
}

//draws trianle for the detected blobs
void BlobDetector::DrawTriangle(cv::Mat & img, const std::vector<std::pair<cv::KeyPoint, int> >& blobs)
{
	cv::Point pt[3] = { blobs[0].first.pt,blobs[1].first.pt, blobs[2].first.pt };
	cv::fillConvexPoly(img, pt, 3, cv::Scalar(0, 150, 0));
	for each (std::pair<cv::KeyPoint, int> blob in blobs)
		cv::putText(img, std::to_string(blob.second), blob.first.pt, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(50, 255, 120),5);
}

bool BlobDetector::hasDesiredColor(const cv::Vec3b& pixel)
{
	return ((pixel[0] >= H - HUETHRESHOLD) && (pixel[0] <= H + HUETHRESHOLD) && (pixel[1] >= 25 && pixel[1] <= 235) && (pixel[2] >= S - 30));
}


int Blob::SIZETHRESHOLD = 200; //NLIU : the smallest size of blob
int Blob::numberOfBlobs = 0; //NLIU : the total number of blobs

int BlobDetector::SEGMENTATIONCONST = 10; //the size of the grid units to scan
int BlobDetector::COLORTHRESHOLD = 2000; //the max color diff to be considered a blob
int BlobDetector::MAXDISTTHRESHOLD = 2000; //the max dist to consider a pixel part of blob 2000
int BlobDetector::MINDISTTHRESHOLD = 100; //NLIU : min distance between blobs
int BlobDetector::MINPONTSOFBLOB = 2; //the min points that makes a blob
int BlobDetector::R = -1000; //the red value of the chosen color
int BlobDetector::G = -1000; //the green value of the chosen color
int BlobDetector::B = -1000; //the blue value of the chosen color

float BlobDetector::H = -1000; //the hue value of the chosen color
float BlobDetector::L = -1000; // NOT REALLY NEEDED the lightness value of the chosen color
float BlobDetector::S = -1000; // NOT REALLY NEEDED the saturation value of the chosen color
int BlobDetector::HUETHRESHOLD = 15;

std::vector<Blob> BlobDetector::oldBlobs;