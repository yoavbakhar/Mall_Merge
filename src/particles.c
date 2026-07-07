#include "particles.h"
#include <stdlib.h>
#include <time.h>

/* Static particle pool */
static Particle particlePool[MAX_PARTICLES] = {0};

/* Initialize random seed */
static int initialized = 0;

void ParticlesInit(void) {
    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
}

void SpawnMergeBurst(Vector2 position, Color color) {
    int count = 8 + (rand() % 5);  /* 8-12 particles */
    
    for (int i = 0; i < count && i < MAX_PARTICLES; i++) {
        /* Find an inactive particle slot */
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!particlePool[j].active) {
                /* Random angle and speed */
                float angle = (float)(rand() % 360) * 3.14159265f / 180.0f;
                float speed = 100.0f + (rand() % 100);
                float variance = 0.3f + (rand() % 40) / 100.0f;
                
                particlePool[j].position = position;
                particlePool[j].velocity.x = cosf(angle) * speed * variance;
                particlePool[j].velocity.y = sinf(angle) * speed * variance;
                particlePool[j].color = color;
                particlePool[j].alpha = 1.0f;
                particlePool[j].size = 8.0f + (rand() % 8);
                particlePool[j].active = 1;
                break;
            }
        }
    }
}

void UpdateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particlePool[i].active) {
            particlePool[i].position.x += particlePool[i].velocity.x * dt;
            particlePool[i].position.y += particlePool[i].velocity.y * dt;
            
            particlePool[i].velocity.y += 200.0f * dt;  /* Gravity */
            
            particlePool[i].alpha -= dt * 2.0f;  /* Fade out */
            particlePool[i].size -= dt * 15.0f;  /* Shrink */
            
            if (particlePool[i].alpha <= 0.0f || particlePool[i].size <= 0.0f) {
                particlePool[i].active = 0;
            }
        }
    }
}

void DrawParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particlePool[i].active) {
            Color faded = Fade(particlePool[i].color, particlePool[i].alpha);
            DrawCircleV(particlePool[i].position, particlePool[i].size, faded);
        }
    }
}

void ParticlesCleanup(void) {
    /* Nothing to cleanup for now - no dynamically allocated memory */
}