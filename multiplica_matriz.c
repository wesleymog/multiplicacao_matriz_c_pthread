
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"
float **mat, **mat2, **saida;
int nthreads;

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int id, dim;
} t_Args;

double ** aloca_matriz( int tam )
{
  double * valores = (double *) malloc( tam * tam * sizeof(double) );

  double ** ponteiros = (double **) malloc( tam * sizeof(double*) );

  int i;
  for (i = 0; i < tam; ++i) {
    ponteiros[ i ] = &valores[ i * tam ];
  }

  return ponteiros;
}
void * tarefa (void *arg) {
  t_Args *args = (t_Args *) arg;
  int part, start, end;
  part = args->dim / nthreads;
  start = args->id * part;
  end = (args->id+1) * part;
  for(int linha=start; linha<end; linha++)
      for(int coluna=0; coluna<args->dim; coluna++){
          int somaprod=0;
          for(int i=0; i<args->dim; i++) {
              somaprod+=mat[ linha ][ i ] * mat2[ i ][ coluna ];
            }

          saida[linha][ coluna ] = somaprod;
      }
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[] ) {

  int dim;
  pthread_t *tid;
  t_Args *args;
  double inicio, fim, delta;

  if(argc<3){
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
  }
  // (a) Inicialização da estrutura de dados
  GET_TIME(inicio);
  dim = atoi(argv[1]);
  nthreads = atoi(argv[2]);
  if (nthreads > dim) nthreads = dim;
  
  mat = aloca_matriz(dim);
  mat2 = aloca_matriz(dim);
  saida = aloca_matriz(dim);


  for(int i = 0; i<dim; i++){
      for (int j = 0; j<dim; j++){
        mat[i][j] = 1;
        mat2[i][j] = 1;
        saida[i][j] = 1;
  }
  }
  tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  if(tid ==NULL){puts("ERRO MALLOC");return 2;}
  args = (t_Args*) malloc(sizeof(t_Args)*nthreads);
  if(args ==NULL){puts("ERRO MALLOC");return 2;}
  GET_TIME(fim);
  delta = fim - inicio;
  printf("tempo de Inicialização da estrutura de dados %lf\n", delta);


  GET_TIME(inicio);
  for(int i=0;i<nthreads; i++){
      (args+i)->id = i;
      (args+i)->dim = dim;
    if(pthread_create(tid+i, NULL, tarefa, (void*)(args+i))){
        puts("ERRO --pthread create");
        return 3;
    }
  }

  //--espera todas as threads terminarem
  for (int thread=0; thread<nthreads; thread++) {
    if (pthread_join(tid[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }
  
  GET_TIME(fim);
  delta = fim - inicio;
  printf("tempo de criação das threads, execução da multiplicação e termino de threads %lf\n", delta);

  GET_TIME(inicio);
  /*
  for(int i = 0; i<dim; i++){
    for (int j = 0; j<dim; j++){
      printf(" %lf ",saida[i][j]);
    }
    printf("\n");
  }
  */
  free(mat);
  free(mat2);
  free(saida);
  free(args);
  free(tid);
  
  GET_TIME(fim);
  delta = fim - inicio;
  printf("tempo de finalização do programa %lf\n", delta);

  pthread_exit(NULL);

}
