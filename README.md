radio_cogn
==========

El objetivo de este programa es maximizar la tasa de transferencia de datos de una red cognoscitiva.

### Para probarlo...
+ Primeramente compilamos nuestras librerias
#####  gcc -c libs/enlaces.c -o libs/enlaces.o

+ Despues compilamos nuestro archivo main, junto con nuestras librerias y lo guardamos en la carpeta bin. Yo le puse de nombre al ejecutable test, porque aún esta en fase de pruebas
#####  gcc main.c libs/enlaces.o -o bin/test -lm

+ Y finalmente ejecutamos el programa
#####  ./bin/test

### Para desarrollar el programa

Para desarrollar este programa, se necesita:

+ Un editor de textos
+ El compilador GCC

### Para desarrollar el programa
+ Al ejecutar el programa, nos debe guardar los resultados en la carpeta output. Un ejemplo de salida lo podemos encontrar en la carpeta docs.
