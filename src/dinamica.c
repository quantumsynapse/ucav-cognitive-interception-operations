#include <math.h>
#include "modelo.h"

static double limitar(double x, double minimo, double maximo)
{
    if (x < minimo) return minimo;
    if (x > maximo) return maximo;
    return x;
}

void inicializar_cenario(const Configuracao *cfg, unsigned long *estado_rng,
                         Aeronave *p, Aeronave *e, Trafego *trafegos,
                         int *n_trafegos, double *propensao_cumprimento)
{
    int i;
    double angulo;
    double velocidade;

    p->posicao.x = 4000.0;
    p->posicao.y = -500.0;
    p->posicao.z = 3200.0;
    p->velocidade.x = 78.0;
    p->velocidade.y = 0.0;
    p->velocidade.z = 0.0;
    p->autonomia = 1.0;
    p->qualidade_enlace = 0.96;
    p->energia_acumulada = 0.0;

    e->posicao.x = 10000.0;
    e->posicao.y = 0.0;
    e->posicao.z = 3000.0;
    e->velocidade.x = 68.0;
    e->velocidade.y = 0.0;
    e->velocidade.z = 0.0;
    e->autonomia = 1.0;
    e->qualidade_enlace = 0.94;
    e->energia_acumulada = 0.0;

    *n_trafegos = cfg->numero_trafegos;
    for (i = 0; i < *n_trafegos; ++i) {
        int tentativas;
        double dp;
        double de;
        tentativas = 0;
        do {
            trafegos[i].posicao.x = 2000.0 + 52000.0 * rng_uniforme(estado_rng);
            trafegos[i].posicao.y = -18000.0 + 36000.0 * rng_uniforme(estado_rng);
            if ((i % 2) == 0)
                trafegos[i].posicao.z = 2200.0 + 350.0 * rng_uniforme(estado_rng);
            else
                trafegos[i].posicao.z = 3650.0 + 350.0 * rng_uniforme(estado_rng);
            dp = vetor_norma_horizontal(vetor_sub(trafegos[i].posicao, p->posicao));
            de = vetor_norma_horizontal(vetor_sub(trafegos[i].posicao, e->posicao));
            tentativas++;
        } while ((dp < 6000.0 || de < 6000.0) && tentativas < 100);

        angulo = 2.0 * PI * rng_uniforme(estado_rng);
        velocidade = 50.0 + 30.0 * rng_uniforme(estado_rng);
        trafegos[i].velocidade.x = velocidade * cos(angulo);
        trafegos[i].velocidade.y = velocidade * sin(angulo);
        trafegos[i].velocidade.z = 0.0;
        trafegos[i].cooperativo = 1;
    }

    /* Tráfego cruzando a rota nominal, usado para exercitar o DAA. */
    trafegos[0].posicao.x = 20000.0;
    trafegos[0].posicao.y = -8000.0;
    trafegos[0].posicao.z = 3000.0;
    trafegos[0].velocidade.x = 0.0;
    trafegos[0].velocidade.y = 55.0;
    trafegos[0].velocidade.z = 0.0;

    *propensao_cumprimento = 0.30 + 0.50 * rng_uniforme(estado_rng);
}

void atualizar_trafegos(Trafego *trafegos, int n, double dt)
{
    int i;
    for (i = 0; i < n; ++i) {
        trafegos[i].posicao = vetor_soma(trafegos[i].posicao,
                                        vetor_escala(trafegos[i].velocidade, dt));
        if (trafegos[i].posicao.x > 60000.0) trafegos[i].posicao.x -= 60000.0;
        if (trafegos[i].posicao.x < 0.0) trafegos[i].posicao.x += 60000.0;
        if (trafegos[i].posicao.y > 22000.0) trafegos[i].posicao.y -= 44000.0;
        if (trafegos[i].posicao.y < -22000.0) trafegos[i].posicao.y += 44000.0;
    }
}

Vetor3 comando_perseguidor(const Aeronave *p, const Aeronave *e,
                           double distancia_interceptacao,
                           double aceleracao_maxima)
{
    Vetor3 direcao_e;
    Vetor3 alvo;
    Vetor3 erro_pos;
    Vetor3 erro_vel;
    Vetor3 comando;

    direcao_e = vetor_unitario_horizontal(e->velocidade);
    alvo = vetor_sub(e->posicao, vetor_escala(direcao_e, distancia_interceptacao));
    alvo.z = e->posicao.z + 200.0;
    erro_pos = vetor_sub(alvo, p->posicao);
    erro_vel = vetor_sub(e->velocidade, p->velocidade);
    comando = vetor_soma(vetor_escala(erro_pos, 0.0007),
                         vetor_escala(erro_vel, 0.06));
    return limitar_aceleracao(comando, aceleracao_maxima);
}

void integrar_aeronave(Aeronave *a, Vetor3 aceleracao, const Configuracao *cfg)
{
    double velocidade;
    Vetor3 direcao;

    a->velocidade = vetor_soma(a->velocidade, vetor_escala(aceleracao, cfg->dt));
    velocidade = vetor_norma_horizontal(a->velocidade);
    if (velocidade < cfg->velocidade_minima) {
        direcao = vetor_unitario_horizontal(a->velocidade);
        a->velocidade.x = direcao.x * cfg->velocidade_minima;
        a->velocidade.y = direcao.y * cfg->velocidade_minima;
    } else if (velocidade > cfg->velocidade_maxima) {
        direcao = vetor_unitario_horizontal(a->velocidade);
        a->velocidade.x = direcao.x * cfg->velocidade_maxima;
        a->velocidade.y = direcao.y * cfg->velocidade_maxima;
    }
    a->velocidade.z = limitar(a->velocidade.z, -8.0, 8.0);
    a->posicao = vetor_soma(a->posicao, vetor_escala(a->velocidade, cfg->dt));
    a->posicao.z = limitar(a->posicao.z, 1500.0, 5000.0);
    a->energia_acumulada += vetor_norma(aceleracao) * vetor_norma(aceleracao) * cfg->dt;
    a->autonomia -= (0.000004 * velocidade + 0.00002 * vetor_norma(aceleracao)) * cfg->dt;
    if (a->autonomia < 0.0) a->autonomia = 0.0;
}
