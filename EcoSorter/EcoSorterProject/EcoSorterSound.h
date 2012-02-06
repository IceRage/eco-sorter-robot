#ifndef __ECOSORTER_SOUND_H
#define __ECOSORTER_SOUND_H

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "fmod.hpp"
#include "fmod_errors.h"

class EcoSorterSound {

public:
	static void playWelcomeSound									();
	static void playMetalSound										();
	static void playPlasticSound									();
	static void playMoreObjectsInSightSound				();
	static void playObjectIsNotFullyCapturedSound	();
	static void playObjectIsNotInCenterSound			();
	static void playSearchingSound								();
	static void playBumperActivatedSound					();
	static void playGoodbyeSound									();

private: 
	static void playAudioFile(char* filepath);
	static void errorCheck		(FMOD_RESULT result);

};

#endif