#include "EcoSorterIRobot.h"
#include "EcoSorterLynxArm.h"
#include "EcoSorterVision.h"

class EcoSorterProject {
private:
	EcoSorterVision*	visionController;
	EcoSorterIRobot*	iRobotController;
	EcoSorterLynxArm* lynxArmController;
	HANDLE						visionThread;

public:
	EcoSorterProject();
	~EcoSorterProject();

	void run();
	EcoSorterVision*	getVisionController();
	EcoSorterLynxArm* getLynxArmController();
	EcoSorterIRobot*	getIRobotController();

};