#include "EcoSorterIRobot.h"
#include "EcoSorterLynxArm.h"
#include "EcoSorterVision.h"
#include <conio.h.>

using namespace std;

int main(int argc, char** argv) {
	EcoSorterLynxArm* armController			= new EcoSorterLynxArm();
	EcoSorterIRobot*	iRobotController	= new EcoSorterIRobot();
	EcoSorterVision*	visionController	= new EcoSorterVision();

	armController->moveToInitialPosition();
	iRobotController->moveForward(300);
	visionController->processVideoCapture();

	delete armController;
	delete iRobotController;
	delete visionController;

	printf("Press enter...\n");
	getch();

	return 0;
}