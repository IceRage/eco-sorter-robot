#include "EcoSorterProject.h"

bool stop = false;

// Run the video capture processing function in a separate thread

void testing(EcoSorterProject* project) {
	/*while (!stop) {
		if (project->getVisionController()->areObjectsInSight())
			if (project->getVisionController()->getNumberOfObjectsInSight() == 1)
				if (project->getVisionController()->isObjectFullyCaptured()) {
						char c = project->getVisionController()->getObjectType();

						if (c=='m') {
							project->getLynxArmController()->moveToInitialPosition();
							project->getLynxArmController()->moveToObjectWithGripperAngle(-90);
							project->getLynxArmController()->moveToMetalContainer();
						} else if (c=='p') {
							project->getLynxArmController()->moveToInitialPosition();
							project->getLynxArmController()->moveToObjectWithGripperAngle(-90);
							project->getLynxArmController()->moveToPlasticContainer();
						}
				}
	}*/
	project->getLynxArmController()->moveToInitialPosition();
	project->getLynxArmController()->moveToObjectWithGripperAngle(-90);
}

// Constructor of the class

EcoSorterProject::EcoSorterProject() {
	lynxArmController = new EcoSorterLynxArm(7, 115200);
	iRobotController	= new EcoSorterIRobot(8, 57600);
	visionController	= new EcoSorterVision();
}

// Destructor for the class

EcoSorterProject::~EcoSorterProject() {
	delete lynxArmController;
	delete iRobotController;
	delete visionController;
}

// Run the main algorithm

void EcoSorterProject::run() {
	visionThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)testing, this, NULL, NULL);

	visionController->processVideoCapture();

	stop = true;

	WaitForSingleObject(visionThread, INFINITE);
}

// Get the IRobotController

EcoSorterIRobot* EcoSorterProject::getIRobotController() {
	return this->iRobotController;
}

// Get the LynxArmController

EcoSorterLynxArm* EcoSorterProject::getLynxArmController() {
	return this->lynxArmController;
}

// Get the VisionController

EcoSorterVision* EcoSorterProject::getVisionController() {
	return this->visionController;
}