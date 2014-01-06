#include "genetico.h"

	// Vectores sinr de la corrida
  float *sinr_pu_local;
  float *sinr_su_local;

//funcion principal
void main(){
  // Inicializa semilla aleatorios
  srand (time(NULL));

	//declaracion de variables
	int num,div,res,i,ren,col,col2,temp,ren2,cont;
	
	// individuos en binario
	bool individuos_binario[ind][usuarios_secundarios];
	int selec[ind][usuarios_secundarios];
	
	// Individuos en decimal
	float individuos_decimal[ind];
	int padres[ind];
	float func[ind];
	float acum[ind];
	srand(time(NULL));
	srand48(time(NULL));
	
	//Inicializamos los enlaces primarios y secundarios
	inicializa_todos_los_enlaces();

	// Guarda los datos en el archivo
  datos_iniciales(nombre, usuarios_primarios, usuarios_secundarios, sinr_max, 0.00, gen);
  guarda_coordenadas(nombre, pu_grupo, usuarios_primarios, su_grupo, usuarios_primarios);

	//Inicializa a los individuos del algoritmo genetico
	for(i = 0; i < ind; i++){
	  /* Utilizamos modulo para sacar valores enteros aleatorios,
	     con el fin de que sean convertidos facilmente a binario */
		num= rand() % (int)pow(2, usuarios_secundarios);
		individuos_decimal[i]=num;
		int estado;
		for(estado = 0; estado < usuarios_secundarios; estado++){			
			div = num / 2;
			res = num % 2;
			individuos_binario[i][estado] = res;
		}
	}

	printf("individuos seleccionados\n");
	do{
		calculos(ren,col,func,individuos_decimal,acum,individuos_binario);
		printf("\n");
		
		// Regresa los individuos que seran padres.
		padres[ind] = SeleccionPadres(padres, acum);
		
		printf("Padres seleccionados: ");
		cont=0;
		//mostar a los padres seleccionados
		for(i = 0; i < ind; i++){
		  // Almacena el valor decimal del individuo
			ren2 = padres[i];
			ren=0;
			col2 = usuarios_secundarios - 1;
			
			do{
				ren++;
				if(ren == ren2){	
					for(col=0;col<usuarios_secundarios;col++){
						selec[cont][col]=individuos_binario[ind-ren2][col2];
						col2--;
					}
					cont++;
				}
			}while(ren!=ren2);
		  printf("%d ",padres[i]);
		} // */
		printf("\n");
		
		cruzamiento(pc,selec); //llamar a la funcion de cruzamiento
		mutacion(pm, selec); //llamar a la funcion de mutacion
		sustituir(selec,individuos_binario,individuos_decimal); //sustituir a los padres por los hijos generados
		printf("DESPUES DE %d GENERACIONES EVALUADAS...\n",generacion);

		generacion++;
	} while(generacion < gen);	
	calculos(ren,col,func,individuos_decimal,acum,individuos_binario);
	resul(individuos_decimal,func);
	marcas_finales(nombre, ultimo_cambio, global);
}


int SeleccionPadres(int padres[ind],float acum[ind]){
	/* Las seleccion de padres se realiza mediante la generacion de numeros 
	   aleatorios. La probabilidad de que sea seleccionado el individuo es 
	   proporcional a su evaluacion de la función fitness 
	*/

	int i,x;
	float lista_aleatorios[ind];

  // Generacion de lista de numeros aleatorios
  printf("Aleatorios generados: ");
	for(i=0;i<ind;i++){
		lista_aleatorios[i] = drand48() * 1;
	}
	printf("\n");

	for(x = 0; x < ind; x++){
		i=ind;
		do{
			i--;
		} while(acum[i] <= lista_aleatorios[x]);

		padres[x]=(ind-i);
	}
	return padres[ind];
}

