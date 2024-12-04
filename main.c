//-----------------------------------------------------------------------------------
//              PTR - Laboratório 4 - Simulação de Sistemas 2 (Non-RT)
//
// Discente : Vinícius Fernandes das Chagas (22050485)
// Docente  : Prof. André Luis Cavalcante
//-----------------------------------------------------------------------------------

// Declarando as bibliotecas
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Incluindo os arquivos de cabeçalho para as funções de cálculo, matriz e simulação
#include "calculo.h"
#include "simulacao.h"
#include "matrix.h"

// Definição de constantes para controlar a simulação
#define INTERVALO 0.03    // Intervalo de tempo entre cálculos (em segundos)
#define TEMPO_INICIAL 0.0 // Tempo inicial da simulação (0s)
#define TEMPO_FINAL 20.0  // Tempo final da simulação (20s)
#define R 0.3             // Raio (parâmetro do sistema)
#define D 0.6             // Diâmetro (parâmetro do sistema)
#define A 3               // Variável de controle do robô (a1 = a2 = 3)

// Variáveis de semáforo para sincronização entre threads
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para acesso exclusivo às matrizes
sem_t semaforo_produtor;                          // Semáforo para sinalizar o produtor
sem_t semaforo_consumidor;                        // Semáforo para sinalizar o consumidor
sem_t semaforo_termino;                           // Semáforo para indicar o término da simulação

// Variáveis globais (compartilhadas entre threads)
Matrix *b; // Matriz B do sistema (derivada de x(t))
Matrix *x; // Matriz X (estado intermediário)
Matrix *u; // Matriz U (entrada do sistema)
Matrix *y; // Matriz Y (saída do sistema)

// Declara os arquivos que serão utilizados
char* arquivo_resultados = "saida.txt";          // Armazena os resultados da simulação
char* arquivo_grafico = "dados_grafico_yt.txt";  // Armazena os valores de y(k) para construção do gráfico

double tempo_termino = 0.0; // Tempo de término da simulação
int flag = 1;               // Flag para controlar o thread de encerramento

// Função responsável pela thread produtor (calcula a entrada e o próximo estado)
void *thread_produtor() {

  for (double t = TEMPO_INICIAL; t <= TEMPO_FINAL; t += INTERVALO) {
    sem_wait(&semaforo_produtor);    // Aguarda espaço para novos dados
    pthread_mutex_lock(&lock);       // Obtém bloqueio exclusivo

    Array uk = calcula_entrada(t);   // Calcula a entrada do sistema no tempo t
    u->dados[0][0] = uk.in[0];       // Atribui o valor de entrada na matriz U
    u->dados[1][0] = uk.in[1];       // Atribui o valor de entrada na matriz U

    *x = calcula_prox_estado(b, u);  // Calcula o próximo estado do sistema

    pthread_mutex_unlock(&lock);     // Libera o bloqueio
    sem_post(&semaforo_consumidor);  // Sinaliza o produtor que a thread produtora está
                                     // pronta para o próximo ciclo
  }

  // Aguarda 40 milissegundos
  usleep(40000);

  // Retorna NULL para indicar o fim da thread
  return NULL;
}

// Função responsável pela thread consumidor (integra e atualiza matrizes, define tempo de término)
void *thread_consumidor() {

  for (double t = TEMPO_INICIAL; t <= TEMPO_FINAL; t += INTERVALO) {
    sem_wait(&semaforo_consumidor); // Aguarda sinal do produtor antes de continuar
    pthread_mutex_lock(&lock);      // Trava o acesso aos dados compartilhados

    integra_x(x);                   // Realiza cálculos na matriz X
    atualiza_b(b, x);               // Atualiza a matriz B com base na matriz X

    *y = calcula_saida(b, x, R);    // Calcula a saída

    tempo_termino = t;              // Armazena o tempo atual para uso potencial

    pthread_mutex_unlock(&lock);    // Destrava o acesso aos dados compartilhados

    // Sinaliza o produtor que a thread consumidora está pronta para o próximo ciclo
    sem_post(&semaforo_produtor);   

    // Salva os resultados obtidos da simulação                                
    salva_resultados(tempo_termino, u, y, arquivo_resultados);
  }

  // Aguarda 50 milissegundos
  usleep(50000);

  // Retorna NULL para indicar o fim da thread
  return NULL;
}

