#include "algoritmo_genetico.h"

//funcion para determinar quienes seran los padres
int SeleccionPadres(int padres[ind],float acum[ind])
{
  /*seleccion de numeros aleatorios para 
    determinar quienes seran padres*/
  int i,x;	
  float nAl;
  float Aleatorio[ind];
  //srand48(time(NULL));
  for(i=0;i<ind;i++)
  {
    nAl=drand48() * 1;
    Aleatorio[i]=nAl;
  }
  for(i=0;i<ind;i++)
  {
    printf("%f ",Aleatorio[i]);
  }
  for(x=0;x<ind;x++)
  {
    i=ind;
    do
    {
      i--;
    }while(acum[i]<=Aleatorio[x]);
    padres[x]=(ind-i);
  }
  printf("\n");
  return padres[ind];
}
//funcion que realizara el cruzamiento
void cruzamiento(float pc, int selec[][8])
{
  int i,ren,col,temp,lim1,lim2,lim3,lim4;
  float numAl;
  printf("\nprobabilidad de cruzamiento: %f\n",pc);
  //evaluando condicion numAleatorio<pc
  for(ren=0;ren<ind;ren=ren+2)
  {
    numAl=drand48() * 1;
    if(numAl<pc)
    {
      lim1=rand() % 8;
      lim2=rand() % 8;
      if(lim2<lim1)
      {
        temp=lim2;
        lim2=lim1;
        lim1=temp;
      }
      if((lim2-lim1)==0 || (lim2-lim1)==1)
      {
        lim1=0;
        lim2=2;
        lim3=6;
        lim4=7;
        for(col=0;col<8;col++)
        {
          if(col>=lim1 && col<=lim2)
          {
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
        for(col=0;col<8;col++)
        {
          if(col>=lim3 && col<=lim4)
          {
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
      }
      else
      {
        //	printf("lim1: %d  lim2:%d\n",lim1,lim2);
        //la condicion se cumplio por lo que se cruzan los bits de las parejas
        for(col=0;col<8;col++)
        {
          if(col>=lim1 && col<=lim2)
          {
            temp = selec[ren][col];
            selec[ren][col]=selec[ren+1][col];
            selec[ren+1][col]=temp;
          }
        }
      }
    }
  }
  /*printf("\nDespues del cruzamiento\n");
    for(ren=0;ren<ind;ren++)
    {	
    for(i=0;i<8;i++)
    {
    printf("%d ",selec[ren][i]);
    }
    printf("\n");
    }*/
  //getchar();
}

//funcion que realizara la mutacion
void mutacion(float pm, int selec[][8])
{
  int ren,col,temp;
  float numAl;
  printf("probabilidad de mutacion: %f\n",pm); //mostrar la probabilidad de mutacion
  //recorrer cada arreglo para hacer la mutacion
  for(ren=0;ren<ind;ren++)
  {
    for(col=0;col<8;col++)
    {
      numAl=drand48() * 1;
      if(numAl<pm)//comparar pm con el numero aleatorio
      {
        if(selec[ren][col]==0)
          selec[ren][col]=1;
        else
          selec[ren][col]=0;
      }
    }
  }
}

//funcion de sustitucion de la nueva generacion en el arreglo principal
void sustituir(int selec[][8],int arre[][8],int arre2[ind])
{
  int ren,col;
  int col2=0;
  int temp[8];
  for(ren=0;ren<ind;ren++)
  {
    for(col=0;col<8;col++)
    {	
      arre[ren][col]=selec[(ind-1)-ren][7-col];
      temp[col]=selec[(ind-1)-ren][7-col];
    }
    col2=convertirDec(temp,arre2,col2);
  }
}

int convertirDec(int temp[ind],int arre2[ind],int col2)
{
  int i=0,suma=0;
  do
  {
    switch(i)
    {
      case 0: if(temp[i]==1)
              {suma=suma+1;}
              break;
      case 1: if(temp[i]==1)
              {suma=suma+2;}
              break;
      case 2: if(temp[i]==1)
              {suma=suma+4;}
              break;
      case 3: if(temp[i]==1)
              {suma=suma+8;}
              break;
      case 4: if(temp[i]==1)
              {suma=suma+16;}
              break;
      case 5: if(temp[i]==1)
              {suma=suma+32;}
              break;
      case 6: if(temp[i]==1)
              {suma=suma+64;}
              break;
      case 7: if(temp[i]==1)
              {suma=suma+128;}
              break;
    }
    i++;
  }while(i<=7);
  arre2[col2]=suma;
  col2++;
  return col2;
}

void calculos(int ren,int col,float func[ind],int arre2[ind],float acum[ind],int arre[][8]){
  int i=8;	
  float normal= 0.0; //funcion del problema
  float funcion;
  float sumatoria = 0.0;
  float acumulado = 0.0;

  //evaluar la funcion y sacamos la sumatoria
  for(ren=0;ren<ind;ren++){
    funcion = sin(PI * arre2[ren]/256);
    func[ren]=funcion;
    sumatoria=sumatoria+funcion;
  }

  //muestra a los individuos seleccionados
  for(ren=ren-1;ren>=0;ren--){	
    for(i=i-1;i>=0;i--){
      printf("%d ",arre[ren][i]);
    }

    normal = func[ren]/sumatoria;  //se calcula f(x)normal
    acumulado = acumulado + normal; //se saca el cumulative
    acum[ren]=acumulado;
    printf("==> %d  ==> %f  ==> %f  ==> %f\n",arre2[ren],func[ren],normal,acum[ren]);
    i=8;
  }
}

//funcion para determinar el resultado optimo
void resul(int arre2[ind],float func[ind]){
  int i,optimo=0;
  float mayor=0.0;
  for(i=0;i<ind;i++){
    if(func[i]>mayor){
      mayor=func[i];
      optimo=arre2[i];
    }
  }
  printf("\nEL RESULTADO OPTIMO ES DE %d\n",optimo);
  printf("EL VALOR DE LA FUNCION ES DE %f\n",mayor);
}
