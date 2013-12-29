// No olvidar inicializar lo siguiente en su archivo main...
/**
  int ind = 5;
  int gen = 5; //numero de generaciones
  float pc = 0.75; //=0.75; //probabilidad de cruzamiento(rango de 0.65 a 0.80)
  float pm = 0.0;  //=0.01; //probabilidad de mutacion(no mayor a 0.05)
*/

int SeleccionPadres(int padres[ind], float acum[ind]);
void cruzamiento(float pc, int selec[][8]);
void mutacion(float pm, int selec[][8]);
void sustituir(int selec[][8],int arre[][8],int arre2[ind]);
int convertirDec(int temp[ind],int arre2[ind],int col2);

