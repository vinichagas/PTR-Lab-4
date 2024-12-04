// Criando a interface "calculo.h" para o cálculo do sistema de controle a ser feito
#ifndef CALCULO_H
#define CALCULO_H

#include "matrix.h"

// Estrutura utilizada para calcular o valor da entrada u(t)
typedef struct array {
    double in[2];
}Array;

// Função para calcular a entrada do robô no instante de tempo t
Array calcula_entrada(double t);

// Função para calcular o próximo estado do robô
Matrix calcula_prox_estado(Matrix *b, Matrix *u);

// Função para integrar a matriz X
void integra_x(Matrix *x);

// Função para atualizar a matriz B
void atualiza_b(Matrix *b, Matrix *x);

// Função para calcular a saída do robô
Matrix calcula_saida(Matrix *b, Matrix *x, double r);

#endif