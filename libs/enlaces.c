#include "enlaces.h"

void inicializa_enlace_secundario(enlace *link, int canales_total){
  link->canal = rand() % canales_total;
  link->Tx.x = rand() % MAPA_DISTANCIA_X;
  link->Tx.y = rand() % MAPA_DISTANCIA_Y;

  int tx, rx;
  do{
    link->Rx.x = rand() %  MAPA_DISTANCIA_X;
    link->Rx.y = rand() %  MAPA_DISTANCIA_Y;
  }while(rango_no_permitido(link));
}

float distancia_enlace(enlace *link){
  return sqrt(pow(link->Tx.x - link->Rx.x, 2) + pow(link->Tx.y - link->Rx.y, 2));
}

float distancia_enlace_en_kms(enlace *link){
  return distancia_enlace(link) / 1000;
}

// Calcula la distancia entre el transmisor y el receptor de diferentes enlaces.
float distancia_entre_enlaces(enlace *transmisor, enlace *receptor){
  return sqrt(pow(transmisor->Tx.x - receptor->Rx.x, 2) + pow(transmisor->Tx.y - receptor->Rx.y, 2));
}

float distancia_entre_enlaces_en_kms(enlace *transmisor, enlace *receptor){
  return distancia_entre_enlaces(transmisor, receptor) / 1000;
}

bool mismo_canal(enlace *link, enlace *link2){
  return link->canal == link2->canal;
}

// Regresa el estado actual
bool esta_transmitiendo(bool estado){
  return estado;
}

void sinr_enlace_primario(enlace *enlaces_primarios, int total_canales, bool *estados_pu, float *sinr_pu, enlace *enlaces_secundarios, int total_secundarios, bool *estados_su){
  printf("sinr enlaces primarios!!!!\n");
  // Comenzamos a evaluar los enlaces
  int i;
  for(i = 0; i < total_canales; i++){
    int j;
    float sinr = 0.0;
    
    /** sumatoria de las distancias entre el enlace 
      * que se esta evaluando y los dispositivos 
      * secundarios dentro del mismo canal */
    float dss = 0.0;

    // Contaremos los dispositivos que interfieren con el enlace
    int dispositivos_interferentes = 0;
    printf("Enlace a analizar...\n");
    // Mostramos el enlace que analizaremos en ese momento
    imprime_enlace(&enlaces_primarios[i]);

    if(esta_transmitiendo(estados_pu[i])){
      // Buscamos los enlaces secundarios que evaluaremos
      for(j = 0; j <= total_secundarios; j++){
        // Si esta dentro del mismo canal, comienza a guardar la distancia 
        // del transmisor secundario con la del receptor primario
        if(mismo_canal(&enlaces_primarios[i], &enlaces_secundarios[j]) && esta_transmitiendo(estados_su[j])){
          dispositivos_interferentes++;
          imprime_enlace(&enlaces_secundarios[j]);
          float distancia = distancia_entre_enlaces_en_kms(&enlaces_secundarios[j], &enlaces_primarios[i]);
          dss += P / pow(distancia, 4);
          printf("dss: %f distancia: %f \n", dss, distancia);
        }
      }

      printf("total interferentes: %d\n", dispositivos_interferentes);
      if(dispositivos_interferentes == 0){
        dss = 1.0;
      }
      
      printf("Sumatoria de las distancias SUs: %f\n", dss);

      // Distancia del enlace primario
      float dps = distancia_enlace_en_kms(&enlaces_primarios[i]);
      printf("Distancia del PU evaluado: %f\n", dps);

      sinr = pow(P / dps, 4) / dss;
      sinr_pu[i] = sinr;
      printf("SINR: %f watts\n\n", sinr);
    } else { 
      printf("No esta transmitiendo.\n\n");
      sinr_pu[i] = 0.0;
    }
  }
}

