#include "EcoSorterIRobot.h"
#include "EcoSorterLynxArm.h"
#include "EcoSorterVision.h"
#include "EcoSorterSound.h"

class EcoSorterProject {
private:
	EcoSorterVision*	visionController;
	EcoSorterIRobot*	iRobotController;
	EcoSorterLynxArm* lynxArmController;
	HANDLE						movementThread;
	int								screenHeight;

	double						CVT_CM2PX;
	double						STEPS;

public:
	EcoSorterProject();
	~EcoSorterProject();

	void run();
	void moveRobot();

	EcoSorterLynxArm* getLynxArmController();
	EcoSorterIRobot*	getIRobotController	();

private:
	void executeAreObjectsInSight			();
	void executeNoObjectsInSight			();
	void executeBumperActivated				();
	void executeObjectFullyCaptured		();
	void executeObjectNotFullyCaptured();
	void executeObjectLongerThanScreen();
	void executeObjectInCenter				();
	void executeObjectNotInCenter			();
	void turnRightRandomAngle					();
	void moveTowardsPoint							(CvPoint2D32f* objectCenter);
	void moveToContainer							(char type, float angle);

};