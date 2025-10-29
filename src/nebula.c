// nebula.c  (color-enabled version)
#include "nebula.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* Helper: clamp value between min and max */
static int clamp(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

/* Random integer from lo to hi inclusive */
static int rand_range(int lo, int hi) { return lo + (rand() % (hi - lo + 1)); }

/* Try to enable ANSI processing on Windows so color codes work */
static void enable_ansi_on_windows(void) {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return;
  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) return;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
  SetConsoleMode(hOut, dwMode);
#endif
}

/* Return colored string (ANSI). We print directly to avoid allocation. */
static void print_colored_char(char ch) {
  switch (ch) {
    case 'O':
      /* bright particle -> yellow */
      printf("\033[1;33mO\033[0m");
      break;
    case '*':
      /* faint particle -> cyan */
      printf("\033[1;36m*\033[0m");
      break;
    default:
      putchar(ch);
      break;
  }
}

/* Initialize particles: random non-overlapping positions (best-effort),
   random energy between 1 and 5, brightness set by energy */
void initializeParticles(Particle* p, int count, int grid_w, int grid_h) {
  int i, tries;
  for (i = 0; i < count && i < MAX_PARTICLES; ++i) {
    p[i].alive = 1;
    p[i].energy = rand_range(1, 5);
    p[i].brightness = (p[i].energy >= 4) ? 2 : 1;
    /* try to place in a mostly random empty position (avoid trivial overlaps)
     */
    tries = 0;
    while (tries < 50) {
      int x = rand_range(0, grid_w - 1);
      int y = rand_range(0, grid_h - 1);
      int occupied = 0;
      for (int j = 0; j < i; ++j) {
        if (p[j].alive && p[j].x == x && p[j].y == y) {
          occupied = 1;
          break;
        }
      }
      if (!occupied) {
        p[i].x = x;
        p[i].y = y;
        break;
      }
      tries++;
    }
    /* if couldn't find unique, just put randomly */
    if (tries >= 50) {
      p[i].x = rand_range(0, grid_w - 1);
      p[i].y = rand_range(0, grid_h - 1);
    }
  }
  /* mark rest as dead if count < MAX_PARTICLES */
  for (int k = count; k < MAX_PARTICLES; ++k) p[k].alive = 0;
}

/* Display the grid to console using '.' '*' 'O' characters, but with colors */
void displayGrid(Particle* p, int count, int grid_w, int grid_h) {
  static int ansi_init_done = 0;
  if (!ansi_init_done) {
    enable_ansi_on_windows();
    ansi_init_done = 1;
  }

  /* create char grid */
  char (*grid)[grid_w] = malloc(sizeof(char[grid_h][grid_w]));
  if (!grid) {
    printf("Memory allocation failed for grid display.\n");
    return;
  }
  for (int r = 0; r < grid_h; ++r)
    for (int c = 0; c < grid_w; ++c) grid[r][c] = '.';

  /* place particles (if multiple in one cell, show brightest) */
  for (int i = 0; i < count; ++i) {
    if (!p[i].alive) continue;
    int x = clamp(p[i].x, 0, grid_w - 1);
    int y = clamp(p[i].y, 0, grid_h - 1);
    char ch = (p[i].brightness >= 2) ? 'O' : '*';
    /* grid[y][x] because y is row */
    if (grid[y][x] == '.' || ch == 'O') grid[y][x] = ch;
  }

  /* print column header */
  printf("   ");
  for (int c = 0; c < grid_w; ++c) {
    printf("%2d", c);
  }
  printf("\n");

  for (int r = 0; r < grid_h; ++r) {
    printf("%2d ", r);
    for (int c = 0; c < grid_w; ++c) {
      putchar(' ');
      print_colored_char(grid[r][c]);
    }
    printf("\n");
  }

  free(grid);
}

