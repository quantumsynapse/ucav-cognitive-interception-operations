#ifndef MODELO_H
#define MODELO_H

#include <stdio.h>

#define MAX_TRAFEGOS 64
#define MAX_MENSAGENS 64
#define PI 3.14159265358979323846

typedef struct {
    double x;
    double y;
    double z;
} Vetor3;

typedef enum {
    ESTRATEGIA_SEM_COMUNICACAO = 0,
    ESTRATEGIA_MENSAGENS_FIXAS = 1,
    ESTRATEGIA_LINGUAGEM_CONTROLADA = 2,
    ESTRATEGIA_COGNITIVA_SEGURA = 3
} Estrategia;

typedef enum {
    MODO_EVASAO = 0,
    MODO_NEGOCIACAO = 1,
    MODO_CUMPRIMENTO_PARCIAL = 2,
    MODO_CUMPRIMENTO = 3,
    MODO_EMERGENCIA = 4,
    MODO_PERDA_ENLACE = 5
} ModoEvasor;

typedef struct {
    Vetor3 posicao;
    Vetor3 velocidade;
    double autonomia;
    double qualidade_enlace;
    double energia_acumulada;
} Aeronave;

typedef struct {
    Vetor3 posicao;
    Vetor3 velocidade;
    int cooperativo;
} Trafego;

typedef struct {
    double dt;
    double duracao;
    int numero_trafegos;
    int execucoes_monte_carlo;
    unsigned long semente;
    double separacao_pe_horizontal;
    double separacao_pe_vertical;
    double separacao_civil_horizontal;
    double separacao_civil_vertical;
    double distancia_interceptacao;
    double distancia_dialogo;
    double horizonte_predicao;
    double aceleracao_maxima;
    double velocidade_minima;
    double velocidade_maxima;
    double limiar_cumprimento;
    double probabilidade_perda_enlace;
    double atraso_medio_enlace;
} Configuracao;

typedef struct {
    double prob_cumprimento;
    double prob_emergencia;
    double ambiguidade;
    double coerencia_cinematica;
    int mensagens_enviadas;
    int mensagens_recebidas;
    int confirmacoes;
    int identificacao_solicitada;
    int corredor_oferecido;
    int cumprimento_solicitado;
    int resposta_recebida;
    double instante_primeira_mensagem;
    double instante_cumprimento;
    double proximo_instante_mensagem;
} EstadoCognitivo;

typedef struct {
    double separacao_minima_pe_horizontal;
    double separacao_minima_pe_vertical;
    double separacao_minima_civil_p_horizontal;
    double separacao_minima_civil_e_horizontal;
    double margem_minima_pe;
    double margem_minima_civil;
    double tempo_condicao_critica;
    double tempo_ate_dialogo;
    double tempo_ate_cumprimento;
    double tempo_ate_escolta;
    double energia_perseguidor;
    double energia_evasor;
    double ambiguidade_acumulada;
    double qualidade_media_enlace;
    int mensagens;
    int confirmacoes;
    int intervencoes_supervisor_p;
    int intervencoes_supervisor_e;
    int abortagens;
    int perdas_enlace;
    int violacoes_separacao;
    int cumprimento_seguro;
    int escolta_concluida;
    int evasao_persistente;
} Metricas;

typedef struct {
    int total;
    int cumprimentos;
    int escoltas;
    int abortagens;
    int evasoes;
    int cenarios_com_violacao;
    double taxa_cumprimento;
    double taxa_escolta;
    double taxa_violacao;
    double media_tempo_cumprimento;
    double media_margem_pe;
    double media_margem_civil;
    double media_ambiguidade;
    double media_mensagens;
    double media_intervencoes_supervisor;
} ResumoEstrategia;

void configuracao_padrao(Configuracao *cfg);
int carregar_configuracao(const char *caminho, Configuracao *cfg);
const char *nome_estrategia(Estrategia estrategia);

Vetor3 vetor_soma(Vetor3 a, Vetor3 b);
Vetor3 vetor_sub(Vetor3 a, Vetor3 b);
Vetor3 vetor_escala(Vetor3 a, double s);
double vetor_norma_horizontal(Vetor3 a);
double vetor_norma(Vetor3 a);
Vetor3 vetor_unitario_horizontal(Vetor3 a);
Vetor3 limitar_aceleracao(Vetor3 a, double maximo);

void inicializar_cenario(const Configuracao *cfg, unsigned long *estado_rng,
                         Aeronave *p, Aeronave *e, Trafego *trafegos,
                         int *n_trafegos, double *propensao_cumprimento);
void atualizar_trafegos(Trafego *trafegos, int n, double dt);
Vetor3 comando_perseguidor(const Aeronave *p, const Aeronave *e,
                           double distancia_interceptacao,
                           double aceleracao_maxima);
void integrar_aeronave(Aeronave *a, Vetor3 aceleracao, const Configuracao *cfg);

void supervisor_seguranca(const Configuracao *cfg,
                          const Aeronave *propria,
                          const Aeronave *outra,
                          const Trafego *trafegos,
                          int n_trafegos,
                          Vetor3 *aceleracao,
                          int *interveio);

double menor_separacao_civil_horizontal(const Aeronave *a,
                                         const Trafego *trafegos,
                                         int n_trafegos,
                                         double *vertical_correspondente);

void inicializar_cognicao(EstadoCognitivo *cog);
void processar_comunicacao(Estrategia estrategia,
                           const Configuracao *cfg,
                           double t,
                           const Aeronave *p,
                           const Aeronave *e,
                           double menor_sep_civil_e,
                           double propensao_cumprimento,
                           unsigned long *estado_rng,
                           EstadoCognitivo *cog,
                           ModoEvasor *modo,
                           FILE *log_dialogo);
void atualizar_crenca_cinematica(const Aeronave *e,
                                 ModoEvasor modo,
                                 EstadoCognitivo *cog,
                                 double dt);

void inicializar_metricas(Metricas *m);
void atualizar_metricas(const Configuracao *cfg,
                        const Aeronave *p,
                        const Aeronave *e,
                        const Trafego *trafegos,
                        int n_trafegos,
                        const EstadoCognitivo *cog,
                        int interveio_p,
                        int interveio_e,
                        double t,
                        Metricas *m);
void finalizar_metricas(const Configuracao *cfg,
                        ModoEvasor modo,
                        const EstadoCognitivo *cog,
                        Metricas *m);

int simular_cenario(const Configuracao *cfg,
                    Estrategia estrategia,
                    unsigned long semente,
                    Metricas *metricas,
                    const char *trajetoria_csv,
                    const char *dialogo_txt);
int executar_monte_carlo(const Configuracao *cfg,
                         Estrategia estrategia,
                         ResumoEstrategia *resumo);

unsigned long rng_proximo(unsigned long *estado);
double rng_uniforme(unsigned long *estado);
double rng_normal_aprox(unsigned long *estado);

#endif
