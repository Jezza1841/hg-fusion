/*============================================================\
|  This file was made by TheGameratorT.                       |
|                                                             |
|  This code is meant to work with the following template:    |
|  https://github.com/Overblade/NSMB-ASMReference             |
|                                                             |
|  You may modify this file and use it for whatever you want  |
|  just be sure to credit me (TheGameratorT).                 |
|                                                             |
|  Hope you like it just as much as I had fun coding this!    |
|                                                             |
|  ---------------------------------------------------------  |
|                                                             |
|  NWAV player core declarations.                             |
|  This is the header that allows the engine code to be       |
|  implemented into the game and be accessed externally.      |
\============================================================*/

#ifndef _NWAVPLAYER_H
#define _NWAVPLAYER_H

#define NWAV 0x5641574E
#define FX32_CAST(x) ((fx32)x)
#define FX32_SHIFT 12

//#include "nitro_if.h"
#include "../include/types.h"
#include "../include/config.h"
#include "../include/debug.h"
#include "../include/sound.h"


#define FX32_CAST(x) ((fx32)x)
#define FX32_SHIFT 12

//The function type of the function that will handle the events.
typedef void(*NWAVPlayer_EventHandler)(int); 

/// <summary>Initializes the player system. (Hook after SND_Init)</summary>
void NWAVPlayer_init(void);

/// <summary>Updates the game fading. (Hook after SND_Main)</summary>
BOOL NWAVPlayer_updateFade(void);

/// <summary>Plays a music.</summary>
/// <param name="fileID">The file ID of the music file to play.</param>
void NWAVPlayer_play(int fileID);

/// <summary>Stops the music playing.</summary>
/// <param name="frames">Number of frames where the volume shift occurs.</param>
void NWAVPlayer_stop(int frames);

/// <summary>Gets the music volume.</summary>
/// <returns>The music volume.</returns>
int  NWAVPlayer_getVolume(void);

/// <summary>Sets the music volume.</summary>
/// <param name="volume">The target volume. Value range = [0, 127]</param>
/// <param name="frames">Number of frames where the volume shift occurs.</param>
void NWAVPlayer_setVolume(int volume, int frames);

/// <summary>Gets the music speed.</summary>
/// <returns>The current music speed.</returns>
fx32 NWAVPlayer_getSpeed(void);

/// <summary>Sets the music speed.</summary>
/// <param name="speed">The target speed for the music to be played at.</param>
void NWAVPlayer_setSpeed(fx32 speed);

/// <summary>Gets if the music is paused.</summary>
/// <returns>True if the music is paused. False otherwise.</returns>
BOOL NWAVPlayer_getPaused(void);

/// <summary>Sets if the music is paused.</summary>
/// <param name="paused">Sets the music as paused when true, unpauses when false.</param>
void NWAVPlayer_setPaused(BOOL paused);

/// <summary>Sets the event handler function.</summary>
/// <param name="func">The function pointer of the event handler.</param>
void NWAVPlayer_setEventHandler(NWAVPlayer_EventHandler func);


#endif //!_NWAVPLAYER_H
