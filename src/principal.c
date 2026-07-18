#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modelo.h"

static void imprimir_resumo(Estrategia e, const ResumoEstrategia *r)
{
    printf("estrategia=%s ", nome_estrategia(e));
    printf("taxa_cumprimento=%.3f ", r->taxa_cumprimento);
    printf("taxa_escolta=%.3f ", r->taxa_escolta);
    printf("taxa_violacao=%.3f ", r->taxa_violacao);
    printf("tempo_medio_cumprimento=%.2f ", r->media_tempo_cumprimento);
    printf("margem_media_pe=%.3f ", r->media_margem_pe);
    printf("margem_media_civil=%.3f ", r->media_margem_civil);
    printf("ambiguidade_media=%.4f ", r->media_ambiguidade);
    printf("mensagens_media=%.2f ", r->media_mensagens);
    printf("intervencoes_supervisor_media=%.2f\n",
           r->media_intervencoes_supervisor);
}

int main(int argc, char **argv)
{
    Configuracao cfg;
    ResumoEstrategia resumo;
    Metricas m;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s dados/configuracao.txt [--referencia]\n", argv[0]);
        return 1;
    }
    if (!carregar_configuracao(argv[1], &cfg)) {
        fprintf(stderr, "Falha ao carregar configuração.\n");
        return 1;
    }

    if (argc >= 3 && strcmp(argv[2], "--referencia") == 0) {
        if (!simular_cenario(&cfg, ESTRATEGIA_COGNITIVA_SEGURA,
                             cfg.semente, &m,
                             "resultados/trajetoria_referencia.csv",
                             "resultados/dialogo_referencia.txt")) {
            fprintf(stderr, "Falha na simulação de referência.\n");
            return 1;
        }
        printf("cumprimento_seguro=%d escolta_concluida=%d ",
               m.cumprimento_seguro, m.escolta_concluida);
        printf("margem_min_pe=%.3f margem_min_civil=%.3f ",
               m.margem_minima_pe, m.margem_minima_civil);
        printf("tempo_cumprimento=%.2f mensagens=%d violacoes=%d\n",
               m.tempo_ate_cumprimento, m.mensagens,
               m.violacoes_separacao);
        return 0;
    }

    printf("estrategia,taxa_cumprimento,taxa_escolta,taxa_violacao,tempo_medio_cumprimento,margem_media_pe,margem_media_civil,ambiguidade_media,mensagens_media,intervencoes_supervisor_media\n");
    for (i = 0; i < 4; ++i) {
        if (!executar_monte_carlo(&cfg, (Estrategia)i, &resumo)) {
            fprintf(stderr, "Falha no Monte Carlo.\n");
            return 1;
        }
        imprimir_resumo((Estrategia)i, &resumo);
    }
    return 0;
}
