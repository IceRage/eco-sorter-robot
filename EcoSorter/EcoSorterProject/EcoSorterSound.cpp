#include "EcoSorterSound.h"

// Initialize the static member

HANDLE EcoSorterSound::audioThread = NULL;

// Play the welcome sound

void EcoSorterSound::playWelcomeSound() {
	playAudioFileUsingThread("buna_ziua.wav");
}

// Play the metal sound

void EcoSorterSound::playMetalSound() {
	playAudioFileUsingThread("metal.wav");
}

// Play the plastic sound

void EcoSorterSound::playPlasticSound() {
	playAudioFileUsingThread("plastic.wav");
}

// Play the unknown object in sight sound

void EcoSorterSound::playUnknownObjectInSightSound() {
	playAudioFileUsingThread("obiect_necunoscut.wav");
}

// Play the positioning sound

void EcoSorterSound::playPositioningSound() {
	playAudioFileUsingThread("pozitionez.wav");
}

// Play the searching sound

void EcoSorterSound::playSearchingSound() {
	playAudioFileUsingThread("cautare.wav");
}

// Play the bumper activated sound

void EcoSorterSound::playBumperActivatedSound() {
	playAudioFileUsingThread("inapoi.wav");
}

// Play the goodbye sound

void EcoSorterSound::playGoodbyeSound() {
	playAudioFileUsingThread("la_revedere.wav");
	releaseAudioThread			();
}

// Release the handle of the audio thread

void EcoSorterSound::releaseAudioThread() {
	WaitForSingleObject(audioThread, INFINITE);
}

// Play an audio file given as input using a thread

void EcoSorterSound::playAudioFileUsingThread(char* filepath) {
	if (audioThread != NULL) {
		WaitForSingleObject(audioThread, INFINITE);
	}

	audioThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EcoSorterSound::playAudioFile, filepath, NULL, NULL);
}

// Play an audio file given as input

void EcoSorterSound::playAudioFile(char* filepath) {
	FMOD::System     *system;
	FMOD::Sound      *sound;
	FMOD::Channel    *channel = 0;
	FMOD_RESULT       result;
	int               key;
	unsigned int			lengthInMilliSeconds;
	unsigned int      version;

	// Create a System object and initialize
	result = FMOD::System_Create(&system);
	errorCheck(result);

	result = system->getVersion(&version);
	errorCheck(result);

	if (version < FMOD_VERSION) {
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		exit(-1);
	}

	result = system->init(32, FMOD_INIT_NORMAL, 0);
	errorCheck(result);

	result = system->createSound(filepath, FMOD_HARDWARE, 0, &sound);
	errorCheck(result);

	result = sound->setMode(FMOD_LOOP_OFF);
	errorCheck(result);

	result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	errorCheck(result);

	result = sound->getLength(&lengthInMilliSeconds, FMOD_TIMEUNIT_MS);
	if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
		errorCheck(result);
	}

	// Wait for audio file to play
	Sleep(lengthInMilliSeconds);

	// Shut down
	result = sound->release();
	errorCheck(result);
	result = system->close();
	errorCheck(result);
	result = system->release();
	errorCheck(result);
}

// Check if there was an error in the last executed statement

void EcoSorterSound::errorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}