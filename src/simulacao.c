#include <stdio.h>
#include <string.h>
#include <math.h>
#include "modelo.h"

static Vetor3 comando_evasor_para_supervisor(const Aeronave *e,
                                              ModoEvasor modo,
                                              double t,
                                              double aceleracao_maxima)
{
    double velocidade_alvo;
    double angulo_alvo;
    Vetor3 alvo;
    Vetor3 erro;

    if (modo == MODO_CUMPRIMENTO) {
        velocidade_alvo = 58.0;
        angulo_alvo = 0.0;
    } else if (modo == MODO_CUMPRIMENTO_PARCIAL) {
        velocidade_alvo = 62.0;
        angulo_alvo = 0.025 * sin(0.02 * t);
    } else {
        velocidade_alvo = 68.0;
        angulo_alvo = 0.09 * sin(0.018 * t) + 0.035 * sin(0.061 * t);
    }
    alvo.x = velocidade_alvo * cos(angulo_alvo);
    alvo.y = velocidade_alvo * sin(angulo_alvo);
    alvo.z = 0.0;
    erro = vetor_sub(alvo, e->velocidade);
    return limitar_aceleracao(vetor_escala(erro, 0.25), aceleracao_maxima);
}

int simular_cenario(const Configuracao *cfg,
                    Estrategia estrategia,
                    unsigned long semente,
                    Metricas *metricas,
                    const char *trajetoria_csv,
                    const char *dialogo_txt)
{
    Aeronave p;
    Aeronave e;
    Trafego trafegos[MAX_TRAFEGOS];
    int n_trafegos;
    unsigned long rng;
    double propensao;
    double t;
    int passos;
    int k;
    int interveio_p;
    int interveio_e;
    double sep_civil_e;
    double vertical;
    Vetor3 a_p;
    Vetor3 a_e;
    EstadoCognitivo cog;
    ModoEvasor modo;
    FILE *traj;
    FILE *dialogo;

    rng = semente;
    inicializar_cenario(cfg, &rng, &p, &e, trafegos, &n_trafegos, &propensao);
    inicializar_cognicao(&cog);
    if (estrategia == ESTRATEGIA_SEM_COMUNICACAO) cog.ambiguidade = 0.0;
    inicializar_metricas(metricas);
    modo = MODO_EVASAO;
    traj = NULL;
    dialogo = NULL;

    if (trajetoria_csv != NULL) {
        traj = fopen(trajetoria_csv, "w");
        if (traj == NULL) return 0;
        fprintf(traj, "tempo,p_x,p_y,p_z,e_x,e_y,e_z,sep_pe,sep_civil_e,vert_civil_e,prob_cumprimento,ambiguidade,modo_evasor\n");
    }
    if (dialogo_txt != NULL) {
        dialogo = fopen(dialogo_txt, "w");
        if (dialogo == NULL) {
            if (traj != NULL) fclose(traj);
            return 0;
        }
    }

    passos = (int)(cfg->duracao / cfg->dt + 0.5);
    for (k = 0; k <= passos; ++k) {
        t = k * cfg->dt;
        sep_civil_e = menor_separacao_civil_horizontal(&e, trafegos, n_trafegos, &vertical);

        processar_comunicacao(estrategia, cfg, t, &p, &e,
                              sep_civil_e, propensao, &rng, &cog,
                              &modo, dialogo);
        atualizar_crenca_cinematica(&e, modo, &cog, cfg->dt);

        a_p = comando_perseguidor(&p, &e,
                                  cfg->distancia_interceptacao,
                                  cfg->aceleracao_maxima);
        a_e = comando_evasor_para_supervisor(&e, modo, t,
                                             cfg->aceleracao_maxima);

        supervisor_seguranca(cfg, &p, &e, trafegos, n_trafegos,
                             &a_p, &interveio_p);
        supervisor_seguranca(cfg, &e, &p, trafegos, n_trafegos,
                             &a_e, &interveio_e);

        integrar_aeronave(&p, a_p, cfg);
        integrar_aeronave(&e, a_e, cfg);
        atualizar_trafegos(trafegos, n_trafegos, cfg->dt);

        atualizar_metricas(cfg, &p, &e, trafegos, n_trafegos,
                           &cog, interveio_p, interveio_e, t, metricas);

        if (metricas->margem_minima_civil < 0.75 || p.autonomia < 0.15) {
            metricas->abortagens = 1;
            break;
        }

        if (traj != NULL && (k % 2 == 0)) {
            sep_civil_e = menor_separacao_civil_horizontal(&e, trafegos, n_trafegos, &vertical);
            fprintf(traj, "%.1f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.6f,%.6f,%d\n",
                    t,
                    p.posicao.x, p.posicao.y, p.posicao.z,
                    e.posicao.x, e.posicao.y, e.posicao.z,
                    vetor_norma_horizontal(vetor_sub(p.posicao, e.posicao)),
                    sep_civil_e, vertical,
                    cog.prob_cumprimento, cog.ambiguidade, (int)modo);
        }
    }

    metricas->energia_perseguidor = p.energia_acumulada;
    metricas->energia_evasor = e.energia_acumulada;
    finalizar_metricas(cfg, modo, &cog, metricas);

    if (traj != NULL) fclose(traj);
    if (dialogo != NULL) fclose(dialogo);
    return 1;
}

