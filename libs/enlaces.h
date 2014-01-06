#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define MAPA_DISTANCIA_X 5000
#define MAPA_DISTANCIA_Y 5000

#define umbral_sinr 10.0

// Potencia de transmisión
#define P 1.0

typedef struct {
  int canal;
  struct {
    int x;
    int y;
  } Tx, Rx;
} enlace;

float distancia_enlace(enlace *);
float distancia_entre_enlaces(enlace *, enlace *);
float distancia_entre_enlaces_en_kms(enlace *, enlace *);

void inicializa_enlace_primario(enlace *, int *, int);
void inicializa_enlace_secundario(enlace *, int);

void sinr_enlace_primario(enlace *, int, bool *, float *, enlace *, int, bool *);
void sinr_enlace_secundario(enlace *, int, bool *, enlace *, int, bool *, float *);

float watt_a_dB(float);
void imprime_enlace(enlace *);
bool mismo_canal(enlace *, enlace *);
bool rango_no_permitido(enlace *);
void evaluacion_dispositivos(float *, int *, bool *, int);
float fitness(float *, int *, float *, int, float *, int *, float *, int);

void datos_iniciales(char *, int, int, float, float, int);
void guarda_coordenadas(char *, enlace *, int, enlace *, int);
void marcas(char *, int, int, bool *, float, int *, float *, int, float *, int, enlace *, float);
void marcas_finales(char *, int, float, float, float, int);
