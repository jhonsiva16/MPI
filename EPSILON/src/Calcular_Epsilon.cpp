
#include <math.h>
#include "mpi.h"   // Biblioteca de MPI
#include <cstdlib> // Incluido para el uso de atoi
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int k, // Numero de sumas (exactitud)
        rank, // Identificador de proceso
        size; // Numero de procesos

    double EPSILON = 2.718281828459045235360; // valor referencial de epsilon
    double mye, // Valor local de e
        e;   // Valor global de e

    double fact; //Valor del factorial

    MPI_Init(&argc, &argv); // Inicializamos los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el numero total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

    // Solo el proceso 0 va a conocer el numero de iteraciones que vamos a
    // ejecutar para la aproximacion de e
    if (rank == 0) {
        cout<<"introduce la precision del calculo (0 < k < size): ";
        cin>>k;
    }

    // El proceso 0 reparte al resto de procesos el numero de iteraciones
    // que calcularemos para la aproximacion de PI
    MPI_Bcast(&k, // Puntero al dato que vamos a enviar
              1,  // Numero de datos a los que apunta el puntero
              MPI_INT, // Tipo del dato a enviar
              0, // Identificacion del proceso que envia el dato
              MPI_COMM_WORLD);

    if (k <= 0 || k >= size){ //si los valores de k estan por debajo de 0 y encima de la
        MPI_Finalize();       // cantidad de procesos, finalizara la ejecucion
        exit(0);
    }else {
        // Calculo de EPSILON
        fact= 1.0; //valor inicial del factorial

        for (int i = rank; i <= k; i += size) { //calculos fracciones en cada proceso

            for (int j = 1; j <= i; j ++){  //calculo del factorial correspondiente al proceso
            fact *= j ;
            }
            //printf("factorial: %f \n",fact);
            mye =  1 / fact; //formula de la fraccion individual

            //printf("primer valor: %f \n",mye);
        }

        // Todos los procesos ahora comparten su valor local de Epsilon,
        // lo hacen reduciendo su valor local a un proceso
        // seleccionada a traves de una operacion aritmetica.
        MPI_Reduce(&mye, // Valor de cada fraccion 1/k!
                    &e,  // Dato sobre el que vamos a reducir el resto
                    1,      // Numero de datos que vamos a reducir
                    MPI_DOUBLE,  // Tipo de dato que vamos a reducir
                    MPI_SUM,  // Operacion que aplicaremos
                    0, // proceso que va a recibir el dato reducido
                    MPI_COMM_WORLD);

        // Solo el proceso 0 imprime el mensaje, ya que es la unica que
        // conoce el valor de Epsilon aproximado.
        if (rank == 0)
        {
            printf("EPSILON: %.30f \n", EPSILON);
            printf("Mi EPSILON: %.30f \n \n", e);
            printf("El valor aproximado de Mi EPSILON es: %f, con un error de %.60f \n", e,fabs(e - EPSILON));
        }
    }

    // Terminamos la ejecucion de los procesos, despues de esto solo existira
    // el proceso 0
    // ¡Ojo! Esto no significa que los demas procesos no ejecuten el resto
    // de codigo despues de "Finalize", es conveniente asegurarnos con una
    // condicion si vamos a ejecutar mas codigo (Por ejemplo, con "if(rank==0)".
    MPI_Finalize();
    return 0;
}


