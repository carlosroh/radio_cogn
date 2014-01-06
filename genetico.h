/* Algoritmo Genetico para Maximizacion*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "libs/enlaces.h"

#define PI 3.14159265

// Numero de individuos que se valorarán en el algoritmo genético
#define ind 30

// Numero de generaciones que evaluará el algoritmo genético
#define gen 20

// El problema nos limita a maximo 10 usuarios primarios
#define usuarios_primarios 10
#define usuarios_secundarios 20

float pc = 0.75;  //probabilidad de cruzamiento
float pm = 0.01;  //probabilidad de mutacion

// Nombre de la archivo de salida.
char *nombre = "salida.txt";

/* Variable global que marca la interacion en el algoritmo genetico. 
   Siempre se comienza con uno, que es el minimo de interacciones. */
int generacion = 1;

// Variable que almacena el optimo global de las corridas
float global = 0.0;

// Variable que almacena la iteraccion del ultimo cambion en el optimo global de las corridas
int ultimo_cambio = 0;

// Salto de marca. A cada cuantas interacciones debe guardar en el archivo de salida
int salto_marca = 1;

// Variables para el calculo del outage
int valida = 0, invalida = 0;

// Guardamos el total de las interacciones
float sinr_max = umbral_sinr;

// El problema requiere que siempre esten transmitiendo los usuarios primarios
bool estados_pu[] = {true, true, true, true, true, true, true, true, true, true};

// Almacena la relacion señal interferencia en watts
float sinr_pu[usuarios_primarios];
float sinr_su[usuarios_secundarios];

// Guarda en el vector la evaluacion de los dispositivos, true si
// transmiten satisfactoriamente, o false si no se puede realizar
//  la transmision.
int evaluacion_pu[usuarios_primarios];
int evaluacion_su[usuarios_secundarios];
  
// En los vectores de aptitud se guardan las tazas de transferencia de los enlaces
float aptitud_pu[usuarios_primarios];
float aptitud_su[usuarios_secundarios];
  
// Vector que almacena en que canal esta trabajando cada PU
int estado_del_espectro[usuarios_primarios];  
enlace pu_grupo[usuarios_primarios];
enlace su_grupo[usuarios_secundarios];

int SeleccionPadres(int padres[ind], float acum[ind]);
void cruzamiento(float pc, int selec[][usuarios_secundarios]);
void mutacion(float pm, int selec[][usuarios_secundarios]);
void sustituir(int selec[][usuarios_secundarios],bool individuos_binario[][usuarios_secundarios],float individuos_decimal[ind]);
void resul(float individuos_decimal[ind],float func[ind]);
void calculos(int ren,int col,float func[ind],float individuos_decimal[ind],float acum[ind],bool individuos_binario[][usuarios_secundarios]);

float calculos_sinr();
void inicializa_todos_los_enlaces();

