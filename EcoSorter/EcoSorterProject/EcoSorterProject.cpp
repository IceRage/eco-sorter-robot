#include "EcoSorterProject.h"

bool stop = false;

// Run the video capture processing function in a separate thread

void activateRobot(EcoSorterProject* project) {
	project->getLynxArmController()->moveToInitialPosition();

	while (!stop) {
		project->moveRobot();

		Sleep(2000);
	}
}

// Constructor of the class

EcoSorterProject::EcoSorterProject() {
	CVT_CM2PX					= 8.4375;
	STEPS							= 2;

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
	visionThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)activateRobot, this, NULL, NULL);

	visionController->processVideoCapture();

	stop = true;

	WaitForSingleObject(visionThread, INFINITE);
}

// Move the robot and search for objects

void EcoSorterProject::moveRobot() {
	int screenHeight = (int)visionController->getScreenHeight();

	if ((visionController->areObjectsInSight()) && (!iRobotController->isBumperActivated())) {
		if (visionController->getNumberOfObjectsInSight() == 1) {
			if (visionController->isObjectFullyCaptured()) {
				if (visionController->isObjectInCenter()) {
					char	type	= visionController->getObjectType();
					float	angle = (float)visionController->getObjectsAngle();

					printf("The object will be picked up and put in the corresponding container(%c).\n", type);

					moveToContainer(type, angle);
				} else {
					printf("The object is not in the center of the screen so we move in a valid position.\n");

					moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
				}
			} else {
				if (visionController->isObjectLongerThanScreen(visionController->getObjectsBoundingCorners())) {
					printf("The object is longer than the screen so we clash into it.\n");

					iRobotController->moveForward(3 * screenHeight / 4);
					iRobotController->moveBackward(3 * screenHeight / 8);
				} else {
					printf("The object is not fully captured so we move towards it.\n");

					moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
				}
			}
		} else {
			printf("Number of objects in sight is more than 1 so we are clashing into them.\n");

			iRobotController->moveForward(3 * screenHeight / 4);
			iRobotController->moveBackward(3 * screenHeight / 8);
		}
	} else {
		if (iRobotController->isBumperActivated()) {
			printf("The bumper is activated so we are turning right with random number of degrees.\n");

			iRobotController->moveBackward(150);
			turnRightRandomAngle();
		} else {
			printf("No object in sight so we are moving forward.\n");

			iRobotController->moveForward(screenHeight / 2);
		}
	}
}

// Get the controller of the LynxArm

EcoSorterLynxArm* EcoSorterProject::getLynxArmController() {
	return lynxArmController;
}

// Get the controller of the iRobot

EcoSorterIRobot* EcoSorterProject::getIRobotController() {
	return iRobotController;
}

// Turn the robot for a random amount of degrees in a random direction

void EcoSorterProject::turnRightRandomAngle() {
	int degrees = 90 + (rand() % 90);

	iRobotController->turnClockwise(degrees);
}

// Move the robot towards the given point

void EcoSorterProject::moveTowardsPoint(CvPoint2D32f* objectCenter) {
	if (fabs(objectCenter->y) - visionController->getMinimumDistanceFromCenter() > DOUBLE_COMPARE_TO_ZERO) {
		if (objectCenter->y > 0)
			iRobotController->moveBackward((int)(fabs(objectCenter->y) / (CVT_CM2PX * STEPS) * 10));
		else
			iRobotController->moveForward((int)(fabs(objectCenter->y) / (CVT_CM2PX * STEPS) * 10));
	}

	if (fabs(objectCenter->x) - visionController->getMinimumDistanceFromCenter() > DOUBLE_COMPARE_TO_ZERO) {
		if (objectCenter->x > 0) {
			iRobotController->turnClockwise(90);
			Sleep(500);
			iRobotController->moveForward((int)(fabs(objectCenter->x) / (CVT_CM2PX * STEPS) * 10));
			Sleep(500);
			iRobotController->turnCounterClockwise(90);
		} else {
			iRobotController->turnCounterClockwise(90);
			Sleep(500);
			iRobotController->moveForward((int)(fabs(objectCenter->x) / (CVT_CM2PX * STEPS) * 10));
			Sleep(500);
			iRobotController->turnClockwise(90);
		}
	}
}

// Move the object to the corresponding container

void EcoSorterProject::moveToContainer(char type, float angle) {
	if (type == 'm') {
		lynxArmController->moveToInitialPosition();
		lynxArmController->moveToObjectWithGripperAngle(angle);
		lynxArmController->moveToMetalContainer();
	} else if (type == 'p') {
		lynxArmController->moveToInitialPosition();
		lynxArmController->moveToObjectWithGripperAngle(angle);
		lynxArmController->moveToPlasticContainer();
	} else {
		// If an error occurs, continue to move forward
		iRobotController->moveForward(100);
	}
}