void sinr_enlace_secundario(enlace *enlaces_primarios, int total_canales, bool *estados_pu, enlace *enlaces_secundarios, int total_secundarios, bool *estados_su, float *sinr_su){
  printf("sinr enlaces secundarios!!!!\n");
  int i;
  // Comienza a comparar los SUs
  for(i = 0; i < total_secundarios; i++){
    int j;
    float sinr = 0.0;
    float dss = 0.0;
    float dps = 0.0;

    printf("Enlace a analizar...\n");
    imprime_enlace(&enlaces_secundarios[i]);

    if(esta_transmitiendo(estados_su[i])){
      for(j = 0; j <= total_secundarios; j++){
        // Verifica que no sea el mismo dispositivo, y que este dentro del mismo canal
        if((i != j) && mismo_canal(&enlaces_secundarios[i], &enlaces_secundarios[j]) && esta_transmitiendo(estados_su[j])){
          float distancia =  distancia_entre_enlaces_en_kms(&enlaces_secundarios[j], &enlaces_secundarios[i]);
          dss += P / pow(distancia, 4);
          imprime_enlace(&enlaces_secundarios[j]);
          printf("dss: %f distancia: %f \n", dss, distancia);
        }
      }

      printf("Sumatoria de las distancias SUs: %f\n", dss);

      int c = enlaces_secundarios[i].canal;
      j = -1;
      // Recuerda, el total de canales = total de usuarios primarios
      for(j = -1; j < total_canales; j++){
        if(mismo_canal(&enlaces_secundarios[i], &enlaces_primarios[j])  && esta_transmitiendo(estados_pu[j])){
          dps = distancia_entre_enlaces_en_kms(&enlaces_primarios[j], &enlaces_secundarios[i]);
          printf(" Enlace primario...\n ");
          imprime_enlace(&enlaces_primarios[j]);
          printf("Distancia del RxSU al TxPU: %f\n", dps);
        }
      }

      // Distancia del enlace que se esta evaluando
      float lds =  distancia_enlace_en_kms(&enlaces_secundarios[i]);
      printf("Distancia del enlace evaluado: %f\n", lds);

      // Se calcula SINRs
      sinr = pow(P / lds, 4) / (dss + pow(P / dps, 4));

      // Los resultados en los calculos se guardan en watt
      sinr_su[i] = sinr;
      printf("SINR: %f\n\n", sinr);
    } else {
      printf("No esta transmitiendo.\n\n");
      sinr_su[i] = 0.0;
    }
  }
}

bool rango_no_permitido(enlace *link){
  return distancia_enlace(link) == 0 || distancia_enlace(link) > 1000;
}

