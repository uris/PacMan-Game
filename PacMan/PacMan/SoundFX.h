#pragma once
#include <windows.h>
#include <string>

class SoundFX {

public:
    SoundFX();
    SoundFX(std::string filename);
    ~SoundFX();
    void play(bool async = true);
    bool isok();

private:
    char* buffer;
    bool ok;
    HINSTANCE HInstance;
};
