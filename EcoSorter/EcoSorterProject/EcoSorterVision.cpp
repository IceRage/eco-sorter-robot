#include "EcoSorterVision.h"

using namespace std;

// Global variables
int	lowThreshold	= 30;
int	highThreshold	= 90;

// Event handler when trackbar position changes

void onTrackbarChange(int position) {
	lowThreshold	= position;
	highThreshold	= 3 * lowThreshold;
}

// Constructor for the class

EcoSorterVision::EcoSorterVision() {
	if (initCamera()) {
		initConstants();
		initVisionMembers();
		initWindow();
		initScreenDimensions();

		mutex = CreateMutex(NULL, FALSE, NULL);
	}
}

// Destructor for the class

EcoSorterVision::~EcoSorterVision() {
	cvDestroyWindow	(windowName);
	cvReleaseCapture(&capture);
}

// Check if there are objects in sight

bool EcoSorterVision::areObjectsInSight() {
	CvBox2D*	centers					= new CvBox2D[ITERATIONS_FOR_DETECTION];
	int*			contoursNumber	= new int[ITERATIONS_FOR_DETECTION];
	double*		perimeters			= new double[ITERATIONS_FOR_DETECTION];

	for (int i=0; i<ITERATIONS_FOR_DETECTION; i++) {
		contoursCount = 0;

		Sleep(250);

		WaitForSingleObject(mutex, INFINITE);

		perimeters[i]			= contourPerimeter;
		contoursNumber[i] = contoursCount;
		centers[i]				= boundingBox; 

		ReleaseMutex(mutex);
	}

	contourPerimeterMean = meanOfPerimeters(perimeters);
	contoursCountMean		 = meanOfContoursNumber(contoursNumber);
	boundingBoxMean			 = meanOfBoundingBoxes(centers);

	delete [] centers;
	delete [] contoursNumber;
	delete [] perimeters;

	return (contoursCountMean > 0)	? true
																	: false;
}

// Check if an object is fully in sight

bool EcoSorterVision::isObjectFullyCaptured() {
	CvPoint2D32f corners[4];
	cvBoxPoints(boundingBoxMean, corners);
	
	for (int i=0; i<4; i++) {
		if (!isCornerFullyCaptured(corners[i]))
			return false;
	}

	return true;
}

// Get the type of the object

char EcoSorterVision::getObjectType() {
	if ((contourPerimeterMean - MIN_PERIMETER > DOUBLE_COMPARE_TO_ZERO) && (contourPerimeterMean - MAX_PERIMETER < DOUBLE_COMPARE_TO_ZERO)) {
		if (contourPerimeterMean - THRESH_PERIMETER < DOUBLE_COMPARE_TO_ZERO)
			return 'm';
		else
			return 'p';
	} else {
		return 'e';
	}
}

// Get the perimeter of the contour of the object

double EcoSorterVision::getContourPerimeter() {
	return contourPerimeterMean;
}

// Get the number of objects is sight

int EcoSorterVision::getNumberOfObjectsInSight() {
	return contoursCountMean;
}

// Check if the object is (almost) in the center of the screen

bool EcoSorterVision::isObjectInCenter() {
	CvPoint2D32f objectCenter = boundingBoxMean.center;
	CvPoint2D32f screenCenter	= cvPoint2D32f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

	if (fabs(objectCenter.x - screenCenter.x) - MIN_DIST_FROM_CENTER > DOUBLE_COMPARE_TO_ZERO)
		return false;

	if (fabs(objectCenter.y - screenCenter.y) - MIN_DIST_FROM_CENTER > DOUBLE_COMPARE_TO_ZERO)
		return false;

	return true;
}

// Check if the object is longer than the screen

bool EcoSorterVision::isObjectLongerThanScreen(CvPoint2D32f corners[4]) {
	int numberOfMarginalPoints = 0;

	for (int i=0; i<4; i++) {
		if (!isCornerFullyCaptured(corners[i]))
			numberOfMarginalPoints++;
	}

	return (numberOfMarginalPoints == 4) ? true
																			 : false;
}

// Get the position of the object with respect to the center of the screen

