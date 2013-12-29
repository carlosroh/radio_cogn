#include <stdio.h>
#include "libs/enlaces.h"

int main(){
  srand (time(NULL));
  
  // Guardamos el total de las interacciones
  int interacciones = 1;
  float sinr_max = umbral_sinr;

  /**  El problema nos limita a maximo 10  *
   *   usuarios primarios                  */
  int pu_total = 1;
  int su_total = 2;

  enlace pu_grupo[pu_total];
  enlace su_grupo[su_total];

  // Vector que almacena en que canal esta trabajando cada PU
  int estado_del_espectro[pu_total];

//  bool estados_pu[pu_total];
//  bool estados_su[su_total];
  bool estados_pu[] = {true, true, true, false, true, true};
  bool estados_su[] = {true, true, true, true, true, true, true, true, true, false};

  // Almacena la relacion señal interferencia en watts
  float sinr_pu[pu_total];
  float sinr_su[su_total];

  // Guarda en el vector la evaluacion de los dispositivos, true si
  // transmiten satisfactoriamente, o false si no se puede realizar
  //  la transmision.
  int evaluacion_pu[pu_total];
  int evaluacion_su[su_total];
  
  // En los vectores de aptitud se guardan las tazas de transferencia de los enlaces
  float aptitud_pu[pu_total];
  float aptitud_su[su_total];

  // Inicializando el archivo...
  char *nombre = "salida.txt";
  datos_iniciales(nombre, pu_total, su_total, sinr_max, 0.00, interacciones);
  //marcas(nombre);
  
  printf("Inicializando enlaces primarios... \n");
  inicializa_enlace_primario(pu_grupo, estado_del_espectro, pu_total);
  
  int i;
  for(i = 0; i < pu_total; i++){
    imprime_enlace(&pu_grupo[i]);
  }

  printf("\n\n\nInicializando encaces secundarios... \n");
  for(i = 0; i < su_total; i++){
    inicializa_enlace_secundario(&su_grupo[i], pu_total);
    imprime_enlace(&su_grupo[i]);
  }

  // Guarda las coordenadas en el archivo
  guarda_coordenadas(nombre, pu_grupo, pu_total, su_grupo, su_total);

  // Calcula la relacion señal interferencia
  printf("\n\n");
  sinr_enlace_primario(pu_grupo, pu_total, estados_pu, sinr_pu, su_grupo, su_total, estados_su);
  sinr_enlace_secundario(pu_grupo, pu_total, estados_pu, su_grupo, su_total, estados_su, sinr_su);


  // Muestra sinr de pus en dB
  printf("\n\nSINRPri\n");
  for(i = 0; i < pu_total; i++)
    printf("- %f, %f db\n", sinr_pu[i], watt_a_dB(sinr_pu[i]));

  printf("\n\nSINRSec\n");
  for(i = 0; i < su_total; i++)
    printf("- %f, %f db\n", sinr_su[i], watt_a_dB(sinr_su[i]));


  // Evalua la transmision en los dispositivos primarios
  printf("\nVector de evaluacion de PU\n");
  evaluacion_dispositivos(sinr_pu, evaluacion_pu, estados_pu, pu_total);

  for(i = 0; i < pu_total; i++)
    printf("%d,  ", evaluacion_pu[i]);

  printf("\n");

  // Evalua la transmision en los dispositivos secundarios
  printf("\nVector de evaluacion de SU\n");
  evaluacion_dispositivos(sinr_su, evaluacion_su, estados_su, su_total);

  for(i = 0; i < su_total; i++)
    printf("%d, ", evaluacion_su[i]);

  printf("\n");

  float fitness_corrida = fitness(sinr_pu, evaluacion_pu, aptitud_pu, pu_total, sinr_su, evaluacion_su, aptitud_su, su_total);
  printf("\nfitness: %f\n", fitness_corrida);
  
  int global = 0;
  float max_taza_datos = 0.0;
  marcas(nombre, interacciones, global, estados_su, max_taza_datos, estado_del_espectro, sinr_pu, pu_total, sinr_su, su_total, su_grupo, fitness_corrida);
}
