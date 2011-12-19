#include "EcoSorterProject.h"

bool stop = false;

// Run the video capture processing function in a separate thread

void activateRobot(EcoSorterProject* project) {
	project->getLynxArmController()->moveToInitialPosition();

	while (!stop) {
		project->moveRobot();

		Sleep(5000);
	}
}

// Constructor of the class

EcoSorterProject::EcoSorterProject() {
	CVT_CM2PX					= 10.3225806;

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
	if (visionController->areObjectsInSight()) {
		if (visionController->getNumberOfObjectsInSight() == 1) {
			if (visionController->isObjectFullyCaptured()) {
				if (visionController->isObjectInCenter()) {
					char type		= visionController->getObjectType();
					float	angle = visionController->getObjectsAngle();

					printf("The object will be picked up and put in the corresponding container(%c).\n", type);

					moveToContainer(type, angle);
				} else {
					printf("The object is not in the center of the screen so we move in a valid position.\n");

					moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
				}
			} else {
				if (visionController->isObjectLongerThanScreen(visionController->getObjectsBoundingCorners())) {
					printf("The object is longer than the screen so we clash into it.\n");

					iRobotController->moveForward(3 * visionController->getScreenHeight() / 4);
					iRobotController->moveBackward(3 * visionController->getScreenHeight() / 8);
				} else {
					printf("The object is not fully captured so we move towards it.\n");

					moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
				}
			}
		} else {
			printf("Number of objects in sight is more than 1 so we are clashing into them.\n");

			iRobotController->moveForward(3 * visionController->getScreenHeight() / 4);
			iRobotController->moveBackward(3 * visionController->getScreenHeight() / 8);
		}
	} else {
		if (iRobotController->isBumperActivated()) {
			printf("The bumper is activated so we are turning in random direction with random number of degrees.\n");

			turnRandomDirectionAndAngle();
		} else {
			printf("No object in sight so we are moving forward.\n");

			iRobotController->moveForward(visionController->getScreenHeight() / 2);
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

void EcoSorterProject::turnRandomDirectionAndAngle() {
	int degrees = 90 + (rand() % 90);

	if (rand() % 2)
		iRobotController->turnClockwise(degrees);
	else
		iRobotController->turnCounterClockwise(degrees);
}

// Move the robot towards the given point

void EcoSorterProject::moveTowardsPoint(CvPoint2D32f* objectCenter) {
	if (fabs(objectCenter->y) - visionController->getMinimumDistanceFromCenter() > 1E-5) {
		if (objectCenter->y > 0)
			iRobotController->moveBackward(static_cast<int>(fabs(objectCenter->y) / CVT_CM2PX * 10));
		else
			iRobotController->moveForward(static_cast<int>(fabs(objectCenter->y) / CVT_CM2PX * 10));
	}

	if (fabs(objectCenter->x) - visionController->getMinimumDistanceFromCenter() > 1E-5) {
		if (objectCenter->x < 0) {
			iRobotController->turnCounterClockwise(90);
			iRobotController->moveForward(static_cast<int>(fabs(objectCenter->x) / CVT_CM2PX * 10));
			iRobotController->turnClockwise(90);
		} else {
			iRobotController->turnClockwise(90);
			iRobotController->moveForward(static_cast<int>(fabs(objectCenter->x) / CVT_CM2PX * 10));
			iRobotController->turnCounterClockwise(90);
		}
	}
}

// Move the object to the corresponding container

void EcoSorterProject::moveToContainer(char type, int angle) {
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