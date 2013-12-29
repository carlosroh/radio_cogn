/* Algoritmo Genetico para Maximizacion*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "libs/enlaces.h"

#define PI 3.14159265


// Algoritmo genetico
#define individuos 5
int gen = 5;      // Numero de generaciones o interacciones
float pc = 0.75;  // Probabilidad de cruzamiento(rango de 0.65 a 0.80)
float pm = 0.03;  // Probabilidad de mutacion(no mayor a 0.05)

  // Variables del algoritmo genetico --------------------------------------------------------------------
  // El problema nos limita a maximo 10 usuarios primarios
  #define pu_total 1
  #define su_total 4

  float sinr_max = 10.0;
  
  // Almacena la relacion señal interferencia en watts
  float sinr_pu[individuos][pu_total];
  float sinr_su[individuos][su_total];


  // Las posiciones son estaticas
  enlace pu_grupo[pu_total];
  enlace su_grupo[su_total];

  // Vector que almacena en que canal esta trabajando cada PU
  int estado_del_espectro[pu_total];

  bool estados_pu[individuos][pu_total];
  bool estados_su[individuos][su_total];

  // Guarda en el vector la evaluacion de los dispositivos, true si
  // transmiten satisfactoriamente, o false si no se puede realizar
  //  la transmision.
  int evaluacion_pu[individuos][pu_total];
  int evaluacion_su[individuos][su_total];

  // --------------------------------------------------------------------------------------------------


int SeleccionPadres(int padres[individuos], float acumulado[individuos]);
void cruzamiento(float pc, int selec[][su_total]);
void mutacion(float pm, int selec[][su_total]);
void sustituir(int selec[][su_total],bool estados_su[][su_total],int arre2[individuos]);
int convertirDec(int temp[individuos],int arre2[individuos],int col2);

void calculos(int col,float func[individuos], float acumulado[individuos], bool estados_su[][su_total]);
void resul(int arre2[individuos],float func[individuos]);

//funcion principal
void main(){
  //declaracion de variables -----------------------------------------------------------------------------
  int generacion = 1;
  int num, div, i, j, ren, col, col2, temp, ren2, cont;
  int selec[individuos][su_total];
  int arre2[individuos];
  int padres[individuos];
  float func[individuos];
  float acumulado[individuos];

  srand(time(NULL));
  srand48(time(NULL));
  
  char *nombre = "salida.txt";
  datos_iniciales(nombre, pu_total, su_total, sinr_max, 0.00, gen);


  // Inicializa a los individuos  
  printf("Inicializando enlaces primarios... \n");
  inicializa_enlace_primario(pu_grupo, estado_del_espectro, pu_total);
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

  // Creacion de individuos
  for(ren = 0; ren < individuos; ren++){
    num = rand() % (int)pow(2, su_total);
    arre2[ren] = num;

    // convierte a binario
    int residuo;
    for(i = 0; i < su_total; i++){                        
      div = num / 2;
      residuo = num % 2;
      if(residuo == 1)
        estados_su[ren][i] = true;
      else
        estados_su[ren][i] = false;
    }
  }//termina la creacion de los individuos

  do{ // inician las interacciones
    printf("\n\n");
    for(i = 0; i < individuos; i++){ // Calcula la relacion señal interferencia
      sinr_enlace_primario(pu_grupo, pu_total, estados_pu[i], sinr_pu[i], su_grupo, su_total, estados_su[i]);
      sinr_enlace_secundario(pu_grupo, pu_total, estados_pu[i], su_grupo, su_total, estados_su[i], sinr_su[i]);
    }
    
    calculos(col, func, acumulado, estados_su);
    printf("\n");

    //llamar a la funcion que selecciona a los padres
    padres[individuos] = SeleccionPadres(padres,acumulado);
    printf("Padres seleccionados: ");
    cont=0;

    //TODO Investigar que hace esto :p
    //mostrar a los padres seleccionados
    for(i=0;i<individuos;i++){ // Recorre el arreglo de individuos
      ren2 = padres[i];   // Individuo actual
      ren = 0;
      col2 = 7;
      do{
        ren++; // Incrementa el individio
        if(ren == ren2){	
          for(col=0; col < su_total; col++){
            selec[cont][col] = estados_su[individuos - ren2][col2];
            col2--;
          }
          cont++;
        }
      }while(ren != ren2);
      printf("%d ",padres[i]);
    }
    printf("\n");

    cruzamiento(pc, selec); //llamar a la funcion de cruzamiento
    mutacion(pm, selec); //llamar a la funcion de mutacion
    sustituir(selec, estados_su, arre2); //sustituir a los padres por los hijos generados
    printf("DESPUES DE %d GENERACIONES EVALUADAS...\n", generacion);
    generacion++;
  }while(generacion<=gen);	
  
  
  calculos(col, func, acumulado, estados_su);
  resul(arre2,func);
}

//funcion para determinar quienes seran los padres
int SeleccionPadres(int padres[individuos],float acumulado[individuos])
{
  /*seleccion de numeros aleatorios para 
    determinar quienes seran padres*/
  int i,x;
  float Aleatorio[individuos]; // Arreglo de individuos

  // Generar lista de numeros aleatorios
  printf("aleatorios");
  for(i=0;i<individuos;i++){
    Aleatorio[i]= drand48() * 1;
    printf("%f ", Aleatorio[i]);
  }
  printf("\n");

  // Busca a los individuos mas "aptos"  
  for(x = 0; x < individuos; x++){
    i=individuos;
    do{
      i--;
    } while(acumulado[i] <= Aleatorio[x]);
    padres[x] = (individuos - i);
  }
  
  return padres[individuos];
}