/* Move particles randomly by -1,0,+1 in x and y while staying inside grid */
void moveParticles(Particle* p, int count, int grid_w, int grid_h) {
  for (int i = 0; i < count; ++i) {
    if (!p[i].alive) continue;
    int dx = rand_range(-1, 1);
    int dy = rand_range(-1, 1);
    p[i].x = clamp(p[i].x + dx, 0, grid_w - 1);
    p[i].y = clamp(p[i].y + dy, 0, grid_h - 1);
    /* random tiny energy decay */
    if (rand() % 10 == 0) p[i].energy = p[i].energy - 1;
    if (p[i].energy < 0) p[i].energy = 0;
  }
}

/* If multiple particles share a cell, merge them into one particle:
   - the first alive in that cell accumulates energies
   - others are set alive=0
*/
void handleCollisions(Particle* p, int count, int grid_w, int grid_h) {
  for (int i = 0; i < count; ++i) {
    if (!p[i].alive) continue;
    for (int j = i + 1; j < count; ++j) {
      if (!p[j].alive) continue;
      if (p[i].x == p[j].x && p[i].y == p[j].y) {
        /* merge j into i */
        p[i].energy += p[j].energy;
        p[j].alive = 0;
      }
    }
  }
}

/* Update brightness based on energy values; remove particles with zero energy
 */
void updateBrightness(Particle* p, int count) {
  for (int i = 0; i < count; ++i) {
    if (!p[i].alive) continue;
    if (p[i].energy >= 5)
      p[i].brightness = 2;
    else if (p[i].energy >= 3)
      p[i].brightness = 2;
    else if (p[i].energy >= 1)
      p[i].brightness = 1;
    else {
      p[i].alive = 0; /* died out */
    }
  }
}

/* Save current grid snapshot into steps/step<step>.txt
   Returns 1 on success, 0 on failure. */
int saveToFile(Particle* p, int count, int step, int grid_w, int grid_h) {
  char filename[256];
  snprintf(filename, sizeof(filename), "steps/step%04d.txt", step);
  FILE* fp = fopen(filename, "w");
  if (!fp) return 0;

  /* Simple metadata */
  fprintf(fp, "NebulaSim frame %d\n", step);
  fprintf(fp, "grid %d %d\n", grid_w, grid_h);

  /* create grid representation */
  char (*grid)[grid_w] = malloc(sizeof(char[grid_h][grid_w]));
  if (!grid) {
    fclose(fp);
    return 0;
  }
  for (int r = 0; r < grid_h; ++r)
    for (int c = 0; c < grid_w; ++c) grid[r][c] = '.';
  for (int i = 0; i < count; ++i) {
    if (!p[i].alive) continue;
    int x = clamp(p[i].x, 0, grid_w - 1);
    int y = clamp(p[i].y, 0, grid_h - 1);
    char ch = (p[i].brightness >= 2) ? 'O' : '*';
    if (grid[y][x] == '.' || ch == 'O') grid[y][x] = ch;
  }

  /* print grid to file */
  for (int r = 0; r < grid_h; ++r) {
    for (int c = 0; c < grid_w; ++c) {
      fputc(grid[r][c], fp);
    }
    fputc('\n', fp);
  }

  free(grid);
  fclose(fp);
  return 1;
}

/* Replay: read files steps/step0001.txt, step0002.txt... until not found.
   Press Enter to advance frames. */
void replaySimulation() {
  int step = 1;
  char filename[256];
  while (1) {
    snprintf(filename, sizeof(filename), "steps/step%04d.txt", step);
    FILE* fp = fopen(filename, "r");
    if (!fp) break;
    /* read entire file and print */
    char line[1024];
    system(
        "clear||cls"); /* try to clear terminal (works on Unix and Windows) */
    while (fgets(line, sizeof(line), fp)) {
      fputs(line, stdout);
    }
    fclose(fp);
    printf("\n--- Frame %d --- Press Enter for next (or Ctrl+C to exit) ---\n",
           step);
    getchar();
    step++;
  }
  if (step == 1) {
    printf("No frames found in steps/ (no step files).\n");
  } else {
    printf("End of saved frames (last was step %d).\n", step - 1);
  }
}
