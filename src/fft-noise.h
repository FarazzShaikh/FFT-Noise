#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/fft.h"
#include "../include/libattopng/libattopng.h"
#include "../include/upng/upng.h"

#define COMP 4
#define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))

int DEBUG;
double START_TIME;
double END_TIME;
char* WHITE_NOISE_PATH = NULL;

void free2d(float **p) {
  free(p[0]);
  free(p);
}

float **malloc2d(int v, int h) {
  int i;
  float **m;
  float *p;

  m = (float **)malloc(sizeof(float *) * v);
  p = (float *)malloc(sizeof(float) * h * v);

  for (i = 0; i < v; i++, p += h) {
    m[i] = p;
  }

  return m;
}

float *malloc1D(int N) { return (float *)malloc(sizeof(float) * N); }

int powerOf2(unsigned int v) { return v > 0 && (v & (v - 1)) == 0; }


void applyFilter(float *frequency, float **m_real, float **m_imag, int lgth, int wdth,
                 float beta) {
  int i;
  int j;
  float **radial_freq = malloc2d(lgth, wdth);

  for (i = 0; i < lgth; ++i) {
    for (j = 0; j < wdth; ++j) {
      radial_freq[i][j] =
          sqrt((frequency[i] * frequency[i]) + (frequency[j] * frequency[j]));
      if (radial_freq[i][j] == 0) {
        m_real[i][j] = 0.0;
        m_imag[i][j] = 0.0;
      } else {
        m_real[i][j] = m_real[i][j] / (pow(radial_freq[i][j], beta) * lgth);
        m_imag[i][j] = m_imag[i][j] / (pow(radial_freq[i][j], beta) * lgth);
      }
    }
  }
  free2d(radial_freq);
}

void saveImage(float **data, int x, int y) {
    libattopng_t *png = libattopng_new(x, y, PNG_RGBA);

    int i, j;
    for (i = 0; i < x; ++i)
    {
        for (j = 0; j < y; ++j)
        {
            unsigned char v = (unsigned char)(data[i][j]);
            libattopng_set_pixel(png, i, j, RGBA(v, v, v, 255));
        }
    }

    libattopng_save(png, "output/fractal_noise.png");
    libattopng_destroy(png);
}

int readImage(int N, char* path, float **m_real) {
    upng_t* upng = upng_new_from_file(path);

    if (upng != NULL) {
        upng_decode(upng);
        if (upng_get_error(upng) == UPNG_EOK) {
            
            if(N != upng_get_width(upng)) {
                printf("Incorrect size provided\n");
                exit(1); 
            }

            if (!powerOf2(N)) {
                printf("Error: N must be a power of 2\n");
                exit(1);
            }

            if(N != upng_get_height(upng)) {
                printf("Image must be square");
                exit(1);
            }


            const unsigned char* buffer = upng_get_buffer(upng);

            int offset = 0;
            int i, j;
            for (i = 0; i < N; ++i) {
                for (j = 0; j < N; ++j) {
                    int index = i * N + j;
                    unsigned char col = buffer[index];

                    m_real[i][j] =
                        (float)col + (float)col + (float)col;
                    m_real[i][j] /= COMP - 1;
                    offset += COMP - 1;
                }
            }
            
        }

        
        upng_free(upng);
    } else {
        printf("Could not load image");
        exit(1);
    }

    
    return N;
}

void makeWhiteNoise(int N, float **m_real) {
  int offset = 0, i, j;

  for (j = 0; j < N; ++j) {
    for (i = 0; i < N; ++i) {

      unsigned char col = rand() % 255;

      m_real[i][j] =
          (float)col + (float)col + (float)col;
      m_real[i][j] /= COMP - 1;
      offset += COMP - 1;
    }
  }
}

void FFTN_useImage(char* path) {
    WHITE_NOISE_PATH = path;
}

void FFTN_noise(float beta, int N) {
  float **m_real = malloc2d(N, N);
  float **modulus = malloc2d(N, N);
  float **m_imag = malloc2d(N, N);
  float **filter = malloc2d(N, N);
  float *frequency = malloc1D(N);


  if(WHITE_NOISE_PATH != NULL) {
    N = readImage(N, WHITE_NOISE_PATH, m_real);
  } else {
    makeWhiteNoise(N, m_real);
  }


  fft(m_real, m_imag, N, N);
  fftfreq(N, 1.0, frequency);
  
  applyFilter(frequency, m_real, m_imag, N, N, beta);

  fftshift(m_real, N, N);

  ifft(m_real, m_imag, N, N);

  if (!DEBUG) {
    normalise(m_real, N, N);
    saveImage(m_real, N, N);
  }
    
  free2d(modulus);
  free2d(m_imag);
  free2d(m_real);
  free2d(filter);
  free(frequency);

}
