#include "melody.hpp"

void MelodyController::playSong(uint8_t songId, bool loop)
{
    currentSong = songId;
    currentSongMessage = 0;
    shouldSongLoop = loop;
}

uint32_t MelodyController::getSongMessage()
{
    if (currentSong == 0)
        return 0;

    uint32_t msgToReturn = 0;
    uint32_t songLoopIndex = 0;

    switch (currentSong)
    {
        case 1:  msgToReturn = main_song[currentSongMessage++]; songLoopIndex = main_song.size(); break;        
        default:                                                                                  break;
    }

    if (currentSongMessage == songLoopIndex)
    {
        if (shouldSongLoop)
            currentSongMessage = 0;
        else
            currentSong = 0;
    }

    return msgToReturn;
}