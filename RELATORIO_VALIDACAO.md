# Relatório de Validação Computacional

## Escopo da validação

A validação realizada é interna e computacional. Ela verifica compilação, consistência de unidades, reprodutibilidade, estabilidade do passo temporal, comportamento das estratégias e atuação do supervisor.

Ela não demonstra validade operacional, certificação aeronáutica ou aplicabilidade em voo.

## Compilação

```text
gcc -std=c89 -pedantic -Wall -Wextra -O2
```

Resultado: aprovado sem avisos.

## Teste automatizado

```text
teste_basico=aprovado
```

O teste confirma:

- geração da trajetória;
- geração do diálogo;
- execução das quatro estratégias;
- ausência de violação no cenário de referência;
- presença da estratégia cognitiva no relatório.

## Cenário de referência

```text
cumprimento_seguro=1
escolta_concluida=1
margem_min_pe=2,400
margem_min_civil=1,328
tempo_cumprimento=108,00 s
mensagens=10
violacoes=0
```

## Monte Carlo

Foram executados 1.000 cenários por estratégia.

| Estratégia | Cumprimento seguro | Escolta | Violação | Tempo médio de cumprimento |
|:---|---:|---:|---:|---:|
| Sem comunicação | 0,0% | 0,0% | 0,0% | não aplicável |
| Mensagens fixas | 25,2% | 19,9% | 0,1% | 597,82 s |
| Linguagem controlada | 33,8% | 33,8% | 0,4% | 81,16 s |
| Cognitiva segura | 66,5% | 66,5% | 0,0% | 107,00 s |

## Interpretação

A estratégia cognitiva segura aumentou a taxa de cumprimento em relação às demais estratégias no gerador sintético.

A linguagem controlada foi mais rápida entre os casos bem-sucedidos, pois utiliza uma sequência curta e não executa verificações adicionais de incoerência. A estratégia cognitiva demorou aproximadamente 26 segundos a mais, mas apresentou taxa de sucesso superior.

A taxa residual de violação não é zero em todas as estratégias. Isso mostra que o supervisor é uma aproximação experimental, não um sistema certificado de detectar e evitar.

## Intervalos de confiança

Intervalos de 95% pelo método de Wilson:

| Estratégia | Intervalo da taxa de cumprimento |
|:---|:---|
| Mensagens fixas | 22,6% a 28,0% |
| Linguagem controlada | 30,9% a 36,8% |
| Cognitiva segura | 63,5% a 69,4% |

## Refinamento temporal

Uma execução adicional utilizou:

```text
dt = 0,25 s
500 cenários por estratégia
```

Resultados principais:

| Estratégia | Cumprimento com dt=0,5 | Cumprimento com dt=0,25 |
|:---|---:|---:|
| Mensagens fixas | 25,2% | 27,4% |
| Linguagem controlada | 33,8% | 31,4% |
| Cognitiva segura | 66,5% | 67,0% |

As diferenças são compatíveis com variação amostral e discretização. O tempo médio de cumprimento variou menos de 0,4 segundo nas estratégias controlada e cognitiva.

## Conclusão

O modelo está consistente como protótipo sintético de pesquisa. Os resultados podem ser apresentados desde que sejam acompanhados das seguintes qualificações:

- dados inteiramente sintéticos;
- nenhuma plataforma real representada;
- ausência de certificação;
- supervisor de segurança experimental;
- resultados dependentes da parametrização;
- uso exclusivo em ambiente simulado.