CvPoint2D32f* EcoSorterVision::getObjectsPostionWrtScreenCenter() {
	CvPoint2D32f objectCenter = boundingBoxMean.center;
	CvPoint2D32f screenCenter	= cvPoint2D32f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

	CvPoint2D32f* result = new CvPoint2D32f; 

	result->x = objectCenter.x - screenCenter.x;
	result->y = objectCenter.y - screenCenter.y;

	return result;
}

// Get the corners of the bounding box

CvPoint2D32f* EcoSorterVision::getObjectsBoundingCorners() {
	CvPoint2D32f* corners = new CvPoint2D32f[4];
	cvBoxPoints(boundingBox, corners);

	return corners;
}

// Get the angle of the object

double EcoSorterVision::getObjectsAngle() {
	return boundingBoxMean.angle;
}

// Process the video capture
void EcoSorterVision::processVideoCapture() {
	IplImage* frame = 0;
	int				key		= 0;

	resetVisionMembers();

	// While ESC was not pressed, continue
	while	(key != ESC) {
		// Get the 5th frame only
		for (int i=0; i<5; i++)
			frame = cvQueryFrame(capture);

		if(!frame)
			break;

		// Set the ROI of the image
		cvSetImageROI(frame, cvRect(SCREEN_WIDTH/2, (SCREEN_HEIGHT + 20)/3 - 20, SCREEN_WIDTH, SCREEN_HEIGHT));

		// Get the image on which Canny was applied
		IplImage* canniedImage = getCannyImage(frame);

		// Show the contours
		displayContours(canniedImage);

		cvReleaseImage(&canniedImage);

		key = cvWaitKey(1);
	}
}

// Get the height of the screen

double EcoSorterVision::getScreenHeight() {
	return SCREEN_HEIGHT;
}

// Get the minimum distance from the screen center

double EcoSorterVision::getMinimumDistanceFromCenter() {
	return MIN_DIST_FROM_CENTER;
}

// Check if a corner is fully in sight

bool EcoSorterVision::isCornerFullyCaptured(CvPoint2D32f corner) {
	if ((corner.x - MIN_DIST_FROM_SCREEN < DOUBLE_COMPARE_TO_ZERO) || (fabs(corner.x - SCREEN_WIDTH) - MIN_DIST_FROM_SCREEN < DOUBLE_COMPARE_TO_ZERO))
		return false;
	else if ((corner.y - MIN_DIST_FROM_SCREEN < DOUBLE_COMPARE_TO_ZERO) || (fabs(corner.y - SCREEN_HEIGHT) - MIN_DIST_FROM_SCREEN < DOUBLE_COMPARE_TO_ZERO))
		return false;
	else
		return true;
}

// Check if a bounding box is uninitialized 

bool EcoSorterVision::isInitialized(CvBox2D boundingBox) {
	double perimeter = 2*(fabs(boundingBox.size.height) + fabs(boundingBox.size.width));

	if ((perimeter - MIN_PERIMETER) <= DOUBLE_COMPARE_TO_ZERO)
		return false;
	else
		return true;
}

// Initialize the constants specific to Vision algorithms

void EcoSorterVision::initVisionMembers() {
	contoursCount			= 0;
	contourPerimeter	= 0;

	contourPerimeterMean = 0;
	contoursCountMean		 = 0;
}

// Initialize the camera for capturing frames

bool EcoSorterVision::initCamera() {
	capture = cvCaptureFromCAM(0);

	if ( !capture ) {
		fprintf(stderr, "Cannot initialize webcam!\n");
		this->~EcoSorterVision();

		return false;
	}

	return true;
}

// Initialize the window for displaying the results