//funcion que realizara el cruzamiento
void cruzamiento(float pc, int selec[][usuarios_secundarios]){
	int i,ren,col,temp,lim1,lim2,lim3,lim4;
	float numAl;
	printf("\nProbabilidad de cruzamiento: %f\n",pc);
	//evaluando condicion numAleatorio<pc para cada pareja
	for(ren=0; ren<ind; ren=ren+2){
		numAl=drand48() * 1;
		if(numAl<pc){
			lim1=rand() % usuarios_secundarios;
			lim2=rand() % usuarios_secundarios;
			
			// Ordena los limites para que lim2 > lim1
			if(lim2<lim1){
				temp=lim2;
				lim2=lim1;
				lim1=temp;
			}
			
			if((lim2-lim1)==0 || (lim2-lim1)==1){
				lim1 = 0;
				lim2 = 2;
				lim3 = usuarios_secundarios - 1;
				lim4 = usuarios_secundarios;
				
				for(col=0; col<usuarios_secundarios; col++){
					if(col>=lim1 && col<=lim2){
						temp = selec[ren][col];
						selec[ren][col]=selec[ren+1][col];
						selec[ren+1][col]=temp;
					}
				}

				for(col=0;col<usuarios_secundarios;col++){
					if(col>=lim3 && col<=lim4){
						temp = selec[ren][col];
						selec[ren][col]=selec[ren+1][col];
						selec[ren+1][col]=temp;
					}
				}
			}
			else{
				//la condicion se cumplio por lo que se cruzan los bits de las parejas
				for(col=0; col<usuarios_secundarios; col++){
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
void mutacion(float pm, int selec[][usuarios_secundarios]){
	int ren,col,temp;
	float numAl;
	printf("probabilidad de mutacion: %f\n",pm); //mostrar la probabilidad de mutacion
	//recorrer cada arreglo para hacer la mutacion
	for(ren=0;ren<ind;ren++){
		for(col=0;col<usuarios_secundarios;col++){
			numAl=drand48() * 1;
			if(numAl < pm){ //comparar pm con el numero aleatorio
				if(selec[ren][col]==false)
					selec[ren][col]=true;
				else
					selec[ren][col]=false;
			}
		}
	}
}

//funcion de sustitucion de la nueva generacion en el arreglo principal
void sustituir(int selec[][usuarios_secundarios],bool individuos_binario[][usuarios_secundarios],float individuos_decimal[ind]){
	int i,col;
	int col2=0;
	int temp[usuarios_secundarios];
	
	for(i=0;i<ind;i++){
		for(col=0;col<usuarios_secundarios;col++){	
			individuos_binario[i][col] = selec[(ind - 1) - i][usuarios_secundarios - col];
			temp[col] = selec[(ind - 1) - i][usuarios_secundarios - col];
		}
	}
}

void calculos(int ren,int col,float func[ind],float individuos_decimal[ind],float acum[ind],bool individuos_binario[][usuarios_secundarios]){
	int usuario = usuarios_secundarios;
	
	// Variables que guardan el indice de los optimos local y global de la funcion
	int maximo_corrida;
	int maximo_global;
	
	// Variables que guardan valor de la corrida (optimo local)
	float local;
	
	float normal= 0.0; //funcion del problema
	float funcion;
	float sumatoria = 0.0;
	float acumulado = 0.0;
	
	// Vectores sinr de la corrida
//  float *sinr_pu_local;
//  float *sinr_su_local;
  
  // indice del maximo de la corrida
  int indice_local = 0;
  
	//evaluar la funcion y sacamos la sumatoria
	int i;
	for(i=0;i<ind;i++){
		//funcion = pow(individuos_decimal[i], 3) - 60 * pow(individuos_decimal[i], 2) + 900 * individuos_decimal[i] + 100;
		
		// Realizamos la evaluacion de los individuos, buscando la taza de transferencia
    funcion = calculos_sinr(individuos_binario[i]);

    if(funcion > global){
      global = funcion;
      ultimo_cambio = generacion;
    }
    
    printf(" ======================================================= funcion: %f\n", funcion);
    printf(" =======================================================  global: %f\n", global);
    if(funcion > local){
      indice_local = i;
      local = funcion;
      sinr_pu_local = sinr_pu;
      sinr_su_local = sinr_su;
    }

		func[i] = funcion;
		sumatoria = sumatoria + funcion;
	}
	  
	printf("\t\tfunc[i], \tnormal, \tacum[i]\n");
	//muestra a los individuos seleccionados
	for(i=i-1;i>=0;i--){
		for(usuario = usuario - 1; usuario >= 0; usuario--){
			printf("%d ",individuos_binario[i][usuario]);
		}

		normal = func[i]/sumatoria;  //se calcula f(x)normal
		acumulado = acumulado + normal; //se saca el cumulative
		acum[ren]=acumulado;
		printf("==> %f ==> %f ==> %f\n", func[i], normal, acum[i]);
		usuario = usuarios_secundarios;
	}

  // Fitness local de la corrida
  float fitness_local = fitness(sinr_pu, evaluacion_pu, aptitud_pu, usuarios_primarios, sinr_su, evaluacion_su, aptitud_su, usuarios_secundarios);
  printf("\nfitness: %f\n", fitness_local);

  // Guarda los resultados en el archivo cada vez que se cumple el salto de marca
  if(generacion % salto_marca == 0){
	  marcas(nombre, generacion, global, individuos_binario[indice_local], local, estado_del_espectro, sinr_pu_local, usuarios_primarios, sinr_su_local, usuarios_secundarios, su_grupo, fitness_local);
	}
}

//funcion para determinar el resultado optimo
void resul(float individuos_decimal[ind],float func[ind]){
	int i,optimo=0;
	float mayor=0.0;
	for(i=0;i<ind;i++){
		if(func[i] > mayor){
			mayor = func[i];
			optimo = individuos_decimal[i];
		}
	}
	
	// Optimo es el valor en decimal del individo
	printf("\nEL RESULTADO OPTIMO ES DE %d\n",optimo);
	printf("EL VALOR DE LA FUNCION ES DE %f\n",mayor);
	printf("max: %f\n", global);
}

float calculos_sinr(bool estados_su[]){
  // Calcula la relacion señal interferencia
  printf("\n\n");
  sinr_enlace_primario(pu_grupo, usuarios_primarios, estados_pu, sinr_pu, su_grupo, usuarios_secundarios, estados_su);
  sinr_enlace_secundario(pu_grupo, usuarios_primarios, estados_pu, su_grupo, usuarios_secundarios, estados_su, sinr_su);

  // Muestra sinr de pus en dB
  printf("\n\nSINRPri\n");
  int i;
  for(i = 0; i < usuarios_primarios; i++)
    printf("- %f, %f db\n", sinr_pu[i], watt_a_dB(sinr_pu[i]));

  printf("\n\nSINRSec\n");
  for(i = 0; i < usuarios_secundarios; i++)
    printf("- %f, %f db\n", sinr_su[i], watt_a_dB(sinr_su[i]));


  // Evalua la transmision en los dispositivos primarios
  printf("\nVector de evaluacion de PU\n");
  evaluacion_dispositivos(sinr_pu, evaluacion_pu, estados_pu, usuarios_primarios);

  for(i = 0; i < usuarios_primarios; i++)
    printf("%d,  ", evaluacion_pu[i]);

  printf("\n");

  // Evalua la transmision en los dispositivos secundarios
  printf("\nVector de evaluacion de SU\n");
  evaluacion_dispositivos(sinr_su, evaluacion_su, estados_su, usuarios_secundarios);

  for(i = 0; i < usuarios_secundarios; i++)
    printf("%d, ", evaluacion_su[i]);

  printf("\n");

  float fitness_total = fitness(sinr_pu, evaluacion_pu, aptitud_pu, usuarios_primarios, sinr_su, evaluacion_su, aptitud_su, usuarios_secundarios);
  printf("\nfitness: %f\n", fitness_total);
  
  return fitness_total;
}

void inicializa_todos_los_enlaces(){
  printf("Inicializando enlaces primarios... \n");
  inicializa_enlace_primario(pu_grupo, estado_del_espectro, usuarios_primarios);
  
  int i;
  for(i = 0; i < usuarios_primarios; i++){
    imprime_enlace(&pu_grupo[i]);
  }

  printf("\n\n\nInicializando encaces secundarios... \n");
  for(i = 0; i < usuarios_secundarios; i++){
    inicializa_enlace_secundario(&su_grupo[i], usuarios_primarios);
    imprime_enlace(&su_grupo[i]);
  }

  // Guarda las coordenadas en el archivo 
  guarda_coordenadas(nombre, pu_grupo, usuarios_primarios, su_grupo, usuarios_secundarios);
}