int executar_monte_carlo(const Configuracao *cfg,
                         Estrategia estrategia,
                         ResumoEstrategia *resumo)
{
    int i;
    Metricas m;
    double soma_tempo;
    double soma_margem_pe;
    double soma_margem_civil;
    double soma_amb;
    double soma_msg;
    double soma_sup;
    int n_tempo;

    memset(resumo, 0, sizeof(*resumo));
    soma_tempo = 0.0;
    soma_margem_pe = 0.0;
    soma_margem_civil = 0.0;
    soma_amb = 0.0;
    soma_msg = 0.0;
    soma_sup = 0.0;
    n_tempo = 0;

    for (i = 0; i < cfg->execucoes_monte_carlo; ++i) {
        if (!simular_cenario(cfg, estrategia,
                             cfg->semente + (unsigned long)(7919 * i + 101 * (int)estrategia),
                             &m, NULL, NULL)) return 0;
        resumo->total++;
        resumo->cumprimentos += m.cumprimento_seguro;
        resumo->escoltas += m.escolta_concluida;
        resumo->abortagens += m.abortagens;
        resumo->evasoes += m.evasao_persistente;
        if (m.violacoes_separacao > 0) resumo->cenarios_com_violacao++;
        if (m.tempo_ate_cumprimento >= 0.0) {
            soma_tempo += m.tempo_ate_cumprimento;
            n_tempo++;
        }
        soma_margem_pe += m.margem_minima_pe;
        soma_margem_civil += m.margem_minima_civil;
        soma_amb += m.ambiguidade_acumulada / cfg->duracao;
        soma_msg += m.mensagens;
        soma_sup += m.intervencoes_supervisor_p + m.intervencoes_supervisor_e;
    }

    resumo->taxa_cumprimento = (double)resumo->cumprimentos / resumo->total;
    resumo->taxa_escolta = (double)resumo->escoltas / resumo->total;
    resumo->taxa_violacao = (double)resumo->cenarios_com_violacao / resumo->total;
    resumo->media_tempo_cumprimento = n_tempo > 0 ? soma_tempo / n_tempo : -1.0;
    resumo->media_margem_pe = soma_margem_pe / resumo->total;
    resumo->media_margem_civil = soma_margem_civil / resumo->total;
    resumo->media_ambiguidade = soma_amb / resumo->total;
    resumo->media_mensagens = soma_msg / resumo->total;
    resumo->media_intervencoes_supervisor = soma_sup / resumo->total;
    return 1;
}
