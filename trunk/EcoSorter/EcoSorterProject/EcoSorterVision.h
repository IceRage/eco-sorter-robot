#ifndef __ECO_SORTER_VISION_H_
#define __ECO_SORTER_VISION_H_

#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include "Geometry2D.h"

#define	DOUBLE_COMPARE_TO_ZERO 0.00001

class EcoSorterVision {
private:
	short int	GAUSSIAN_PARAM;
	short int	CLOSE_IT;
	short int	CONTOUR_APPROX_LEVEL;
	double		MIN_PERIMETER;
	double		THRESH_PERIMETER;
	double		MAX_PERIMETER;
	double		MIN_DIST_FROM_SCREEN;
	double		MIN_DIST_FROM_CENTER;
	double		SCREEN_WIDTH;
	double		SCREEN_HEIGHT;
	int				ITERATIONS_FOR_DETECTION;

	short int ESC;
	
	char*		  windowName;

	int thresholdLimit;

	CvCapture*		capture;
	CvBox2D				boundingBox;
	int						contoursCount;
	double				contourPerimeter;
	CvBox2D				boundingBoxMean;
	int						contoursCountMean;
	double				contourPerimeterMean;
	CvPoint2D32f	closestPoint;

	HANDLE				mutex;

public:
	EcoSorterVision();
	~EcoSorterVision();

	bool					areObjectsInSight									();
	bool					isObjectFullyCaptured							();
	char					getObjectType											();
	double				getContourPerimeter								();
	int						getNumberOfObjectsInSight					();
	bool					isObjectInCenter									();
	bool					isObjectLongerThanScreen					(CvPoint2D32f corners[4]);
	CvPoint2D32f*	getObjectsPostionWrtScreenCenter	();
	CvPoint2D32f*	getObjectsBoundingCorners					();
	double				getObjectsAngle										();
	void					processVideoCapture								();
	double				getScreenHeight										();
	double				getMinimumDistanceFromCenter			();

private:
	bool			isCornerFullyCaptured			(CvPoint2D32f corner);
	bool			isInitialized							(CvBox2D boundingBox);
	void			initVisionMembers					();
	bool			initCamera								();
	void			initWindow								();
	void			initConstants							();
	void			initScreenDimensions			();
	CvBox2D		meanOfBoundingBoxes				(CvBox2D* boundingBoxes);
	double		meanOfPerimeters					(double* perimeters);
	int				meanOfContoursNumber			(int* contoursNumber);
	void			resetVisionMembers				();
	IplImage* convertRGBToGrayscale			(IplImage *sourceImage);
	void			saveContours							(CvSize size, CvSeq* contour);
	void			filterImage								(IplImage *sourceImage);
	void			contrastAndSmoothImage		(IplImage *sourceImage);
	IplImage* getCannyImage							(IplImage *sourceImage);
	void			displayContours						(IplImage *sourceImage);
	void			printContours							(CvSeq* contours, IplImage* image);
	void			printBoundingBoxInfo			(CvBox2D boundingBox, IplImage* image);
	CvSeq*		contourFromImage					(IplImage *scratchImage);
	CvBox2D		contourBoundingBox				(CvSeq *contour);
	CvBox2D		closestBoundingBox				(CvBox2D tmpBoundingBox);
	double		angleOfBoundingBox				(CvBox2D boundingBox);

};

#endif