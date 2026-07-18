#include <stdio.h>
#include <string.h>
#include "modelo.h"

void configuracao_padrao(Configuracao *cfg)
{
    cfg->dt = 0.5;
    cfg->duracao = 900.0;
    cfg->numero_trafegos = 24;
    cfg->execucoes_monte_carlo = 200;
    cfg->semente = 20260718UL;
    cfg->separacao_pe_horizontal = 500.0;
    cfg->separacao_pe_vertical = 150.0;
    cfg->separacao_civil_horizontal = 1500.0;
    cfg->separacao_civil_vertical = 300.0;
    cfg->distancia_interceptacao = 1200.0;
    cfg->distancia_dialogo = 5000.0;
    cfg->horizonte_predicao = 20.0;
    cfg->aceleracao_maxima = 2.0;
    cfg->velocidade_minima = 45.0;
    cfg->velocidade_maxima = 95.0;
    cfg->limiar_cumprimento = 0.70;
    cfg->probabilidade_perda_enlace = 0.015;
    cfg->atraso_medio_enlace = 1.5;
}

static void atribuir(Configuracao *cfg, const char *chave, double valor)
{
    if (strcmp(chave, "dt") == 0) cfg->dt = valor;
    else if (strcmp(chave, "duracao") == 0) cfg->duracao = valor;
    else if (strcmp(chave, "numero_trafegos") == 0) cfg->numero_trafegos = (int)valor;
    else if (strcmp(chave, "execucoes_monte_carlo") == 0) cfg->execucoes_monte_carlo = (int)valor;
    else if (strcmp(chave, "semente") == 0) cfg->semente = (unsigned long)valor;
    else if (strcmp(chave, "separacao_pe_horizontal") == 0) cfg->separacao_pe_horizontal = valor;
    else if (strcmp(chave, "separacao_pe_vertical") == 0) cfg->separacao_pe_vertical = valor;
    else if (strcmp(chave, "separacao_civil_horizontal") == 0) cfg->separacao_civil_horizontal = valor;
    else if (strcmp(chave, "separacao_civil_vertical") == 0) cfg->separacao_civil_vertical = valor;
    else if (strcmp(chave, "distancia_interceptacao") == 0) cfg->distancia_interceptacao = valor;
    else if (strcmp(chave, "distancia_dialogo") == 0) cfg->distancia_dialogo = valor;
    else if (strcmp(chave, "horizonte_predicao") == 0) cfg->horizonte_predicao = valor;
    else if (strcmp(chave, "aceleracao_maxima") == 0) cfg->aceleracao_maxima = valor;
    else if (strcmp(chave, "velocidade_minima") == 0) cfg->velocidade_minima = valor;
    else if (strcmp(chave, "velocidade_maxima") == 0) cfg->velocidade_maxima = valor;
    else if (strcmp(chave, "limiar_cumprimento") == 0) cfg->limiar_cumprimento = valor;
    else if (strcmp(chave, "probabilidade_perda_enlace") == 0) cfg->probabilidade_perda_enlace = valor;
    else if (strcmp(chave, "atraso_medio_enlace") == 0) cfg->atraso_medio_enlace = valor;
}

int carregar_configuracao(const char *caminho, Configuracao *cfg)
{
    FILE *f;
    char linha[256];
    char chave[128];
    double valor;

    configuracao_padrao(cfg);
    f = fopen(caminho, "r");
    if (f == NULL) return 0;

    while (fgets(linha, sizeof(linha), f) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n') continue;
        if (sscanf(linha, "%127[^=]=%lf", chave, &valor) == 2) {
            atribuir(cfg, chave, valor);
        }
    }
    fclose(f);

    if (cfg->numero_trafegos < 1) cfg->numero_trafegos = 1;
    if (cfg->numero_trafegos > MAX_TRAFEGOS) cfg->numero_trafegos = MAX_TRAFEGOS;
    if (cfg->dt <= 0.0 || cfg->duracao <= 0.0) return 0;
    return 1;
}

const char *nome_estrategia(Estrategia estrategia)
{
    switch (estrategia) {
        case ESTRATEGIA_SEM_COMUNICACAO: return "sem_comunicacao";
        case ESTRATEGIA_MENSAGENS_FIXAS: return "mensagens_fixas";
        case ESTRATEGIA_LINGUAGEM_CONTROLADA: return "linguagem_controlada";
        case ESTRATEGIA_COGNITIVA_SEGURA: return "cognitiva_segura";
        default: return "desconhecida";
    }
}