void inicializa_enlace_primario(enlace *link, int *estado_del_espectro, int total_pu){
  printf("total primarios: %d\n", total_pu);
  switch(total_pu){
    case 0:
      break;
    case 1:
      link[0].Tx.x = 3500; link[0].Tx.y = 2500; link[0].Rx.x = 2500; link[0].Rx.y = 2500;
      break;
    case 2:
      link[0].Tx.x = 2200; link[0].Tx.y = 200; link[0].Rx.x = 2907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 4200; link[1].Rx.x = 2907; link[1].Rx.y = 4907;
      break;
    case 3:
      link[0].Tx.x = 2200; link[0].Tx.y = 200; link[0].Rx.x = 2907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 2200; link[1].Rx.x = 2907; link[1].Rx.y = 2907;
      link[2].Tx.x = 2200; link[2].Tx.y = 4200; link[2].Rx.x = 2907; link[2].Rx.y = 4907;
      break;
    case 4:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 4200; link[1].Tx.y = 200; link[1].Rx.x = 4907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 4200; link[2].Rx.x = 4907; link[2].Rx.y = 4907;
      link[3].Tx.x = 200; link[3].Tx.y = 4200; link[3].Rx.x = 907; link[3].Rx.y = 4907;
      break;
    case 5:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 4200; link[1].Tx.y = 200; link[1].Rx.x = 4907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 4200; link[2].Rx.x = 4907; link[2].Rx.y = 4907;
      link[3].Tx.x = 200; link[3].Tx.y = 4200; link[3].Rx.x = 907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 2200; link[4].Rx.x = 2907; link[4].Rx.y = 2907;
      break;
    case 6:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 200; link[1].Rx.x = 2907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 200; link[2].Rx.x = 4907; link[2].Rx.y = 907;
      link[3].Tx.x = 4200; link[3].Tx.y = 4200; link[3].Rx.x = 4907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 4200; link[4].Rx.x = 2907; link[4].Rx.y = 4907;
      link[5].Tx.x = 200; link[5].Tx.y = 4200; link[5].Rx.x = 907; link[5].Rx.y = 4907;
      break;
    case 7:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 200; link[1].Rx.x = 2907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 200; link[2].Rx.x = 4907; link[2].Rx.y = 907;
      link[3].Tx.x = 4200; link[3].Tx.y = 4200; link[3].Rx.x = 4907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 4200; link[4].Rx.x = 2907; link[4].Rx.y = 4907;
      link[5].Tx.x = 200; link[5].Tx.y = 4200; link[5].Rx.x = 907; link[5].Rx.y = 4907;
      link[6].Tx.x = 2200; link[6].Tx.y = 2200; link[6].Rx.x = 2907; link[6].Rx.y = 2907;
      break;
    case 8:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 200; link[1].Rx.x = 2907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 200; link[2].Rx.x = 4907; link[2].Rx.y = 907;
      link[3].Tx.x = 4200; link[3].Tx.y = 4200; link[3].Rx.x = 4907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 4200; link[4].Rx.x = 2907; link[4].Rx.y = 4907;
      link[5].Tx.x = 200; link[5].Tx.y = 4200; link[5].Rx.x = 907; link[5].Rx.y = 4907;
      link[6].Tx.x = 200; link[6].Tx.y = 2200; link[6].Rx.x = 907; link[6].Rx.y = 2907;
      link[7].Tx.x = 4200; link[7].Tx.y = 2200; link[7].Rx.x = 4907; link[7].Rx.y = 2907;
      break;
    case 9:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 200; link[1].Rx.x = 2907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 200; link[2].Rx.x = 4907; link[2].Rx.y = 907;
      link[3].Tx.x = 4200; link[3].Tx.y = 4200; link[3].Rx.x = 4907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 4200; link[4].Rx.x = 2907; link[4].Rx.y = 4907;
      link[5].Tx.x = 200; link[5].Tx.y = 4200; link[5].Rx.x = 907; link[5].Rx.y = 4907;
      link[6].Tx.x = 200; link[6].Tx.y = 2200; link[6].Rx.x = 907; link[6].Rx.y = 2907;
      link[7].Tx.x = 2200; link[7].Tx.y = 2200; link[7].Rx.x = 2907; link[7].Rx.y = 2907;
      link[8].Tx.x = 4200; link[8].Tx.y = 2200; link[8].Rx.x = 4907; link[8].Rx.y = 2907;
      break;
    case 10:
      link[0].Tx.x = 200; link[0].Tx.y = 200; link[0].Rx.x = 907; link[0].Rx.y = 907;
      link[1].Tx.x = 2200; link[1].Tx.y = 200; link[1].Rx.x = 2907; link[1].Rx.y = 907;
      link[2].Tx.x = 4200; link[2].Tx.y = 200; link[2].Rx.x = 4907; link[2].Rx.y = 907;
      link[3].Tx.x = 4200; link[3].Tx.y = 4200; link[3].Rx.x = 4907; link[3].Rx.y = 4907;
      link[4].Tx.x = 2200; link[4].Tx.y = 4200; link[4].Rx.x = 2907; link[4].Rx.y = 4907;
      link[5].Tx.x = 200; link[5].Tx.y = 4200; link[5].Rx.x = 907; link[5].Rx.y = 4907;
      link[6].Tx.x = 200; link[6].Tx.y = 2200; link[6].Rx.x = 907; link[6].Rx.y = 2907;
      link[7].Tx.x = 1200; link[7].Tx.y = 2200; link[7].Rx.x = 1907; link[7].Rx.y = 2907;
      link[8].Tx.x = 3200; link[8].Tx.y = 2200; link[8].Rx.x = 3907; link[8].Rx.y = 2907;
      link[9].Tx.x = 4200; link[9].Tx.y = 2200; link[9].Rx.x = 4907; link[9].Rx.y = 2907;
      break;
  }
  
  int i;
  for(i = 0; i < total_pu; i++)
    link[i].canal = -1;
  
  int canal;
  for(canal = 0; canal < total_pu; canal++){
    i = rand() % total_pu;
    if(link[i].canal == -1){
      link[i].canal = canal;
      estado_del_espectro[canal] = i;
    }
    else
      canal--;
  }
}

void imprime_enlace(enlace *link){
  printf("Canal: %d, Tx: [%d, %d], Rx: [%d, %d]\n", link->canal, link->Tx.x, link->Tx.y, link->Rx.x, link->Rx.y);
}

float watt_a_dB(float watt){
  if(watt > 0.0)
    return 10*log10(1000*watt);
  else
    return 0.0;
}

