#include <math.h>
#include "modelo.h"

unsigned long rng_proximo(unsigned long *estado)
{
    *estado = (*estado * 1664525UL + 1013904223UL) & 0xffffffffUL;
    return *estado;
}

double rng_uniforme(unsigned long *estado)
{
    return (double)rng_proximo(estado) / 4294967295.0;
}

double rng_normal_aprox(unsigned long *estado)
{
    double soma;
    int i;
    soma = 0.0;
    for (i = 0; i < 12; ++i) {
        soma += rng_uniforme(estado);
    }
    return soma - 6.0;
}

Vetor3 vetor_soma(Vetor3 a, Vetor3 b)
{
    Vetor3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

Vetor3 vetor_sub(Vetor3 a, Vetor3 b)
{
    Vetor3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

Vetor3 vetor_escala(Vetor3 a, double s)
{
    Vetor3 r;
    r.x = a.x * s;
    r.y = a.y * s;
    r.z = a.z * s;
    return r;
}

double vetor_norma_horizontal(Vetor3 a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

double vetor_norma(Vetor3 a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vetor3 vetor_unitario_horizontal(Vetor3 a)
{
    double n;
    Vetor3 r;
    n = vetor_norma_horizontal(a);
    if (n < 1.0e-9) {
        r.x = 1.0;
        r.y = 0.0;
        r.z = 0.0;
        return r;
    }
    r.x = a.x / n;
    r.y = a.y / n;
    r.z = 0.0;
    return r;
}

Vetor3 limitar_aceleracao(Vetor3 a, double maximo)
{
    double n;
    n = vetor_norma(a);
    if (n > maximo && n > 0.0) {
        return vetor_escala(a, maximo / n);
    }
    return a;
}
