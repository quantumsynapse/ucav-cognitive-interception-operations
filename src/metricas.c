#include <math.h>
#include "modelo.h"

void inicializar_metricas(Metricas *m)
{
    m->separacao_minima_pe_horizontal = 1.0e30;
    m->separacao_minima_pe_vertical = 1.0e30;
    m->separacao_minima_civil_p_horizontal = 1.0e30;
    m->separacao_minima_civil_e_horizontal = 1.0e30;
    m->margem_minima_pe = 1.0e30;
    m->margem_minima_civil = 1.0e30;
    m->tempo_condicao_critica = 0.0;
    m->tempo_ate_dialogo = -1.0;
    m->tempo_ate_cumprimento = -1.0;
    m->tempo_ate_escolta = -1.0;
    m->energia_perseguidor = 0.0;
    m->energia_evasor = 0.0;
    m->ambiguidade_acumulada = 0.0;
    m->qualidade_media_enlace = 0.0;
    m->mensagens = 0;
    m->confirmacoes = 0;
    m->intervencoes_supervisor_p = 0;
    m->intervencoes_supervisor_e = 0;
    m->abortagens = 0;
    m->perdas_enlace = 0;
    m->violacoes_separacao = 0;
    m->cumprimento_seguro = 0;
    m->escolta_concluida = 0;
    m->evasao_persistente = 0;
}

void atualizar_metricas(const Configuracao *cfg,
                        const Aeronave *p,
                        const Aeronave *e,
                        const Trafego *trafegos,
                        int n_trafegos,
                        const EstadoCognitivo *cog,
                        int interveio_p,
                        int interveio_e,
                        double t,
                        Metricas *m)
{
    Vetor3 delta;
    double dh;
    double dv;
    double vert_p;
    double vert_e;
    double civ_p;
    double civ_e;
    double margem_pe;
    double margem_civ_p;
    double margem_civ_e;

    delta = vetor_sub(p->posicao, e->posicao);
    dh = vetor_norma_horizontal(delta);
    dv = fabs(delta.z);
    civ_p = menor_separacao_civil_horizontal(p, trafegos, n_trafegos, &vert_p);
    if (civ_p < m->separacao_minima_civil_p_horizontal)
        m->separacao_minima_civil_p_horizontal = civ_p;
    civ_e = menor_separacao_civil_horizontal(e, trafegos, n_trafegos, &vert_e);
    if (civ_e < m->separacao_minima_civil_e_horizontal)
        m->separacao_minima_civil_e_horizontal = civ_e;

    if (dh < m->separacao_minima_pe_horizontal) m->separacao_minima_pe_horizontal = dh;
    if (dv < m->separacao_minima_pe_vertical) m->separacao_minima_pe_vertical = dv;

    margem_pe = dh / cfg->separacao_pe_horizontal;
    if (dv / cfg->separacao_pe_vertical > margem_pe)
        margem_pe = dv / cfg->separacao_pe_vertical;
    margem_civ_p = civ_p / cfg->separacao_civil_horizontal;
    if (vert_p / cfg->separacao_civil_vertical > margem_civ_p)
        margem_civ_p = vert_p / cfg->separacao_civil_vertical;
    margem_civ_e = civ_e / cfg->separacao_civil_horizontal;
    if (vert_e / cfg->separacao_civil_vertical > margem_civ_e)
        margem_civ_e = vert_e / cfg->separacao_civil_vertical;
    if (margem_pe < m->margem_minima_pe) m->margem_minima_pe = margem_pe;
    if (margem_civ_p < m->margem_minima_civil) m->margem_minima_civil = margem_civ_p;
    if (margem_civ_e < m->margem_minima_civil) m->margem_minima_civil = margem_civ_e;

    if ((dh < 1.25 * cfg->separacao_pe_horizontal &&
         dv < 1.25 * cfg->separacao_pe_vertical) ||
        (civ_p < 1.25 * cfg->separacao_civil_horizontal &&
         vert_p < 1.25 * cfg->separacao_civil_vertical) ||
        (civ_e < 1.25 * cfg->separacao_civil_horizontal &&
         vert_e < 1.25 * cfg->separacao_civil_vertical)) {
        m->tempo_condicao_critica += cfg->dt;
    }

    if (dh < cfg->separacao_pe_horizontal && dv < cfg->separacao_pe_vertical)
        m->violacoes_separacao++;
    if ((civ_p < cfg->separacao_civil_horizontal &&
         vert_p < cfg->separacao_civil_vertical) ||
        (civ_e < cfg->separacao_civil_horizontal &&
         vert_e < cfg->separacao_civil_vertical))
        m->violacoes_separacao++;

    if (interveio_p) m->intervencoes_supervisor_p++;
    if (interveio_e) m->intervencoes_supervisor_e++;
    m->ambiguidade_acumulada += cog->ambiguidade * cfg->dt;
    m->qualidade_media_enlace += 0.5 * (p->qualidade_enlace + e->qualidade_enlace) * cfg->dt;

    if (m->tempo_ate_dialogo < 0.0 && cog->instante_primeira_mensagem >= 0.0)
        m->tempo_ate_dialogo = cog->instante_primeira_mensagem;
    if (m->tempo_ate_cumprimento < 0.0 && cog->instante_cumprimento >= 0.0)
        m->tempo_ate_cumprimento = cog->instante_cumprimento;

    (void)t;
}

void finalizar_metricas(const Configuracao *cfg,
                        ModoEvasor modo,
                        const EstadoCognitivo *cog,
                        Metricas *m)
{
    m->mensagens = cog->mensagens_enviadas + cog->mensagens_recebidas;
    m->confirmacoes = cog->confirmacoes;
    m->energia_perseguidor = m->energia_perseguidor;
    m->energia_evasor = m->energia_evasor;
    m->qualidade_media_enlace /= cfg->duracao;

    if (modo == MODO_CUMPRIMENTO && m->violacoes_separacao == 0) {
        m->cumprimento_seguro = 1;
        if (m->tempo_ate_cumprimento >= 0.0 &&
            cfg->duracao - m->tempo_ate_cumprimento >= 120.0) {
            m->escolta_concluida = 1;
            m->tempo_ate_escolta = m->tempo_ate_cumprimento + 120.0;
        }
    } else {
        m->evasao_persistente = 1;
    }
}
