
#include "Music.h"
#include <string>
#include <irrKlang/irrKlang.h>

using namespace irrklang;

irrklang::ISoundEngine *sound;
std::string bgmFilename, shootFilename, bloomFilename;

void musicInit(std::string bgmFile, std::string shootFile, std::string bloomFile){
    sound = createIrrKlangDevice();
    bgmFilename = bgmFile;
    shootFilename = shootFile;
    bloomFilename = bloomFile;
}

void playBGM(){
    sound->setSoundVolume(0.4);
    sound->play2D(bgmFilename.c_str(), true);
}

void playShootSound(){
    sound->setSoundVolume(0.4);
    sound->play2D(shootFilename.c_str(), false);
    return;
}

void playBloomSound(){
    sound->setSoundVolume(0.8);
    sound->play2D(bloomFilename.c_str(), false);
    return;
}