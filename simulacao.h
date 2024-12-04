// Criando a interface para a simulação
#ifndef SIMULACAO_H
#define SIMULACAO_H

#include "matrix.h"
#include <stdio.h>
#include <pthread.h>

typedef struct {
    double media;
    double var;
    double desv_pad;
    double min;
    double max;
    double q1;
    double q2;
    double q3;
} Dados;

// Função que salva o resultado da simulação no formato CSV
void salva_resultados(double t, Matrix *u, Matrix *y, const char *nome_arquivo);

// Função que gera o gráfico de y(t)
void gera_grafico();

// Função que realiza a leitura dos dados obtidos na simulação
void leitura_dados(char *nome_arquivo, double *u_00, double *u_01, double *y_00, double *y_01, int tam);

// Função que processa os dados, retornando os valores de média, variância, desvio padrão e valores máximos e mínimos
Dados processa_dados(double *dados, size_t tam);

// Função que utiliza as outras funções e escreve os dados finais em uma tabela
void salva_tabela_dados(int tam, char *nome_arquivo);

#endif