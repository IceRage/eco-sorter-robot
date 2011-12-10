// EcoSorterLynxArm.h
// Project EcoSorter
// Krisztian Szabo

#ifndef __ECOSORTER_LYNXARM_H__
#define __ECOSORTER_LYNXARM_H__

#define TIME 5000

#include <windows.h>
#include "Serial.h"

class EcoSorterLynxArm {
private:
	CSerial* serial;
	
public:
	EcoSorterLynxArm( int portNumber = 3, int bitsPerSecond = 115200);
	~EcoSorterLynxArm();

	bool move( int nServo, int nPos, int nTime );

	void closeGripper();
	void openGripper();

	void rotateGripperVertical();
	void rotateGripperHorizontal();

	void moveToInitialPosition();
	void moveToPlasticContainer();
	void moveToMetalContainer();
	void moveToObjectWithGripperAngle(float angle);

private:
	void rotateGripperToAngle(int angle);
	void rotateGripper( int position );
	void clenchGripper( int position );
	void moveArm( int servo, int position );

};

#endif