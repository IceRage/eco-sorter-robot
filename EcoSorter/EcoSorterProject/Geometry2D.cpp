#include "Geometry2D.h"

// Determine the distance between two points

double Geometry2D::distanceBtwPoints(CvPoint2D32f p1, CvPoint2D32f p2) {
	double tmpX = pow(p1.x - p2.x, 2);
	double tmpY = pow(p1.y - p2.y, 2);

	return sqrt(tmpX + tmpY);
}

// Determine the slope of the line determined by the two points wrt to Ox-axis

double Geometry2D::slopeOfLineDetByPoints(CvPoint2D32f p1, CvPoint2D32f p2) {
	return (p2.y - p1.y)/(p2.x - p1.x);
}

// Determine the angle of the line determined by the two points wrt to Ox-axis

double Geometry2D::angleOfLineDetByPoints(CvPoint2D32f p1, CvPoint2D32f p2) {
	double slope				= Geometry2D::slopeOfLineDetByPoints(p1, p2);
	double angleInRads	= atan(slope);
	double angleInDegs	= angleInRads * 180 / PI;

	if (angleInDegs < 1E-7)
		angleInDegs += 360;

	if (angleInDegs - 180 < 1E-7)
		return angleInDegs;
	else
		return angleInDegs - 180;
}