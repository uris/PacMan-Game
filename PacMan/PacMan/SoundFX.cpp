#include "SoundFX.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>

using namespace std;

SoundFX::SoundFX()
{
    ok = false;
    buffer = 0;
    HInstance = GetModuleHandle(0);
}

SoundFX::~SoundFX()
{
    PlaySound(NULL, NULL, 0);
    delete[] buffer;
}

SoundFX::SoundFX(string filename)
{
    ok = false;
    buffer = 0;
    HInstance = GetModuleHandle(0);

    std::ifstream infile(filename, ios::binary);

    if (!infile)
    {
        cout << "Wave::file error: " << filename << endl;
        return;
    }

    infile.seekg(0, ios::end);   // get length of file
    ULONGLONG length = infile.tellg();
    buffer = new char[length];    // allocate memory
    infile.seekg(0, ios::beg);   // position to start of file
    infile.read(buffer, length);  // read entire file

    infile.close();
    ok = true;
}
void SoundFX::play(bool async)
{
    if (!ok)
        return;

    if (async)
        PlaySound((LPCTSTR)buffer, HInstance, SND_MEMORY | SND_ASYNC);
    else
        PlaySound((LPCTSTR)buffer, HInstance, SND_MEMORY);
}

bool SoundFX::isok()
{
    return ok;
}