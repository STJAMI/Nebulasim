// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "nebula.h"

/* Default parameters */
#define DEFAULT_GRID_W 20
#define DEFAULT_GRID_H 12
#define DEFAULT_PARTICLES 20
#define DEFAULT_STEPS 40

/* utility: pause until user presses enter */
static void wait_enter() {
  printf("Press Enter to continue...");
  while (getchar() != '\n') { /* consume until newline */
  }
}

/* Count alive particles */
static int aliveCount(Particle* p, int count) {
  int c = 0;
  for (int i = 0; i < count; ++i)
    if (p[i].alive) c++;
  return c;
}

int main() {
  srand((unsigned int)time(NULL));
  Particle particles[MAX_PARTICLES];
  int grid_w = DEFAULT_GRID_W;
  int grid_h = DEFAULT_GRID_H;
  int num_particles = DEFAULT_PARTICLES;
  int steps = DEFAULT_STEPS;

  /* Basic menu */
  while (1) {
    system("clear||cls");
    printf("=========================================\n");
    printf("           NebulaSim - C Project         \n");
    printf("        Interactive Space Simulation     \n");
    printf("=========================================\n");
    printf("1. Start new simulation (interactive)\n");
    printf("2. Start new simulation (batch save frames)\n");
    printf("3. Replay saved simulation frames\n");
    printf("4. Example run with defaults\n");
    printf("5. Exit\n");
    printf("-----------------------------------------\n");
    printf("Enter choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n');  // clear input
      continue;
    }
    while (getchar() != '\n');  // clear newline

    if (choice == 5) break;

    if (choice == 1) {
      printf("Grid width (default %d): ", grid_w);
      char buf[64];
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 4 && v <= 80) grid_w = v;
      }
      printf("Grid height (default %d): ", grid_h);
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 4 && v <= 50) grid_h = v;
      }
      printf("Number of particles (default %d): ", num_particles);
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 0 && v <= MAX_PARTICLES) num_particles = v;
      }

      /* init */
      initializeParticles(particles, num_particles, grid_w, grid_h);
      int step = 1;
      while (1) {
        system("clear||cls");
        printf("Step %d  Alive: %d\n", step,
               aliveCount(particles, num_particles));
        displayGrid(particles, num_particles, grid_w, grid_h);
        printf(
            "\nOptions: (Enter) next step | s Save frame | q Quit to menu\n");
        char cmd = getchar();
        if (cmd == 'q' || cmd == 'Q') break;
        if (cmd == 's' || cmd == 'S') {
          if (!saveToFile(particles, num_particles, step, grid_w, grid_h)) {
            printf("Failed to save frame %d\n", step);
            wait_enter();
          } else {
            printf("Saved frame %d\n", step);
            wait_enter();
          }
        } else {
          /* proceed normal update */
          moveParticles(particles, num_particles, grid_w, grid_h);
          handleCollisions(particles, num_particles, grid_w, grid_h);
          updateBrightness(particles, num_particles);
          step++;
        }
        /* consume leftover newline if any */
        if (cmd != '\n')
          while (getchar() != '\n');
      }
    } else if (choice == 2) {
      /* batch run that saves frames automatically */
      printf("Grid width (default %d): ", grid_w);
      char buf[64];
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 4 && v <= 80) grid_w = v;
      }
      printf("Grid height (default %d): ", grid_h);
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 4 && v <= 50) grid_h = v;
      }
      printf("Number of particles (default %d): ", num_particles);
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 0 && v <= MAX_PARTICLES) num_particles = v;
      }
      printf("Number of steps to save (default %d): ", steps);
      if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int v = atoi(buf);
        if (v > 0 && v <= 10000) steps = v;
      }

      initializeParticles(particles, num_particles, grid_w, grid_h);

      /* ensure steps folder exists - best-effort */
      system("mkdir -p steps 2> /dev/null || mkdir steps 2> /dev/null");

      for (int s = 1; s <= steps; ++s) {
        printf("Running step %d / %d\r", s, steps);
        fflush(stdout);
        displayGrid(particles, num_particles, grid_w, grid_h);
        saveToFile(particles, num_particles, s, grid_w, grid_h);
        moveParticles(particles, num_particles, grid_w, grid_h);
        handleCollisions(particles, num_particles, grid_w, grid_h);
        updateBrightness(particles, num_particles);
        /* reduce console spam slightly */
      }
      printf("\nBatch save complete. Files saved to steps/stepXXXX.txt\n");
      wait_enter();
    } else if (choice == 3) {
      printf(
          "Replay saved frames from steps/ (press Enter to advance frames)\n");
      wait_enter();
      replaySimulation();
      wait_enter();
    } else if (choice == 4) {
      /* example run: default small run and show a few steps interactively */
      grid_w = DEFAULT_GRID_W;
      grid_h = DEFAULT_GRID_H;
      num_particles = DEFAULT_PARTICLES;
      steps = 10;
      initializeParticles(particles, num_particles, grid_w, grid_h);
      for (int s = 1; s <= steps; ++s) {
        system("clear||cls");
        printf("Example run - Step %d / %d\n", s, steps);
        displayGrid(particles, num_particles, grid_w, grid_h);
        moveParticles(particles, num_particles, grid_w, grid_h);
        handleCollisions(particles, num_particles, grid_w, grid_h);
        updateBrightness(particles, num_particles);
        printf("\nPress Enter for next step... (or Ctrl+C to exit example)\n");
        getchar();
      }
      wait_enter();
    }
  }

  printf("Exiting NebulaSim. Goodbye!\n");
  return 0;
}
