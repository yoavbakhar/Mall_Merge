#include "audio.h"

/* Sound effect holders */
static Sound sounds[SFX_COUNT] = {0};

/* Sound file paths */
static const char *soundPaths[SFX_COUNT] = {
    "assets/sounds/merge.wav",
    "assets/sounds/coin.wav",
    "assets/sounds/trash.wav"
};

void AudioInit(void) {
    InitAudioDevice();
    
    /* Load sound effects - will be NULL if files don't exist */
    for (int i = 0; i < SFX_COUNT; i++) {
        sounds[i] = LoadSound(soundPaths[i]);
    }
}

void AudioCleanup(void) {
    for (int i = 0; i < SFX_COUNT; i++) {
        if (sounds[i].stream.buffer != NULL) {
            UnloadSound(sounds[i]);
        }
    }
    CloseAudioDevice();
}

void PlaySoundSafe(SoundEffect sfx) {
    if (sfx >= 0 && sfx < SFX_COUNT) {
        if (sounds[sfx].stream.buffer != NULL) {
            PlaySound(sounds[sfx]);
        }
    }
}

void PlayMergeSound(void) {
    PlaySoundSafe(SFX_MERGE);
}

void PlayCoinSound(void) {
    PlaySoundSafe(SFX_COIN);
}

void PlayTrashSound(void) {
    PlaySoundSafe(SFX_TRASH);
}