// Função responsável pela thread de encerramento
void *thread_encerramento() {
  while (flag == 1) {
    sem_wait(&semaforo_termino);  // Aguarda sinal do consumidor
    pthread_mutex_lock(&lock);    // Obtém bloqueio exclusivo
    pthread_mutex_unlock(&lock);  // Libera o bloqueio
  }
  return NULL;
}

// Função que inicializa os semáforos e as matrizes utilizadas na simulação
void inicializar() {

  // OBS: O semafaro_produtor será inicializado com 5 para permitir que o produtor avance primeiro

  // Valor inicial 5 indica que o produtor pode executar 5 vezes antes de esperar pelo consumidor
  sem_init(&semaforo_produtor, 0, 5);

  // Valor inicial 0 indica que o consumidor deve esperar inicialmente
  sem_init(&semaforo_consumidor, 0, 0); 

  // Valor inicial 0 (semáforo usado para finalização, detalhes não fornecidos)
  sem_init(&semaforo_termino, 0, 0); 

  // Cria e inicializa as matrizes
  x = cria_matriz(3, 1);
  b = cria_matriz(3, 2);
  u = cria_matriz(2, 1);
  y = cria_matriz(2, 1);

  // Define valores iniciais para a matriz X
  x->dados[0][0] = 0;
  x->dados[1][0] = 0;
  x->dados[2][0] = 0;

  // Define valores iniciais para a matriz B
  b->dados[0][0] = cos(x->dados[2][0]);
  b->dados[0][1] = 0;
  b->dados[1][0] = sin(x->dados[2][0]);
  b->dados[1][1] = 0;
  b->dados[2][0] = 0;
  b->dados[2][1] = 1;
}

// Função principal do programa
int main() {

  // Uso de estruturas para medir tempo de execução
  struct timeval inicio, fim;

  // Inicia as variáveis para as threads
  pthread_t t1, t2, thread_tempo; // Threads para produtor, consumidor e thread de finalização
  pthread_attr_t attr;            // Atributos de thread
  struct sched_param param;       // Parâmetros de escalonamento

  // Criação do menu inicial
  printf("\n---------------------------- Sistema de Robô Móvel ----------------------------\n\n");

  // Marca o tempo de início da execução
  gettimeofday(&inicio, NULL);

  // Configura atributos da thread com política de escalonamento SCHED_RR e prioridade máxima
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_RR);
  param.sched_priority = sched_get_priority_max(SCHED_RR);
  pthread_attr_setschedparam(&attr, &param);

  // Exclui os arquivos antes da simulação para evitar conflitos de dados
  remove(arquivo_resultados);
  remove(arquivo_grafico);

  // Inicializa matrizes
  inicializar();

  // Cria threads
  pthread_create(&thread_tempo, NULL, thread_encerramento, NULL); // Thread para gerenciar a finalização
  pthread_create(&t1, &attr, thread_produtor, NULL);              // Thread produtora
  pthread_create(&t2, &attr, thread_consumidor, NULL);            // Thread consumidora

  // Espera a finalização das threads produtora e consumidora
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  
  // Sinaliza a finalização
  flag = 0; 
  sem_post(&semaforo_termino);
  pthread_join(thread_tempo, NULL);

  // Marca o tempo de finalização da execução
  gettimeofday(&fim, NULL);

  // Calcula o tempo de execução em milissegundos
  long tempo_segundos = fim.tv_sec - inicio.tv_sec;
  long tempo_milissegundos = ((tempo_segundos * 1000000) + fim.tv_usec) - (inicio.tv_usec);

  // Imprime o tempo de execução na tela
  printf("Tempo de Execução: %ld ms\n", tempo_milissegundos / 1000);

  // Destrói semáforos
  sem_destroy(&semaforo_produtor);
  sem_destroy(&semaforo_consumidor);
  sem_destroy(&semaforo_termino);

  // Gera o gráfico de yf(t)
  gera_grafico();

  // Mensagens de verificação que aparecem no final do programa
  printf("\nOBS: As funções u(t) e y(t) foram impressas transpostas\n\n");
  printf("Operações feitas com sucesso!!\n\n");

  // Libera memória alocada para as matrizes
  libera_matriz(b);
  libera_matriz(x);
  libera_matriz(u);
  libera_matriz(y);

  // Retorna 0 para indicar que a execução foi bem-sucedida
  return 0;
}