// Regresa true si todos los enlaces se comunican sin problema,
// en caso contrario, regresa false
bool todos_transmiten(int *evaluacion, int total){
  int i;
  for(i = 0; i < total; i++){
    if(evaluacion[i]==0){
      return false;
    }
  }
  
  return true;
}

float tasa_de_transferencia(float sinr){
  #define B 1
  // Recuerda: log10(X)/log10(2) == log2(x)
  return B*log10(1 + sinr)/log10(2);
}

// Devuelve la taza de trasferencia de todos los dispositivos primarios y secundarios
float fitness(float *sinr_pu, int *evaluacion_pu, float *aptitud_pu, int total_pu, float *sinr_su, int *evaluacion_su, float *aptitud_su, int total_su){
  float fitness = 0.0;
  
  // Verificamos que ningun dispositivo este sin transmitir
  if(!todos_transmiten(evaluacion_pu, total_pu) || !todos_transmiten(evaluacion_su, total_su)){
    //Si hay dispositivos sin transmitir, penaliza la solucion regresando 0.0Mbps
    printf("Hay dispositivos sin transmitir.\n");
    return 0.0;
  }

  int i; printf("\nfitness: ");
  for(i = 0; i < total_pu; i++){
    aptitud_pu[i] = tasa_de_transferencia(sinr_pu[i]);
    printf(" %f + ", aptitud_pu[i]);
    fitness += aptitud_pu[i];
  }
  printf("= %f", fitness);

  printf("\nfitness: ");
  for(i = 0; i < total_su; i++){
    aptitud_su[i] = tasa_de_transferencia(sinr_su[i]);
    printf(" %f + ", aptitud_pu[i]);
    fitness += aptitud_su[i];
  }
  printf("= %f", fitness);

  printf("\nfitness corrida: %f\n", fitness);
  return fitness;
}

void evaluacion_dispositivos(float *sinr_dispositivos, int *evaluacion_dispositivos, bool *estados, int total_dispositivos){
  int i;
  for(i = 0; i < total_dispositivos; i++){
    if(estados[i] == false){
      // Si no esta transmitiendo, su evaluacion sera -1000
      evaluacion_dispositivos[i] = -1000;
    } else if(watt_a_dB(sinr_dispositivos[i]) > umbral_sinr){
      // Si esta transimitiendo correctamente
      evaluacion_dispositivos[i] = 1;
    } else {
      // Si la transmision es mala
      evaluacion_dispositivos[i] = 0;
    }
  }
}

void datos_iniciales(char *nombre, int pus, int sus, float sinrdb, float sinrln, int iter){
// Inicio del programa
  FILE *archivo;
  archivo = fopen(nombre, "w");
  fprintf(archivo, "Numero de enlaces primarios: %d\n", pus);
  fprintf(archivo, "Numero de enlaces secundarios: %d\n", sus);
  fprintf(archivo, "SINRL en dB's: %f\n", sinrdb);
  fprintf(archivo, "SINRL en unidades lineales: %f\n", sinrln);
  fprintf(archivo, "Total particulas: %d\n", pus + sus);
  fprintf(archivo, "Total iteraciones: %d\n", iter);
  fclose(archivo);
}

void guarda_coordenadas(char *nombre, enlace *pus, int total_pus, enlace *sus, int total_sus){
  FILE *archivo; 
  archivo = fopen(nombre, "a+");
  fprintf(archivo, "\n ** Matriz de coordenadas de SU's **\n");
  int i;
  for(i = 0; i < total_sus; i++){
    fprintf(archivo, "\t%d, %d, %d, %d\n", sus[i].Tx.x, sus[i].Tx.y, sus[i].Rx.x, sus[i].Rx.y);
  }

  fprintf(archivo, "\n ** Matriz de coordenadas de PU's **\n");
  for(i = 0; i < total_pus; i++){
    fprintf(archivo, "\t%d, %d, %d, %d\n", pus[i].Tx.x, pus[i].Tx.y, pus[i].Rx.x, pus[i].Rx.y);
  }
  fclose(archivo);
}