//funcion que realizara el cruzamiento
void cruzamiento(float pc, int selec[][su_total]){
  int i,ren,col,temp,lim1,lim2,lim3,lim4;
  float numAl;
  printf("\nprobabilidad de cruzamiento: %f\n",pc);
  //evaluando condicion numAleatorio<pc
  for(ren=0; ren < individuos; ren = ren + 2){
    numAl=drand48() * 1;
    if(numAl<pc){
      lim1=rand() % su_total;
      lim2=rand() % su_total;
      if(lim2<lim1){
        temp=lim2;
        lim2=lim1;
        lim1=temp;
      }
      if((lim2-lim1)==0 || (lim2-lim1)==1){
        lim1=0;
        lim2=2;
        lim3=6;
        lim4=7;
        for(col=0;col<su_total;col++){
          if(col>=lim1 && col<=lim2){
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
        for(col=0;col<su_total;col++){
          if(col>=lim3 && col<=lim4){
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
      }
      else{
        //	printf("lim1: %d  lim2:%d\n",lim1,lim2);
        //la condicion se cumplio por lo que se cruzan los bits de las parejas
        for(col=0;col<su_total;col++){
          if(col>=lim1 && col<=lim2){
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
      }
    }
  }
}

//funcion que realizara la mutacion
void mutacion(float pm, int selec[][su_total]){
  int ren,col,temp;
  float numAl;
  printf("probabilidad de mutacion: %f\n",pm); //mostrar la probabilidad de mutacion
  //recorrer cada arreglo para hacer la mutacion
  for(ren=0;ren<individuos;ren++){
    for(col=0;col<su_total;col++){
      numAl=drand48() * 1;
      if(numAl<pm)//comparar pm con el numero aleatorio{
        if(selec[ren][col]==0)
          selec[ren][col] = true;
        else
          selec[ren][col] = false;
    }
  }
}


//funcion de sustitucion de la nueva generacion en el arreglo principal
void sustituir(int selec[][su_total],bool estados_su[][su_total],int arre2[individuos]){
  int ren,col;
  int col2=0;
  int temp[su_total];
  for(ren=0;ren<individuos;ren++){
    for(col=0;col<su_total;col++){
      estados_su[ren][col]=selec[(individuos-1)-ren][7-col];
      temp[col]=selec[(individuos-1)-ren][7-col];
    }
    col2=convertirDec(temp,arre2,col2);
  }
}

int convertirDec(int temp[individuos],int arre2[individuos],int col2){
  int i=0,suma=0;
  do{
    switch(i){
      case 0: if(temp[i] == true)
              {suma=suma+1;}
              break;
      case 1: if(temp[i] == true)
              {suma=suma+2;}
              break;
      case 2: if(temp[i] == true)
              {suma=suma+4;}
              break;
      case 3: if(temp[i] == true)
              {suma=suma+8;}
              break;
      case 4: if(temp[i] == true)
              {suma=suma+16;}
              break;
      case 5: if(temp[i] == true)
              {suma=suma+32;}
              break;
      case 6: if(temp[i] == true)
              {suma=suma+64;}
              break;
      case 7: if(temp[i] == true)
              {suma=suma+128;}
              break;
    }
    i++;
  }while(i < su_total);
  arre2[col2]=suma;
  col2++;
  return col2;
}

void calculos(int col, float func[individuos], float acumulado[individuos], bool estados_su[][su_total]){
  float normal = 0.0;       //funcion del problema
  float sumatoria = 0.0;
  float acumulado_fNormal = 0.0;  
  float funcion;           // Equivalente al fitness

  //evaluar la funcion y sacamos la sumatoria
  int i, j;
  for(i = 0; i < individuos; i++){
    funcion = *sinr_su[i]; // Estamos evaluando SINR
    func[i] = funcion;
    sumatoria = sumatoria + funcion;
  }

  // Muestra a los individuos seleccionados
  for(i = 0; i < individuos; i++){	
    for(j = 0; j < su_total; j++){
      printf("%d ", estados_su[i][j]);
    }

    normal = func[i] / sumatoria;  // se calcula f(x)normal
    acumulado_fNormal = acumulado_fNormal + normal;  //se saca el cumulative de fNormal
    acumulado[i] = acumulado_fNormal;
    printf("-> => fitness: %f => normal: %f => acumulado: %f\n", func[i], normal, acumulado[i]);
  }
}

//funcion para determinar el resultado optimo
void resul(int arre2[individuos],float func[individuos]){
  int i,optimo=0;
  float mayor=0.0;
  for(i=0;i<individuos;i++){
    if(func[i]>mayor){
      mayor=func[i];
      optimo=arre2[i];
    }
  }
  printf("\nEL RESULTADO OPTIMO ES DE %d\n",optimo);
  printf("EL VALOR DE LA FUNCION ES DE %f\n",mayor);
}
