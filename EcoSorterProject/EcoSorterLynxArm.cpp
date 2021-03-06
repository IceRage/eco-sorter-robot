#include "EcoSorterLynxArm.h"
#include <iostream>
#include <cstring>
#include <time.h>
#include <math.h>

using namespace std;

EcoSorterLynxArm::EcoSorterLynxArm(int portNumber, int bitsPerSecond) {
	serial = new CSerial();

	initialize();

	serial->Open(portNumber, bitsPerSecond);
}

EcoSorterLynxArm::~EcoSorterLynxArm() {
	serial->Close();
}

// Close The gripper

void EcoSorterLynxArm::closeGripper() {
	moveArm(4, 1350);
}

// Open the gripper

void EcoSorterLynxArm::openGripper() {
	moveArm(4, 1800);
}

// Rotate the gripper in a vertical position

void EcoSorterLynxArm::rotateGripperVertical() {
	this->rotateGripper(1600);
}

// Rotate the gripper in a horizontal position

void EcoSorterLynxArm::rotateGripperHorizontal() {
	this->rotateGripper(700);
}

// Initialize the data of the class

void EcoSorterLynxArm::initialize() {
	isInInitialPosition = false;
}

// Rotate the gripper in a given position

void EcoSorterLynxArm::rotateGripper(int position) {
	moveArm(5, position);
}

// Rotate the gripper to the given angle

void EcoSorterLynxArm::rotateGripperToAngle(int angle) {
	angle = 700 + (int)(((double)angle / 180) * (2300 - 700));

	if (angle >= 700 && angle <= 2300)
		rotateGripper(angle);
	else
		rotateGripper(1500);
}

// Clench the gripper to the given position

void EcoSorterLynxArm::clenchGripper(int position) {
	moveArm(4, position);
}

// Move the robot to the initial position

void EcoSorterLynxArm::moveToInitialPosition() {
	if (!isInInitialPosition) {
		moveArm(3, 900);
		closeGripper();
		moveArm(1, 1900);
		moveArm(2, 1400);
		moveArm(0, 600);
		
		isInInitialPosition = true;
	}
}

// Move the arm to the plastic container

void EcoSorterLynxArm::moveToPlasticContainer() {
	closeGripper();
	rotateGripperHorizontal();
	moveArm(1, 1900);
	moveArm(0, 2200);
	moveArm(2, 1000);
	moveArm(3, 2400);
	moveArm(0, 1650);
	rotateGripperVertical();
	openGripper();

	isInInitialPosition = false;
}

// Move the arm to the metal container

void EcoSorterLynxArm::moveToMetalContainer() {
	closeGripper();
	rotateGripperHorizontal();
	moveArm(1, 1900);
	moveArm(0, 600);
	moveArm(2, 1000);
	moveArm(3, 2400);
	moveArm(0, 1100);
	rotateGripperVertical();
	openGripper();

	isInInitialPosition = false;
}

// Move the arm to the position from which it will grab the object

void EcoSorterLynxArm::moveToObjectWithGripperAngle(float angle) {
	rotateGripperToAngle((int)angle);
	moveArm(0, 1370);

	if (angle - 10.0 < 1E-5)
		moveArm(3, 900);
	else if (angle - 95.0 > 1E-5)
		moveArm(3, 800);
	else
		moveArm(3, 1000);
	openGripper();
	moveArm(2, 900);
	moveArm(1, 970);

	if ((angle - 10.0 < 1E-5) || (angle - 95.0 > 1E-5))
		moveArm(3, 1000);

	isInInitialPosition = false;
}

// Move the arm from the given servo to the given position

void EcoSorterLynxArm::moveArm(int servo, int position) {
	char sir[100];
	char timeString[10];
	char servoString[2];
	char positionString[10];

	sprintf(timeString, "%d", TIME);
	sprintf(servoString, "%d", servo);
	sprintf(positionString, "%d", position);

	strcpy(sir, "#");
	strcat(sir, servoString);
	strcat(sir, "P");
	strcat(sir, positionString);
	strcat(sir, "T");
	strcat(sir, timeString);

	int len = strlen(sir);

	sir[len] = 0xD;
	sir[len + 1] = '\0';

	serial->SendData((const char*)sir, len + 1);

	Sleep(TIME);
}