void EcoSorterVision::initWindow() {
	cvNamedWindow		(windowName, CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("Trackbar", windowName, &lowThreshold, thresholdLimit, onTrackbarChange);
}

// Initialize the values for the constants 

void EcoSorterVision::initConstants() {
	GAUSSIAN_PARAM				= 15;
	CLOSE_IT							= 14;
	CONTOUR_APPROX_LEVEL	= 2;
	MIN_PERIMETER					= 200;
	THRESH_PERIMETER			= 500;
	MAX_PERIMETER					= 1000;
	MIN_DIST_FROM_SCREEN	= 1;
	MIN_DIST_FROM_CENTER	= 15;

	ITERATIONS_FOR_DETECTION = 30;

	ESC = 27;

	thresholdLimit = 100;
	windowName		 = "EcoSorterVision";
}

// Initialize the screen dimension

void EcoSorterVision::initScreenDimensions() {
	IplImage* frame = cvQueryFrame(capture);

	SCREEN_WIDTH		= frame->width/2;
	SCREEN_HEIGHT		= 3*frame->height/4 - 20;
}

// Get the mean of all the center points found

CvBox2D EcoSorterVision::meanOfBoundingBoxes(CvBox2D* boundingBoxes) {
	int divideBy = ITERATIONS_FOR_DETECTION;

	double x = 0;
	double y = 0;
	
	double width	= 0;
	double height	= 0;

	double angle	= 0;

	for (int i=0; i<ITERATIONS_FOR_DETECTION; i++) {
		if (isInitialized(boundingBoxes[i])) {
			x += boundingBoxes[i].center.x;
			y += boundingBoxes[i].center.y;

			width		+= boundingBoxes[i].size.width;
			height	+= boundingBoxes[i].size.height;

			float tmpAngle = angleOfBoundingBox(boundingBoxes[i]);

			// If the object is horizontal i.e. at angle >170 set it to 0
			if (tmpAngle - 170 > 1E-7)
				tmpAngle = 0;
			
			angle	+= tmpAngle;
		} else {
			divideBy--;
		}
	}

	CvBox2D result;

	result.center = cvPoint2D32f(x/divideBy, y/divideBy);
	result.size		= cvSize2D32f(width/divideBy, y/divideBy);
	result.angle	= angle/divideBy;

	return result;
}

// Get the mean of all perimeters found

double EcoSorterVision::meanOfPerimeters(double* perimeters) {
	double perimeter = 0;
	int		 divideBy	 = ITERATIONS_FOR_DETECTION;

	for (int i=0; i<ITERATIONS_FOR_DETECTION; i++) {
		if (perimeters[i] - 70 > DOUBLE_COMPARE_TO_ZERO)
			perimeter += perimeters[i];
		else
			divideBy--;
	}

	if (divideBy != 0)
		return perimeter/divideBy;
	else
		return perimeter;
}

// Get the mean of the number of contours

int EcoSorterVision::meanOfContoursNumber(int* contoursNumber) {
	int contours = 0;
	int divideBy = ITERATIONS_FOR_DETECTION;

	for (int i=0; i<ITERATIONS_FOR_DETECTION; i++) {
		if (contoursNumber[i])
			contours += contoursNumber[i];
		else
			divideBy--;
	}

	if (divideBy)
		return contours/divideBy;
	else
		return 0;
}

// Reset the values of the members related to the Vision algorithms

void EcoSorterVision::resetVisionMembers() {
	contourPerimeter = -1;
	contoursCount		 = -1;
}

// Convert a Color image to a Grayscale image

IplImage* EcoSorterVision::convertRGBToGrayscale(IplImage* sourceImage) {
	IplImage *grayFrame	= cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

	cvConvertImage(sourceImage, grayFrame, 0);

	return grayFrame;
}

// Apply Canny edge detection and filters to a grayscale image

void EcoSorterVision::filterImage(IplImage* sourceImage) {
	contrastAndSmoothImage(sourceImage);

	// Apply Canny to the Image
	cvCanny(sourceImage, sourceImage, lowThreshold, highThreshold);
	
	// Morphological Close operation
	cvMorphologyEx(sourceImage, sourceImage, 0, 0, CV_MOP_CLOSE, CLOSE_IT);
}

// Increase contrast and smooth the image afterwards

void EcoSorterVision::contrastAndSmoothImage(IplImage* sourceImage) {
	cvConvertScale(sourceImage, sourceImage, 1.0, -50);
	cvSmooth(sourceImage, sourceImage, CV_GAUSSIAN, GAUSSIAN_PARAM, GAUSSIAN_PARAM);
}

// Get the image on which Canny was applied

IplImage* EcoSorterVision::getCannyImage(IplImage* sourceImage) {
	IplImage *grayFrame = convertRGBToGrayscale(sourceImage);
	
	// Apply contrast, smooth, canny and morphological close operations to the image
	filterImage(grayFrame);
	
	return grayFrame;
}

// Get the contours corresponding to the image on which Canny was applied

void EcoSorterVision::displayContours(IplImage* sourceImage) {
	IplImage* scratchImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_8U, 1);

	cvCopy(sourceImage, scratchImage);

	// Obtain the contours
	CvSeq* contours	= contourFromImage(scratchImage);

	cvReleaseImage(&scratchImage);

	// Draw the contours
	if (contours != NULL) {
		IplImage* resultImage = cvCreateImage(cvGetSize(sourceImage), IPL_DEPTH_32F, 3);

		printContours(contours, resultImage);

		cvShowImage(windowName, resultImage);

		cvReleaseImage(&resultImage);
		cvReleaseMemStorage(&(contours->storage));
	}
}

