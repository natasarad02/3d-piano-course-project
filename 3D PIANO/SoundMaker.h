#ifndef SOUND_MAKER_H
#define SOUND_MAKER_H

#include <GL/glew.h>   
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <irrKlang.h>
using namespace irrklang;
namespace SoundMaker {

	void playSound(std::string filePath, bool isPressed);

	void resetSoundFlag();
	void stop(ISoundEngine* engine);
	void VolumeDown();
	void VolumeUp();
	int getVolumePercentage();
	void writeToFile();
	void playSoundsFromFile(const std::string& fileName);


}


#endif 
