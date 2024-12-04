#include <math.h>
#include <stdio.h>

#include "calculo.h"

#define PI 3.14159265358979323846

// Função para calcular a entrada do robô no instante de tempo t
Array calcula_entrada(double t) {
  Array u; // Declara a variável u do tipo Array

  // Define a entrada de acordo com o valor de t
  if (t < 0) {
    u.in[0] = 0;
    u.in[1] = 0;
  } else if (t > 0 && t < 10) {
    u.in[0] = 1.0;
    u.in[1] = 0.2 * PI;
  } else if (t >= 10) {
    u.in[0] = 1.0;
    u.in[1] = -0.2 * PI;
  }

  // Retorna a variável u contendo a entrada calculada
  return u;
}

// Função para calcular o próximo estado do robô, retornando o produto das matrizes B e U
Matrix calcula_prox_estado(Matrix *b, Matrix *u) {
  return *produto_matrizes(b, u);
}

// Função para integrar a matriz X
void integra_x(Matrix *x) {
  // Atualiza os valores da matriz X utilizando seno e cosseno
  x->dados[0][0] = -1 * sin(x->dados[2][0]);
  x->dados[1][0] = cos(x->dados[2][0]);
  x->dados[2][0] = 0;
}

// Função para atualizar a matriz B
void atualiza_b(Matrix *b, Matrix *x) {
  // Atualiza os valores da matriz B utilizando seno e cosseno de um elemento da matriz X
  b->dados[0][0] = cos(x->dados[2][0]);
  b->dados[0][1] = 0;
  b->dados[1][0] = sin(x->dados[2][0]);
  b->dados[1][1] = 0;
  b->dados[2][0] = 0;
  b->dados[2][1] = 1;
}

// Função para calcular a saída do robô, retornando a soma da matriz X e o produto da matriz C com a matriz X
Matrix calcula_saida(Matrix *b, Matrix *x, double r) {
  // Criando as matrizes auxiliares de y(t)
  Matrix *aux_y1, *aux_y2; 
  aux_y1 = cria_matriz(2, 1);
  aux_y2 = cria_matriz(2, 3);

  // Atualiza os valores da matriz aux_y1
  aux_y1->dados[0][0] = r * cos(x->dados[2][0]);
  aux_y1->dados[1][0] = r * sin(x->dados[2][0]);

  // Atualiza os valores da matriz aux_y2
  aux_y2->dados[0][0] = 1;
  aux_y2->dados[0][1] = 0;
  aux_y2->dados[0][2] = 0;
  aux_y2->dados[1][0] = 0;
  aux_y2->dados[1][1] = 1;
  aux_y2->dados[1][2] = 0;

  // Faz o cálculo de y(t)
  Matrix *y = soma_matrizes(produto_matrizes(aux_y2, x), aux_y1);

  // Libera memória alocada para as matrizes auxiliares
  libera_matriz(aux_y1);
  libera_matriz(aux_y2);

  // Retorna a saída y(t)
  return *y;
}
