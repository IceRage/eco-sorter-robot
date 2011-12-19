// EcoSorterLynxArm.cpp
// Project EcoSorter
// Krisztian Szabo 

#include "EcoSorterLynxArm.h"
#include <iostream>
#include <cstring>
#include <time.h>
#include <math.h>

using namespace std;

EcoSorterLynxArm::EcoSorterLynxArm( int portNumber, int bitsPerSecond ) 
{
	serial = new CSerial();

	serial->Open(portNumber, bitsPerSecond);
}

EcoSorterLynxArm::~EcoSorterLynxArm( void )
{
	serial->Close();
}

bool EcoSorterLynxArm::move( int nServo, int nPos, int nTime )
{
		this->moveArm(nServo, nPos);

		return true;
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

// Rotate the gripper in a given position

void EcoSorterLynxArm::rotateGripper( int position ) {
	moveArm(5, position);
}

// Rotate the gripper to the given angle

void EcoSorterLynxArm::rotateGripperToAngle( int angle ) {
	float gripperpos = 1500.0 + ( angle * 8.3 );

	if (gripperpos >= 700 && gripperpos <= 2300)
		rotateGripper(gripperpos);
	else
		rotateGripper(1500);
}

// Clench the gripper to the given position

void EcoSorterLynxArm::clenchGripper( int position ) {
	moveArm(4, position);
}

// Move the robot to the initial position

void EcoSorterLynxArm::moveToInitialPosition() {
	closeGripper();
	moveArm(3, 900);
	moveArm(1, 1900);
	moveArm(2, 1400);
	moveArm(0, 600);
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
}

// Move the arm to the position from which it will grab the object

void EcoSorterLynxArm::moveToObjectWithGripperAngle(float angle) {
	rotateGripperToAngle((int)angle);
	moveArm(0, 1370);

	if (fabs(angle) - 80.0 > 1E-5)
		moveArm(3, 900);
	else
		moveArm(3, 1000);

	openGripper();
	moveArm(2, 950);
	moveArm(1, 970);

	if (fabs(angle) - 80.0 > 1E-5)
		moveArm(3, 1000);
}

// Move the arm from the given servo to the given position

void EcoSorterLynxArm::moveArm( int servo, int position ) {
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