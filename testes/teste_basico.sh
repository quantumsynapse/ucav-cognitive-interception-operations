#!/bin/sh
set -eu

[ -x ./interceptacao_cognitiva ]
[ -s resultados/resumo_referencia.txt ]
[ -s resultados/resumo_monte_carlo.txt ]
[ -s resultados/trajetoria_referencia.csv ]
[ -s resultados/dialogo_referencia.txt ]

grep -q 'violacoes=0' resultados/resumo_referencia.txt
grep -q 'estrategia=cognitiva_segura' resultados/resumo_monte_carlo.txt

echo 'teste_basico=aprovado'
