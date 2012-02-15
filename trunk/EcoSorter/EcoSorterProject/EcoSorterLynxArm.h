#ifndef __ECOSORTER_LYNXARM_H__
#define __ECOSORTER_LYNXARM_H__

#define TIME 3000

#include <windows.h>
#include "Serial.h"

class EcoSorterLynxArm {
private:
	CSerial* serial;
	bool		 isInInitialPosition;
	
public:
	EcoSorterLynxArm( int portNumber = 3, int bitsPerSecond = 115200);
	~EcoSorterLynxArm();

	void closeGripper	();
	void openGripper	();

	void rotateGripperVertical	();
	void rotateGripperHorizontal();

	void moveToInitialPosition				();
	void moveToPlasticContainer				();
	void moveToMetalContainer					();
	void moveToObjectWithGripperAngle	(float angle);

private:
	void initialize						();
	void rotateGripperToAngle	(int angle);
	void rotateGripper				(int position);
	void clenchGripper				(int position);
	void moveArm							(int servo, int position);

};

#endif