#ifndef __ECO_SORTER_IROBOT_H
#define __ECO_SORTER_IROBOT_H

#include "Serial.h"
#include "oi.h"
#include <stdio.h>
#include <climits>

#define VELOCITY		100
#define	SLEEP_TIME	20

class EcoSorterIRobot {
private:
	CSerial* serial;
	int			 portNumber;
	int			 bitsPerSecond;

public:
	EcoSorterIRobot	(int portNumber = 3, int bitsPerSecond = 57600);
	~EcoSorterIRobot();

	void moveForward					(int distance);
	void moveBackward					(int distance);
	void turnClockwise				(int degrees);
	void turnCounterClockwise	(int degrees);
	bool isBumperActivated		();

private:
	void move								(int distance, bool isForward);
	void turn								(int degrees, bool isClockWise);
	void moveInDirection		(bool isForward);
	void moveOnDistance			(int distance, bool isForward);
	void turnInDirection		(bool isClockWise);
	void turnNrOfDegrees		(int degrees, bool isClockWise);
	bool areBumpersActivated();
	bool isBatteryCharged		();
	void startCommand				();
	void stopFromMoving			();
	void openConnection			();
	void closeConnection		();

};

#endif