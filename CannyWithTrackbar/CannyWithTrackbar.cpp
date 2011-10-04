#include "CannyWithTrackbar.h"

// Global variables
int	lowThreshold	= 50;
int	highThreshold	= 150;

// Run the program
int run() {
	CvCapture *capture	= 0;

	// Initialize the camera
	capture = cvCaptureFromCAM(0);

	if ( !capture ) {
		fprintf(stderr, "Cannot initialize webcam!\n");
		return 1;
	}

	// Create the window and the associated trackbar
	cvNamedWindow		(windowName, CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("trackbar", windowName, &lowThreshold, thresholdLimit, onTrackbarChange);

	// Process the video capture
	processVideoCapture(capture);

	// Cleanup
	cvDestroyWindow(windowName);
	cvReleaseCapture(&capture);

	return 0;
}

// Process the video capture
void processVideoCapture(CvCapture *capture) {
	IplImage* frame = 0;
	int				key		= 0;

	// While ESC was not pressed, continue
	while	(key != 27) {
		// Get a frame from the webcam
		frame = cvQueryFrame(capture);

		if(!frame)
			break;

		// Apply Canny to the input frame
		IplImage *grayFrame			= convertBGRToGrayscale(frame);
		IplImage *cannyFrame		= applyCannyToImage(grayFrame);
		IplImage *flippedFrame	= flipImageHorizontal(cannyFrame);

		cvReleaseImage(&grayFrame);
		cvReleaseImage(&cannyFrame);

		// Show the modified frame
		cvShowImage(windowName, flippedFrame);

		cvReleaseImage(&flippedFrame);

		key = cvWaitKey(1);
	}
}

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

// Flip an image with respect to the horizontal axis
IplImage* flipImageHorizontal(IplImage* sourceImage) {
	IplImage *flippedImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

	cvFlip(sourceImage, flippedImage, 1);

	return flippedImage;
}

// Main function
int main( int argc, char **argv ) {
	return run();
}