#ifndef __ECOSORTER_SOUND_H
#define __ECOSORTER_SOUND_H

#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#include "fmod.hpp"
#include "fmod_errors.h"

class EcoSorterSound {
private:
	static HANDLE audioThread;

public:
	static void playWelcomeSound							();
	static void playMetalSound								();
	static void playPlasticSound							();
	static void playUnknownObjectInSightSound	();
	static void playPositioningSound					();
	static void playSearchingSound						();
	static void playBumperActivatedSound			();
	static void playGoodbyeSound							();

private: 
	static void releaseAudioThread			();
	static void playAudioFileUsingThread(char* filepath);
	static void playAudioFile						(char* filepath);
	static void errorCheck							(FMOD_RESULT result);

};

#endif