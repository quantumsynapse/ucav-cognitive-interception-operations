#include <stdio.h>
#include <math.h>
#include "modelo.h"

static double limitar01(double x)
{
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void inicializar_cognicao(EstadoCognitivo *cog)
{
    cog->prob_cumprimento = 0.15;
    cog->prob_emergencia = 0.02;
    cog->ambiguidade = 0.20;
    cog->coerencia_cinematica = 0.0;
    cog->mensagens_enviadas = 0;
    cog->mensagens_recebidas = 0;
    cog->confirmacoes = 0;
    cog->identificacao_solicitada = 0;
    cog->corredor_oferecido = 0;
    cog->cumprimento_solicitado = 0;
    cog->resposta_recebida = 0;
    cog->instante_primeira_mensagem = -1.0;
    cog->instante_cumprimento = -1.0;
    cog->proximo_instante_mensagem = 0.0;
}

static void registrar(FILE *log, double t, const char *origem,
                      const char *mensagem)
{
    if (log != NULL) {
        fprintf(log, "[%07.1f s] %s: %s\n", t, origem, mensagem);
    }
}

static int mensagem_entregue(const Configuracao *cfg,
                             const Aeronave *p,
                             const Aeronave *e,
                             unsigned long *rng)
{
    double q;
    double p_perda;
    q = 0.5 * (p->qualidade_enlace + e->qualidade_enlace);
    p_perda = cfg->probabilidade_perda_enlace + 0.10 * (1.0 - q);
    return rng_uniforme(rng) >= p_perda;
}

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
                           FILE *log_dialogo)
{
    double distancia;
    double ruido;
    double incremento;
    int entregue;
    int emitiu;

    distancia = vetor_norma_horizontal(vetor_sub(e->posicao, p->posicao));
    if (estrategia == ESTRATEGIA_SEM_COMUNICACAO) return;
    if (distancia > cfg->distancia_dialogo) return;
    if (t < cog->proximo_instante_mensagem) return;

    ruido = 0.04 * fabs(rng_normal_aprox(estado_rng));
    entregue = mensagem_entregue(cfg, p, e, estado_rng);
    emitiu = 0;

    if (cog->instante_primeira_mensagem < 0.0) {
        cog->instante_primeira_mensagem = t;
    }

    if (estrategia == ESTRATEGIA_MENSAGENS_FIXAS) {
        registrar(log_dialogo, t, "PERSEGUIDOR",
                  "Aeronave à frente, reduza a velocidade e siga em frente. Confirme.");
        cog->mensagens_enviadas++;
        emitiu = 1;
        cog->ambiguidade = limitar01(0.26 + ruido);
        cog->proximo_instante_mensagem = t + 45.0;
        if (entregue) {
            cog->mensagens_recebidas++;
            incremento = 0.10 + 0.18 * propensao_cumprimento - cog->ambiguidade * 0.12;
            cog->prob_cumprimento = limitar01(cog->prob_cumprimento + incremento);
            if (rng_uniforme(estado_rng) < 0.45 * propensao_cumprimento) {
                registrar(log_dialogo, t + cfg->atraso_medio_enlace, "EVASOR",
                          "Mensagem recebida. Avaliando instrução.");
                cog->resposta_recebida = 1;
                cog->mensagens_recebidas++;
                if (*modo == MODO_CUMPRIMENTO_PARCIAL &&
                    cog->prob_cumprimento >= cfg->limiar_cumprimento) {
                    *modo = MODO_CUMPRIMENTO;
                    cog->instante_cumprimento = t;
                    cog->confirmacoes++;
                } else if (cog->prob_cumprimento >= 0.55) {
                    *modo = MODO_CUMPRIMENTO_PARCIAL;
                } else {
                    *modo = MODO_NEGOCIACAO;
                }
            }
        }
    } else {
        if (!cog->identificacao_solicitada) {
            registrar(log_dialogo, t, "PERSEGUIDOR",
                      "Aeronave à frente, identifique-se e declare sua condição operacional. Confirme o recebimento.");
            cog->identificacao_solicitada = 1;
            cog->mensagens_enviadas++;
            emitiu = 1;
            cog->ambiguidade = limitar01((estrategia == ESTRATEGIA_COGNITIVA_SEGURA ? 0.025 : 0.08) + ruido);
            cog->proximo_instante_mensagem = t + 20.0;
            if (entregue) {
                registrar(log_dialogo, t + cfg->atraso_medio_enlace, "EVASOR",
                          "Recebido. Sistemas de voo operacionais. Intenção em avaliação.");
                cog->mensagens_recebidas += 2;
                cog->resposta_recebida = 1;
                cog->confirmacoes++;
                *modo = MODO_NEGOCIACAO;
                cog->prob_cumprimento = limitar01(cog->prob_cumprimento + 0.12 + 0.15 * propensao_cumprimento);
            }
        } else if (!cog->corredor_oferecido &&
                   (menor_sep_civil_e < 5000.0 || t > cog->instante_primeira_mensagem + 25.0)) {
            registrar(log_dialogo, t, "PERSEGUIDOR",
                      "Conflito de tráfego previsto. Corredor seguro C2 disponível. Reduza para 58 metros por segundo e mantenha proa leste. Confirme intenção.");
            cog->corredor_oferecido = 1;
            cog->mensagens_enviadas++;
            emitiu = 1;
            cog->proximo_instante_mensagem = t + 18.0;
            if (entregue) {
                cog->mensagens_recebidas++;
                incremento = (estrategia == ESTRATEGIA_COGNITIVA_SEGURA ? 0.22 : 0.18) +
                             0.20 * propensao_cumprimento - 0.10 * cog->ambiguidade;
                cog->prob_cumprimento = limitar01(cog->prob_cumprimento + incremento);
                registrar(log_dialogo, t + cfg->atraso_medio_enlace, "EVASOR",
                          "Corredor C2 recebido. Iniciando redução de velocidade. Cumprimento parcial.");
                cog->mensagens_recebidas++;
                cog->confirmacoes++;
                *modo = MODO_CUMPRIMENTO_PARCIAL;
            }
        } else if (!cog->cumprimento_solicitado &&
                   cog->corredor_oferecido) {
            registrar(log_dialogo, t, "PERSEGUIDOR",
                      "Confirme adesão integral ao corredor C2 e manutenção da separação. Após confirmação, será iniciada escolta.");
            cog->cumprimento_solicitado = 1;
            cog->mensagens_enviadas++;
            emitiu = 1;
            cog->proximo_instante_mensagem = t + 30.0;
            if (entregue) {
                cog->mensagens_recebidas++;
                incremento = (estrategia == ESTRATEGIA_COGNITIVA_SEGURA ? 0.18 : 0.15) +
                             0.15 * propensao_cumprimento;
                cog->prob_cumprimento = limitar01(cog->prob_cumprimento + incremento);
                if (cog->prob_cumprimento >= cfg->limiar_cumprimento &&
                    rng_uniforme(estado_rng) <
                    (estrategia == ESTRATEGIA_COGNITIVA_SEGURA ?
                     0.25 + 0.40 * propensao_cumprimento :
                     0.35 + 0.40 * propensao_cumprimento)) {
                    registrar(log_dialogo, t + cfg->atraso_medio_enlace, "EVASOR",
                              "Adesão integral ao corredor C2 confirmada. Pronto para escolta.");
                    cog->mensagens_recebidas++;
                    cog->confirmacoes++;
                    *modo = MODO_CUMPRIMENTO;
                    if (cog->instante_cumprimento < 0.0) cog->instante_cumprimento = t;
                }
            }
        } else if (estrategia == ESTRATEGIA_COGNITIVA_SEGURA &&
                   cog->corredor_oferecido &&
                   cog->mensagens_enviadas < 6 &&
                   *modo != MODO_CUMPRIMENTO) {
            if (cog->coerencia_cinematica > 0.65) {
                registrar(log_dialogo, t, "PERSEGUIDOR",
                          "Movimento compatível com o corredor C2. Confirme manutenção do cumprimento.");
                cog->prob_cumprimento = limitar01(cog->prob_cumprimento + 0.10);
            } else {
                registrar(log_dialogo, t, "PERSEGUIDOR",
                          "Movimento ainda incompatível com o corredor declarado. Informe sua intenção e mantenha separação do tráfego cooperativo.");
                cog->prob_cumprimento = limitar01(cog->prob_cumprimento - 0.04);
                cog->ambiguidade = limitar01(cog->ambiguidade + 0.015);
            }
            cog->mensagens_enviadas++;
            emitiu = 1;
            cog->proximo_instante_mensagem = t + 45.0;
            if (entregue) {
                cog->mensagens_recebidas++;
                if (cog->coerencia_cinematica > 0.65)
                    cog->prob_cumprimento = limitar01(cog->prob_cumprimento + 0.08);
                if (cog->prob_cumprimento >= cfg->limiar_cumprimento &&
                    rng_uniforme(estado_rng) < 0.05 + 0.30 * propensao_cumprimento) {
                    registrar(log_dialogo, t + cfg->atraso_medio_enlace, "EVASOR",
                              "Manutenção do cumprimento confirmada. Permanecendo no corredor C2.");
                    cog->mensagens_recebidas++;
                    cog->confirmacoes++;
                    *modo = MODO_CUMPRIMENTO;
                    if (cog->instante_cumprimento < 0.0)
                        cog->instante_cumprimento = t;
                }
            }
        }
    }

    if (emitiu && !entregue) {
        cog->ambiguidade = limitar01(cog->ambiguidade + 0.08);
    }
}

void atualizar_crenca_cinematica(const Aeronave *e,
                                 ModoEvasor modo,
                                 EstadoCognitivo *cog,
                                 double dt)
{
    double velocidade;
    double alinhamento;
    velocidade = vetor_norma_horizontal(e->velocidade);
    if (velocidade > 1.0e-9) alinhamento = e->velocidade.x / velocidade;
    else alinhamento = 0.0;

    if (modo == MODO_CUMPRIMENTO || modo == MODO_CUMPRIMENTO_PARCIAL) {
        cog->coerencia_cinematica += dt * (0.20 * alinhamento + 0.15 * (velocidade < 64.0));
    } else {
        cog->coerencia_cinematica -= dt * 0.03;
    }
    cog->coerencia_cinematica = limitar01(cog->coerencia_cinematica);
    cog->prob_cumprimento = limitar01(cog->prob_cumprimento +
                                      dt * 0.006 * (cog->coerencia_cinematica - 0.5));
    if (cog->coerencia_cinematica > 0.55)
        cog->ambiguidade = limitar01(cog->ambiguidade - dt * 0.003);
}
