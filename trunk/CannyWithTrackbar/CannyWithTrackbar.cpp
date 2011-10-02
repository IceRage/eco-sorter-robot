#include "CannyWithTrackbar.h"

// Global variables
int	lowThreshold	= 50;
int	highThreshold	= 150;

// Event handler when trackbar position changes
void onTrackbarChange(int position) {
	lowThreshold	= position;
	highThreshold	= 3 * lowThreshold;
}

// Convert a Color image to a Grayscale image
IplImage* convertBGRToGrayscale(IplImage* sourceImage) {
	IplImage *grayFrame	= cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

	cvConvertImage(sourceImage, grayFrame, 0);

	return grayFrame;
}

// Apply Canny edge detection to a grayscale image
IplImage* applyCannyToImage(IplImage* sourceImage) {
	IplImage *cannyFrame	= cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

	cvCanny(sourceImage, cannyFrame, lowThreshold, highThreshold, 3);

	return cannyFrame;
}

// Main function
int main( int argc, char **argv ) {
	CvCapture *capture	= 0;
	IplImage  *frame		= 0;
	int       key				= 0;

	// Initialize the camera
	capture = cvCaptureFromCAM(0);

	if ( !capture ) {
		fprintf(stderr, "Cannot initialize webcam!\n");
		return 1;
	}

	// Create the window and the associated trackbar
	cvNamedWindow		(windowName, CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("trackbar", windowName, &lowThreshold, thresholdLimit, onTrackbarChange);

	// While ESC was not pressed, continue
	while	(key != 27) {
		// Get a frame from the webcam
		frame = cvQueryFrame(capture);

		if(!frame)
			break;

		// Apply Canny to the input frame
		IplImage *grayFrame		= convertBGRToGrayscale(frame);
		IplImage *cannyFrame	= applyCannyToImage(grayFrame);

		cvReleaseImage(&grayFrame);

		// Show the modified frame
		cvShowImage(windowName, cannyFrame);

		cvReleaseImage(&cannyFrame);

		key = cvWaitKey(1);
	}

	// Cleanup
	cvDestroyWindow(windowName);
	cvReleaseCapture(&capture);

	return 0;
}