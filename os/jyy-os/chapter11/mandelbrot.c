#include "thread.h"
#include <math.h>

int NT;
#define W 6400
#define H 6400
#define IMG_FILE "mandelbrot.ppm"

static inline int belongs(int x, int y, int t) {
  return x / (W / NT) == t;
}

int x[W][H];
_Atomic int done = 0;

void dump(FILE *fp, int step) { 
  int w = W / step, h = H / step;
  // STFW: Portable Pixel Map
  fprintf(fp, "P6\n%d %d 255\n", w, h);
  for (int j = 0; j < H; j += step) {
    for (int i = 0; i < W; i += step) {
      int n = x[i][j];
      int r = 255 * pow((n - 80) / 800.0, 3);
      int g = 255 * pow((n - 80) / 800.0, 0.7);
      int b = 255 * pow((n - 80) / 800.0, 0.5);
      fputc(r, fp); fputc(g, fp); fputc(b, fp);
    }
  }
}

void Tworker(int tid) {
  for (int i = 0; i < W; i++)
    for (int j = 0; j < H; j++)
      if (belongs(i, j, tid - 1)) {
        double a = 0, b = 0, c, d;
        while ((c = a * a) + (d = b * b) < 4 && x[i][j]++ < 880) {
          b = 2 * a * b + j * 1024.0 / H * 8e-9 - 0.645411;
          a = c - d + i * 1024.0 / W * 8e-9 + 0.356888;
        }
      }
  atomic_fetch_add(&done, 1);
}

void Tdump() {
  float ms = 0;
  while (1) {
    FILE *fp = popen("viu -", "w"); assert(fp);
    dump(fp, W / 256);
    pclose(fp);
    if (atomic_load(&done) == NT) break;
    usleep(1000000 / 2);
    ms += 1000.0 / 2;
  }
  printf("Approximate render time: %.1lfs\n", ms / 1000);

  FILE *fp = fopen(IMG_FILE, "w"); assert(fp);
  dump(fp, 2);
  fclose(fp);
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  NT = atoi(argv[1]);

  create(Tdump);

  #pragma omp parallel num_threads(NT)
  #pragma omp for schedule(static)
  for (int i = 0; i < NT; i++) {
    Tworker(i + 1);  // Sequential code
  }
  // Equivalent to:
  //
  // for (int i = 0; i < NT; i++) {
  //   create(Tworker);
  // }

  join();
  return 0;
}