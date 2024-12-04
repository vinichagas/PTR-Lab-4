#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "simulacao.h"
#include "matrix.h"
#include "calculo.h"

#define PI 3.14159265358979323846

// Função que salva os resultados da simlação em um arquivo de texto
void salva_resultados(double t, Matrix *u, Matrix *y, const char *nome_arquivo) {

  // Abre o arquivo selecionado para anexação
  FILE *arquivo;
  FILE* dados_grafico_yt;

  // Se o arquivo for aberto com sucesso, escreve o estado atual da matriz
  if ((arquivo = fopen(nome_arquivo, "a")) != NULL) {

    // Imprime o tempo atual
    fprintf(arquivo, "t = %.2lfs\t", t);

    // Imprime u no tempo atual
    fprintf(arquivo, "u(t) = [%.2lf, %.2lf]\t ", u->dados[0][0], u->dados[1][0]);

    // Imprime y no tempo atual    
    fprintf(arquivo, "y(t) = [%.2lf, %.2lf]\n", y->dados[0][0], y->dados[1][0]); 
  }

  // Fecha o arquivo
  fclose(arquivo);


  // Se o arquivo responsável por plotar o gráfico for aberto com sucesso, escreve os valores de y(t)
  if ((dados_grafico_yt = fopen("dados_grafico_yt.txt", "a")) != NULL) {
    fprintf(dados_grafico_yt, "%.2lf %.2lf\n", y->dados[0][0], y->dados[1][0]);
  }

  fclose(dados_grafico_yt);
}

// Função que gera o gráfico de y(t)
void gera_grafico() {
  char comando[512];

  // Verifica se o arquivo existe
  FILE *fp = fopen("dados_grafico_yt.txt", "r");
  if (fp == NULL) {
      fprintf(stderr, "Erro ao abrir o arquivo: %s\n", "dados_grafico_yt.txt");
      return;
  }
  fclose(fp);

  // Comando gnuplot
  snprintf(comando, sizeof(comando), "gnuplot -persist << EOF\n"
                                       "set terminal pngcairo size 800,600\n"
                                       "set output '%s'\n"
                                       "set title '%s'\n"
                                       "set xlabel 'Eixo X'\n"
                                       "set ylabel 'Eixo Y'\n"
                                       "set grid\n"
                                       "set style line 1 pt 7 ps 0.05 lc rgb 'blue'\n"
                                       "plot '%s' using 1:2 with lines title 'Dados'\n"
                                       "unset output\n"
                                       "EOF",
                                       "grafico_yt.png",
                                       "Gráfico de y(t)",
                                       "dados_grafico_yt.txt");

  // Execua o comando gnuplot
  system(comando);

  // Abre o gráfico com feh
  snprintf(comando, sizeof(comando), "feh '%s.png' &", "grafico_yt");
  system(comando);
}

// Função que realiza a leitura dos dados obtidos na simulação
void leitura_dados(char *nome_arquivo, double *u_00, double *u_01, double *y_00, double *y_01, int tam) {
  FILE *arquivo;

  if((arquivo = fopen(nome_arquivo, "r")) != NULL) {
    
    int tempo, i = 0;
    char buffer[60];

    while (fgets(buffer, sizeof(buffer), arquivo)) {
      sscanf(buffer, "t = %d  u(t) = [%lf, %lf] 	y(t) = [%lf, %lf]", &tempo, &u_00[i], &u_01[i], &y_00[i], &y_01[i]);
      i++;
    }
  }

  fclose(arquivo);
}

// Função auxiliar para ordenar tipo double
int qsort_double(const void *a, const void *b) {
    return (*(double*)a - *(double*)b > 0) - (*(double*)a - *(double*)b < 0);
}

// Função que processa os dados, retornando os valores de média, variância, desvio padrão e valores máximos e mínimos
Dados processa_dados(double *dados, size_t tam) {
  Dados data = {0};

  qsort(dados, tam, sizeof(double), qsort_double);

  for(int i = 0; i < tam; i++){
    data.media += dados[i];
  }

  data.media /= tam;

  for(int i = 0; i < tam; i++){
    data.var += (dados[i] - data.media) * (dados[i] - data.media);
  }

  data.var /= tam;
  data.min = dados[0];
  data.max = dados[tam-1];
  data.desv_pad = sqrt(data.var);
  data.q1 = dados[(int)(tam * 0.25)];
  data.q2 = dados[(int)(tam * 0.5)];
  data.q3 = dados[(int)(tam * 0.75)];

  return data;
}

// Função que uiliza as ouras funções e escreve os dados finais em uma tabela
void salva_tabela_dados(int tam, char *nome_arquivo) {
  //double u_00[tam], u_01[tam], y_00[tam], y_01[tam];

  //leitura_dados(nome_arquivo, u_00, u_01, y_00, y_01, tam);

  //COMPLETAR 
}
