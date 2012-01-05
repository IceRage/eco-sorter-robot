#ifndef _GEOMETRY_2D
#define _GEOMETRY_2D

#include <cv.h>

#define PI 3.14159265

class Geometry2D {
public:
	static double distanceBtwPoints			(CvPoint2D32f p1, CvPoint2D32f p2);
	static double angleOfLineDetByPoints(CvPoint2D32f p1, CvPoint2D32f p2);

private:
	static double slopeOfLineDetByPoints(CvPoint2D32f p1, CvPoint2D32f p2);

};

#endif