#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

/* Sound effect identifiers */
typedef enum {
    SFX_MERGE,
    SFX_COIN,
    SFX_TRASH,
    SFX_COUNT
} SoundEffect;

/* Initialize audio system - load sound files */
void AudioInit(void);

/* Cleanup audio system - unload sounds */
void AudioCleanup(void);

/* Play a sound effect safely (checks if loaded) */
void PlaySoundSafe(SoundEffect sfx);

/* Play merge sound */
void PlayMergeSound(void);

/* Play coin reward sound */
void PlayCoinSound(void);

/* Play trash/delete sound */
void PlayTrashSound(void);

#endif