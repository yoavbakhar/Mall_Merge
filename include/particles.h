#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"

#define MAX_PARTICLES 100

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float alpha;
    float size;
    int active;
} Particle;

/* Initialize particle pool */
void ParticlesInit(void);

/* Spawn a burst of particles at position */
void SpawnMergeBurst(Vector2 position, Color color);

/* Update all active particles */
void UpdateParticles(float dt);

/* Draw all active particles */
void DrawParticles(void);

/* Cleanup particle system */
void ParticlesCleanup(void);

#endif