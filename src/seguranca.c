#include <math.h>
#include "modelo.h"

static double absd(double x)
{
    return x < 0.0 ? -x : x;
}

static double limitar(double x, double minimo, double maximo)
{
    if (x < minimo) return minimo;
    if (x > maximo) return maximo;
    return x;
}

static void aplicar_desvio(const Vetor3 *r_cpa,
                           double limite_h,
                           double limite_v,
                           Vetor3 *aceleracao,
                           int *interveio)
{
    double dh;
    double dv;
    double fator_h;
    double fator_v;
    Vetor3 afastamento;

    dh = vetor_norma_horizontal(*r_cpa);
    dv = absd(r_cpa->z);
    if (dh >= limite_h || dv >= limite_v) return;

    afastamento = vetor_unitario_horizontal(*r_cpa);
    fator_h = limitar((limite_h - dh) / limite_h, 0.0, 1.0);
    fator_v = limitar((limite_v - dv) / limite_v, 0.0, 1.0);

    aceleracao->x += afastamento.x * (0.4 + 0.8 * fator_h);
    aceleracao->y += afastamento.y * (0.4 + 0.8 * fator_h);

    /* A separação vertical recebe prioridade para resolver o conflito. */
    if (r_cpa->z >= 0.0) aceleracao->z = 3.0;
    else aceleracao->z = -3.0;
    (void)fator_v;

    *interveio = 1;
}

static Vetor3 cpa_relativo(Vetor3 r, Vetor3 v_rel, double horizonte)
{
    double vv;
    double t_cpa;
    vv = v_rel.x * v_rel.x + v_rel.y * v_rel.y;
    if (vv < 1.0e-9) t_cpa = 0.0;
    else t_cpa = -(r.x * v_rel.x + r.y * v_rel.y) / vv;
    t_cpa = limitar(t_cpa, 0.0, horizonte);
    return vetor_soma(r, vetor_escala(v_rel, t_cpa));
}

void supervisor_seguranca(const Configuracao *cfg,
                          const Aeronave *propria,
                          const Aeronave *outra,
                          const Trafego *trafegos,
                          int n_trafegos,
                          Vetor3 *aceleracao,
                          int *interveio)
{
    int i;
    Vetor3 vel_proposta;
    Vetor3 r;
    Vetor3 v_rel;
    Vetor3 r_cpa;

    *interveio = 0;
    vel_proposta = vetor_soma(propria->velocidade,
                              vetor_escala(*aceleracao, cfg->dt));

    r = vetor_sub(propria->posicao, outra->posicao);
    v_rel = vetor_sub(vel_proposta, outra->velocidade);
    r_cpa = cpa_relativo(r, v_rel, cfg->horizonte_predicao);
    aplicar_desvio(&r_cpa,
                   1.30 * cfg->separacao_pe_horizontal,
                   1.30 * cfg->separacao_pe_vertical,
                   aceleracao,
                   interveio);

    for (i = 0; i < n_trafegos; ++i) {
        r = vetor_sub(propria->posicao, trafegos[i].posicao);
        v_rel = vetor_sub(vel_proposta, trafegos[i].velocidade);
        r_cpa = cpa_relativo(r, v_rel, cfg->horizonte_predicao);
        aplicar_desvio(&r_cpa,
                       1.50 * cfg->separacao_civil_horizontal,
                       1.50 * cfg->separacao_civil_vertical,
                       aceleracao,
                       interveio);
    }

    /* Barreira imediata adicional para estados já próximos do limite. */
    r = vetor_sub(propria->posicao, outra->posicao);
    if (vetor_norma_horizontal(r) < 1.15 * cfg->separacao_pe_horizontal &&
        absd(r.z) < 1.15 * cfg->separacao_pe_vertical) {
        aplicar_desvio(&r,
                       1.15 * cfg->separacao_pe_horizontal,
                       1.15 * cfg->separacao_pe_vertical,
                       aceleracao,
                       interveio);
    }

    *aceleracao = limitar_aceleracao(*aceleracao, cfg->aceleracao_maxima);
}

double menor_separacao_civil_horizontal(const Aeronave *a,
                                         const Trafego *trafegos,
                                         int n_trafegos,
                                         double *vertical_correspondente)
{
    int i;
    double menor;
    double dh;
    Vetor3 delta;
    menor = 1.0e30;
    *vertical_correspondente = 1.0e30;
    for (i = 0; i < n_trafegos; ++i) {
        delta = vetor_sub(a->posicao, trafegos[i].posicao);
        dh = vetor_norma_horizontal(delta);
        if (dh < menor) {
            menor = dh;
            *vertical_correspondente = fabs(delta.z);
        }
    }
    return menor;
}
