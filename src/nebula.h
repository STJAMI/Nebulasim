// nebula.h
#ifndef NEBULA_H
#define NEBULA_H

#define MAX_PARTICLES 200

/* Particle structure */
typedef struct {
    int x, y;         // position on grid (0..grid_w-1, 0..grid_h-1)
    int energy;       // energy (>=0)
    int brightness;   // 1 = faint ('*'), 2 = bright ('O')
    int alive;        // 1 alive, 0 removed
} Particle;

/* API functions */
void initializeParticles(Particle *p, int count, int grid_w, int grid_h);
void displayGrid(Particle *p, int count, int grid_w, int grid_h);
void moveParticles(Particle *p, int count, int grid_w, int grid_h);
void handleCollisions(Particle *p, int count, int grid_w, int grid_h);
void updateBrightness(Particle *p, int count);
int saveToFile(Particle *p, int count, int step, int grid_w, int grid_h);
void replaySimulation();

#endif // NEBULA_H
