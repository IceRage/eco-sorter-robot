#ifndef _CANNY_TRACKBAR_H
#define _CANNY_TRACKBAR_H

#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace std;

// Constants
const short int	thresholdLimit	= 100;
const char			*windowName			= "Trackbar for Canny Threshold";

// Functions
void onTrackbarChange(int position);
IplImage* convertBGRToGrayscale(IplImage* sourceImage);
IplImage* applyCannyToImage(IplImage* sourceImage);

#endif