// Get the contour bounding box

CvBox2D EcoSorterVision::contourBoundingBox(CvSeq *contours) {
	CvMemStorage* storage		= cvCreateMemStorage(2048);
	CvBox2D boundingBox			= cvMinAreaRect2(contours, storage);

	cvReleaseMemStorage(&storage);

	return boundingBox;
}

// Get the contours from an image on which Canny was applied

CvSeq* EcoSorterVision::contourFromImage(IplImage* scratchImage) {
	CvMemStorage* storage				= cvCreateMemStorage(0);
	CvSeq*				tmpContour		= 0;

	CvContourScanner scanner = cvStartFindContours(
															 scratchImage, 
															 storage, 
															 sizeof(CvContour), 
															 CV_RETR_EXTERNAL, 
															 CV_CHAIN_APPROX_SIMPLE
														 );

	while ((tmpContour = cvFindNextContour(scanner)) != NULL) {
		CvSeq* newContour = cvApproxPoly(
													tmpContour, 
													sizeof(CvContour), 
													storage, 
													CV_POLY_APPROX_DP, 
													CONTOUR_APPROX_LEVEL, 
													0
												);

		cvSubstituteContour(scanner, newContour);
	}

	return cvEndFindContours(&scanner);
}

// Print the cotours onto the given image

void EcoSorterVision::printContours(CvSeq* contours, IplImage* image) {
	contoursCount = 0;

	for (CvSeq* tmpContour = contours; tmpContour != NULL; tmpContour = tmpContour->h_next) {
		WaitForSingleObject(mutex, INFINITE);
		contourPerimeter = cvContourPerimeter(tmpContour);
		ReleaseMutex(mutex);

		if ((contourPerimeter - MAX_PERIMETER < DOUBLE_COMPARE_TO_ZERO) && (contourPerimeter - MIN_PERIMETER > DOUBLE_COMPARE_TO_ZERO)) {
			WaitForSingleObject(mutex, INFINITE);
			contoursCount++;
			ReleaseMutex(mutex);

			// Draw the contours and the center of the bounding rectangle
			cvDrawContours(image, tmpContour, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), -1, 5);

			// Draw the center of the bounding box and print additional information
			WaitForSingleObject(mutex, INFINITE);
			boundingBox = contourBoundingBox(tmpContour);
			ReleaseMutex(mutex);

			printBoundingBoxInfo(boundingBox, image);

			cvDrawCircle(image, cvPointFrom32f(boundingBox.center), 2, CV_RGB(0, 0, 255), 10); 
		}
	}
}

// Print additional information regarding the bounding box

void EcoSorterVision::printBoundingBoxInfo(CvBox2D boundingBox, IplImage* image) {
	CvPoint2D32f corners[4];
	cvBoxPoints(boundingBox, corners);

	for (int i=0; i<4; i++) {
		cvLine(image, cvPointFrom32f(corners[i]), cvPointFrom32f(corners[(i+1) % 4]), CV_RGB(255, 0, 0));
	}
}

// Compute the angle of the bounding box

double EcoSorterVision::angleOfBoundingBox(CvBox2D boundingBox) {
	CvPoint2D32f corners[4];
	cvBoxPoints(boundingBox, corners);

	double distFrom0To1 = Geometry2D::distanceBtwPoints(corners[0], corners[1]);
	double distFrom0To3 = Geometry2D::distanceBtwPoints(corners[0], corners[3]);

	if (distFrom0To1 - distFrom0To3 < DOUBLE_COMPARE_TO_ZERO)
		return (180 - Geometry2D::angleOfLineDetByPoints(corners[0], corners[3]));
	else
		return (180 - Geometry2D::angleOfLineDetByPoints(corners[0], corners[1]));
}