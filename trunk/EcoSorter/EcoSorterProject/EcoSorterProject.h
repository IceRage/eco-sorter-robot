#include "EcoSorterIRobot.h"
#include "EcoSorterLynxArm.h"
#include "EcoSorterVision.h"

class EcoSorterProject {
private:
	EcoSorterVision*	visionController;
	EcoSorterIRobot*	iRobotController;
	EcoSorterLynxArm* lynxArmController;
	HANDLE						visionThread;

	double						CVT_CM2PX;
	int								STEPS;

public:
	EcoSorterProject();
	~EcoSorterProject();

	void run();
	void moveRobot();

	EcoSorterLynxArm* getLynxArmController();
	EcoSorterIRobot*	getIRobotController	();

private:
	void turnRightRandomAngle				();
	void moveTowardsPoint						(CvPoint2D32f* objectCenter);
	void moveToContainer						(char type, int angle);

};