void marcas(char *nombre, int iteraciones, int global, bool *estados_su, float max_taza_datos, int *estado_del_espectro, float *sinr_pu, int pu_total, float *sinr_su, int su_total, enlace *su_grupo, float fitness){
  FILE *archivo;
  // a+ es para que agrege contenido al final del archivo
  archivo = fopen(nombre, "a+");
  fprintf(archivo, "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Iteraciones: %d\n", iteraciones);
            printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Iteraciones: %d\n", iteraciones);

  // Individuo que tiene la solucion
  fprintf(archivo, "\nIndice de global: %d\n", global);
            printf("\nIndice de global: %d\n", global);

  // Enlaces secundarios seleccionados por el individuo anterior
  fprintf(archivo, "\n ** Enlaces seleccionados ** \n");
            printf("\n ** Enlaces seleccionados ** \n");
  
  int i;
  for(i = 0; i < su_total; i++){
    if(estados_su[i] == true){
      fprintf(archivo, "Enlace %d, ", i);
                printf("Enlace %d, ", i);
    }
  }


  // Maxima taza de datos que obtuvo el individuo
  fprintf(archivo, "\n\nMaxima tasa de datos (Mbps): %f\n", max_taza_datos);
            printf("\n\nMaxima tasa de datos (Mbps): %f\n", max_taza_datos);

  // Vector de estado del espectro (canal[usuario])
  fprintf(archivo, "\n ** Estado del espectro **\n");
            printf("\n ** Estado del espectro **\n");
  
  for(i = 0; i < pu_total; i++){
              printf("%d, ", estado_del_espectro[i]);
    fprintf(archivo, "%d, ", estado_del_espectro[i]);
  }


  // Asignacion de canales de los SUs
  fprintf(archivo, "\n\n ** Vector de SU's (Asignacion de canales a los SU's) **\n");
            printf("\n\n ** Vector de SU's (Asignacion de canales a los SU's) **\n");
  
  for(i = 0; i < su_total; i++){
              printf("%d, ", su_grupo[i].canal);
    fprintf(archivo, "%d, ", su_grupo[i].canal);
  }

  // SINR enlaces
  fprintf(archivo, "\n\n ** Vector del SIR de enlaces primarios **\n");
            printf("\n\n ** Vector del SIR de enlaces primarios (dB)**\n");
  
  for(i = 0; i < pu_total; i++){
              printf("%d:%f, ", i, watt_a_dB(sinr_pu[i]));
    fprintf(archivo, "%d:%f, ", i, sinr_pu[i]);
  }


  fprintf(archivo, "\n\n ** Vector del SIR de enlaces secundarios **\n");
            printf("\n\n ** Vector del SIR de enlaces secundarios (dB)**\n");
            
  for(i = 0; i < su_total; i++){
              printf("%d:%f, ", i, watt_a_dB(sinr_su[i]));
    fprintf(archivo, "%d:%f, ", i, sinr_su[i]);
  }

  // R PARA MARCAR LA ITERACCION, FITNESS EN ESA ITERACION
  fprintf(archivo, "\n\nR %d, %f\n\n\n", iteraciones, fitness);
            printf("\n\nR %d, %f\n\n\n", iteraciones, fitness);
  fclose(archivo);
}

void marcas_finales(char *nombre, int ultimo_cambio, float global, float outage, float fitness, int su_total){
  FILE *archivo;
  archivo = fopen(nombre, "a+");
  
  // IT MARCA ITERACION EN QUE YA NO HUBO CAMBIO DE LA SOLUCION, FITNESS
  fprintf(archivo, "\n\nIT %d, %f\n", ultimo_cambio, global);
  
  //OUTAGE MARCA LA RELACION ENTRE SOLUCIONES VALIDAS/SOLUCIONES NO VALIDAS
  fprintf(archivo, "OUTAGE %f\n", outage);
  
  // RR PARA MARCAR RESULTADO DE LA CORRIDA* FITNESS*ENLACES SECUNDARIOS SELECCIONADOS* TOTAL DE ENLACES SECUNDARIOS SELECCIONADOS* RELACION SEÑAL A INTERFERENCIA DE SECUNDARIOS* RELACION SEÑAL A INTERFERENCIA DE PRIMARIOS* ASIGNACION DE CANALES PARA ENLACES SECUNDARIOS* TIEMPO DE EJECUCION DE LA CORRIDA EN MILISEGUNDOS (PUEDES MANEJARLA EN SEGUNDOS)
  fprintf(archivo, "RR %f, *", fitness);
  
  // imprime la lista de enlaces secundarios seleccionados
  int i;
  for(i = 0; i < su_total; i++){
  }
  
  fprintf(archivo, " %d, *", su_total);
  
  fclose(archivo);
}

