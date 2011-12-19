#include "EcoSorterIRobot.h"

// Constructor for the class

EcoSorterIRobot::EcoSorterIRobot(int portNumber, int bitsPerSecond) {
	this->serial = new CSerial();

	this->portNumber		= portNumber;
	this->bitsPerSecond = bitsPerSecond;
}

// Destructor for the class

EcoSorterIRobot::~EcoSorterIRobot() {
	
}

// Move forward on the given distance

void EcoSorterIRobot::moveForward(int distance) {
	move(distance, true);
}

// Move backward on the given distance

void EcoSorterIRobot::moveBackward(int distance) {
	move(distance, false);
}

// Turn the robot clockwise for the specified number of degrees

void EcoSorterIRobot::turnClockwise(int degrees) {
	turn(degrees, true);
}

// Turn the robot counterclockwise for the specified number of degrees

void EcoSorterIRobot::turnCounterClockwise(int degrees) {
	turn(degrees, false);
}

// Check if the bumper is activated

bool EcoSorterIRobot::isBumperActivated() {
	openConnection();
	startCommand();
	Sleep(SLEEP_TIME);

	bool result = areBumpersActivated();

	Sleep(SLEEP_TIME);
	closeConnection();

	return result;
}

// Move the robot in the given direction on the specified distance

void EcoSorterIRobot::move(int distance, bool isForward) {
	openConnection();
	startCommand();
	Sleep(SLEEP_TIME);
	moveInDirection(isForward);
	Sleep(SLEEP_TIME);
	moveOnDistance(distance, isForward);
	Sleep(SLEEP_TIME);
	stopFromMoving();
	Sleep(SLEEP_TIME);
	closeConnection();
}

// Turn the robot for the given number of degrees in the given direction

void EcoSorterIRobot::turn(int degrees, bool isClockWise) {
	openConnection();
	startCommand();
	Sleep(SLEEP_TIME);
	turnInDirection(isClockWise);
	Sleep(SLEEP_TIME);
	turnNrOfDegrees(degrees, isClockWise);
	Sleep(SLEEP_TIME);
	stopFromMoving();
	Sleep(SLEEP_TIME);
	closeConnection();
}

// Move the robot forward or backward

void EcoSorterIRobot::moveInDirection(bool isForward) {
	unsigned char cmd[100];

	int k = 0;

	// Empty the input buffer
	serial->ReadData(cmd, 100);

	cmd[k++] = 145;

	if (isForward) {
		cmd[k++] = 0;
		cmd[k++] = VELOCITY;
		cmd[k++] = 0;
		cmd[k++] = VELOCITY;
	} else {
		cmd[k++] = 255;
		cmd[k++] = 255 - VELOCITY;
		cmd[k++] = 255;
		cmd[k++] = 255 - VELOCITY;
	}
	
	serial->SendData((const char*)cmd, k);
}

// Move the robot on a given distance

void EcoSorterIRobot::moveOnDistance(int distance, bool isForward) {
	unsigned char cmd[100];

	// Empty the input buffer
	serial->ReadData(cmd, 100);
	
	int k = 0;

	cmd[k++] = 156;

	if (isForward) {
		cmd[k++] = distance / 256;
		cmd[k++] = distance % 256;
	} else {
		cmd[k++] = 255 - (distance / 256);
		cmd[k++] = 255 - (distance % 256);
	}

	serial->SendData((const char*)cmd, k);
}

// Turn the robot clockwise/counterclockwise

void EcoSorterIRobot::turnInDirection(bool isClockWise) {
	unsigned char cmd[100];

	int k = 0;

	// Empty the input buffer
	serial->ReadData(cmd, 100);

	cmd[k++] = 145;

	if (isClockWise) {
		cmd[k++] = 255;
		cmd[k++] = -VELOCITY;
		cmd[k++] = 0;
		cmd[k++] = VELOCITY;
	} else {
		cmd[k++] = 0;
		cmd[k++] = VELOCITY;
		cmd[k++] = 255;
		cmd[k++] = -VELOCITY;
	}

	serial->SendData((const char*)cmd, k);
}

// Turn the robot for the specified number of degrees

void EcoSorterIRobot::turnNrOfDegrees(int degrees, bool isClockWise) {
	unsigned char cmd[100];

	// Empty the input buffer
	serial->ReadData(cmd, 100);
	
	int k = 0;

	cmd[k++] = 157;

	if (isClockWise) {
		cmd[k++] = 255 - (degrees / 256);
		cmd[k++] = 255 - (degrees % 256);
	} else {
		cmd[k++] = degrees / 256;
		cmd[k++] = degrees % 256;
	}

	serial->SendData((const char*)cmd, k);
}

// Check if the left- or right-bumper is activated

bool EcoSorterIRobot::areBumpersActivated() {
	unsigned char cmd[100];

	// Empty the input buffer
	serial->ReadData(cmd, 100);
	
	int k = 0;

	// Send the request for reading the state of the sensors
	cmd[k++] = 142;
	cmd[k++] = 7;

	serial->SendData((const char*)cmd, k);

	Sleep(SLEEP_TIME);

	// Interpret the results

	serial->ReadData(cmd, 1);

	int bumpers = (int)cmd[0];

	// Keep only the bits related to the bumper
	bumpers = bumpers & 3;

	return (bumpers > 0) ? true
											 : false;
}

// Check if the battery level is greater than 400 mAh

bool EcoSorterIRobot::isBatteryCharged() {
	unsigned char cmd[100];

	int k = 0;

	// Empty the input buffer
	serial->ReadData(cmd, 100);

	cmd[k++] = CmdSensorList;
	cmd[k++] = 1;
	cmd[k++] = 25;

	serial->SendData((const char*)cmd, k);

	Sleep(SLEEP_TIME);

	serial->ReadData(cmd, 2);

	unsigned short chargeLevel = (unsigned short) cmd[1] * 256 + (unsigned short) cmd[0];

	if (chargeLevel < 400)
		return false;
	else
		return true;
}

// Send the start command to the robot

void EcoSorterIRobot::startCommand() {
	unsigned char cmd[100];

	int k = 0;

	// Empty the input buffer
	serial->ReadData(cmd, 100);

	cmd[k++] = CmdStart;
	cmd[k++] = CmdFull;
	serial->SendData((const char*)cmd, k);
}

// Stop the robot from moving

void EcoSorterIRobot::stopFromMoving() {
	unsigned char cmd[100];
	int k = 0;

	// Empty the input buffer
	serial->ReadData(cmd, 100);

	cmd[k++] = CmdDrive;
	cmd[k++] = 0;
	cmd[k++] = 0;
	cmd[k++] = 0;
	cmd[k++] = 0;

	serial->SendData((const char*)cmd, k);
}

// Open the connection to the robot

void EcoSorterIRobot::openConnection() {
	if ((!serial->Open(portNumber, bitsPerSecond)) || (!isBatteryCharged())) {
		printf("Not enough battery or couldn't connect to iRobot\n");
	}
}

// Close the connection to the robot

void EcoSorterIRobot::closeConnection() {
	serial->Close();
}