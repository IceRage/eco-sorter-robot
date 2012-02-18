#include "EcoSorterProject.h"

bool stop = false;

// Run the video capture processing function in a separate thread

void activateRobot(EcoSorterProject* project) {
	project->getLynxArmController()->moveToInitialPosition();

	while (!stop) {
		project->moveRobot();

		Sleep(1000);
	}
}

// Constructor of the class

EcoSorterProject::EcoSorterProject() {
	CVT_CM2PX					= 8.4375;
	STEPS							= 1.90;

	lynxArmController = new EcoSorterLynxArm(7, 115200);
	iRobotController	= new EcoSorterIRobot(8, 57600);
	visionController	= new EcoSorterVision();

	screenHeight			= (int)visionController->getScreenHeight();
}

// Destructor for the class

EcoSorterProject::~EcoSorterProject() {
	delete lynxArmController;
	delete iRobotController;
	delete visionController;
}

// Run the main algorithm

void EcoSorterProject::run() {
	EcoSorterSound::playWelcomeSound();

	movementThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)activateRobot, this, NULL, NULL);

	visionController->processVideoCapture();

	stop = true;

	WaitForSingleObject(movementThread, INFINITE);

	EcoSorterSound::playGoodbyeSound();
}

// Move the robot and search for objects

void EcoSorterProject::moveRobot() {
	if ((visionController->areObjectsInSight()) && (!iRobotController->isBumperActivated())) {
		executeAreObjectsInSight();
	} else {
		if (iRobotController->isBumperActivated()) {
			executeBumperActivated();
		} else {
			executeNoObjectsInSight();
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

// Execute the action corresponding to "are objects in sight"

void EcoSorterProject::executeAreObjectsInSight() {
	if (visionController->getNumberOfObjectsInSight() == 1) {
		executeOneObjectInSight();
	} else {
		executeMoreObjectsInSight();
	}
}

// Execute the action corresponding to "no objects in sight"

void EcoSorterProject::executeNoObjectsInSight() {
	printf("No object in sight so we are moving forward.\n");

	EcoSorterSound::playSearchingSound();

	iRobotController->moveForward(screenHeight / 2);
}

// Execute the action corresponding to "bumper activated"

void EcoSorterProject::executeBumperActivated() {
	printf("The bumper is activated so we are turning right with random number of degrees.\n");

	EcoSorterSound::playBumperActivatedSound();

	iRobotController->moveBackward(50);
	turnRightRandomAngle();
}

// Execute the action corresponding to "one object in sight"

void EcoSorterProject::executeOneObjectInSight() {
	if (visionController->isObjectFullyCaptured()) {
		executeObjectFullyCaptured();
	} else {
		if (visionController->isObjectLongerThanScreen(visionController->getObjectsBoundingCorners())) {
			executeObjectLongerThanScreen();
		} else {
			executeObjectNotFullyCaptured();
		}
	}
}

// Execute the action corresponding to "more objects in sight"

void EcoSorterProject::executeMoreObjectsInSight() {
	printf("Number of objects in sight is more than 1 so we are clashing into them.\n");

	EcoSorterSound::playUnknownObjectInSightSound();

	iRobotController->moveForward(3 * screenHeight / 2);
	iRobotController->moveBackward(3 * screenHeight / 4);
}

// Execute the action corresponding to "object is fully captured"

void EcoSorterProject::executeObjectFullyCaptured() {
	if (visionController->isObjectInCenter()) {
		executeObjectInCenter();
	} else {
		executeObjectNotInCenter();
	}
}

// Execute the action corresponding to "object is not fully captured"

void EcoSorterProject::executeObjectNotFullyCaptured() {
	printf("The object is not fully captured so we move towards it.\n");

	EcoSorterSound::playPositioningSound();

	moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
}

// Execute the action corresponding to "object longer than screen"

void EcoSorterProject::executeObjectLongerThanScreen() {
	printf("The object is longer than the screen so we clash into it.\n");

	EcoSorterSound::playUnknownObjectInSightSound();

	iRobotController->moveForward (3 * screenHeight / 4);
	iRobotController->moveBackward(3 * screenHeight / 8);
}

// Execute the action corresponding to "object in center"

void EcoSorterProject::executeObjectInCenter() {
	char	type	= visionController->getObjectType();
	float	angle = (float)visionController->getObjectsAngle();

	printf("The object will be picked up and put in the corresponding container(%c).\n", type);

	moveToContainer(type, angle);
}

// Execute the action corresponding to "object not in center"

void EcoSorterProject::executeObjectNotInCenter() {
	printf("The object is not in the center of the screen so we move in a valid position.\n");

	EcoSorterSound::playPositioningSound();

	moveTowardsPoint(visionController->getObjectsPostionWrtScreenCenter());
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
			iRobotController->moveForward((int)(fabs(objectCenter->x) / (CVT_CM2PX * STEPS) * 10));
			iRobotController->turnCounterClockwise(90);
		} else {
			iRobotController->turnCounterClockwise(90);
			iRobotController->moveForward((int)(fabs(objectCenter->x) / (CVT_CM2PX * STEPS) * 10));
			iRobotController->turnClockwise(90);
		}
	}
}

// Move the object to the corresponding container

void EcoSorterProject::moveToContainer(char type, float angle) {
	if (type == 'm') {
		EcoSorterSound::playMetalSound();

		lynxArmController->moveToInitialPosition();
		lynxArmController->moveToObjectWithGripperAngle(angle);
		lynxArmController->moveToMetalContainer();
	} else if (type == 'p') {
		EcoSorterSound::playPlasticSound();

		lynxArmController->moveToInitialPosition();
		lynxArmController->moveToObjectWithGripperAngle(angle);
		lynxArmController->moveToPlasticContainer();
	} else {
		// If an error occurs, continue to move forward
		EcoSorterSound::playUnknownObjectInSightSound();

		iRobotController->moveForward(100);
	}
}