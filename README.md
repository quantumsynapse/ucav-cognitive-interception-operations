# Jogo Causal Cognitivo para Interceptação Aérea Não Cinética e Desescalada

Simulação de uma interceptação aérea não cinética entre duas aeronaves não tripuladas representadas como agentes cognitivos em software. O perseguidor procura estabelecer uma geometria segura de acompanhamento, iniciar comunicação estruturada e induzir o evasor a aceitar um corredor controlado e uma condição de escolta.

O projeto não contém armamento, seleção de alvos, solução de tiro, guiamento terminal ou decisão autônoma de emprego de força.

## Objetivo

Avaliar se comunicação estruturada, inferência de intenção e supervisão determinística de segurança podem aumentar a probabilidade de cumprimento sem degradar a separação em um espaço aéreo simulado com tráfego cooperativo.

## Arquitetura

O modelo contém:

- agente perseguidor;
- agente evasor;
- tráfegos cooperativos sintéticos;
- camada cinemática;
- camada cognitiva;
- canal de comunicação sujeito a perda e atraso;
- linguagem natural controlada;
- supervisor independente de detectar e evitar;
- comparação por Monte Carlo.

A linguagem não produz comandos de voo diretamente. As mensagens são convertidas em atos semânticos discretos e a atuação proposta passa por um supervisor determinístico.

## Estratégias comparadas

1. Sem comunicação.
2. Mensagens fixas e repetitivas.
3. Linguagem controlada baseada em regras.
4. Agente cognitivo com inferência de intenção, verificação cinemática e supervisor de segurança.

O supervisor de segurança permanece ativo em todas as estratégias. Essa escolha evita comparar uma solução segura com uma referência deliberadamente insegura.

## Cenário sintético de referência

| Parâmetro | Valor |
|:---|---:|
| Duração | 900 s |
| Passo de integração | 0,5 s |
| Execuções Monte Carlo | 1.000 por estratégia |
| Tráfegos cooperativos | 24 |
| Velocidade admissível | 45 a 95 m/s |
| Aceleração máxima | 3,0 m/s² |
| Horizonte do supervisor | 60 s |
| Distância nominal de acompanhamento | 1.200 m |
| Distância máxima para iniciar diálogo | 5.000 m |

As velocidades, acelerações, altitudes e limiares são genéricos e inteiramente sintéticos. Não representam uma aeronave real e não constituem limites regulatórios.

## Envelopes internos do experimento

| Relação | Horizontal | Vertical |
|:---|---:|---:|
| Perseguidor–evasor | 500 m | 100 m |
| Aeronaves do experimento–tráfego cooperativo | 1.000 m | 200 m |

O modelo considera conflito quando as duas dimensões ficam simultaneamente abaixo dos respectivos limiares. A margem normalizada é definida como o maior valor entre a razão horizontal e a razão vertical. Valores iguais ou superiores a 1 indicam que ao menos uma dimensão permanece fora do volume interno de conflito.

## Resultados sintéticos

Resultados obtidos com 1.000 execuções por estratégia e semente reprodutível:

| Estratégia | Cumprimento seguro | Escolta concluída | Cenários com violação | Tempo médio até cumprimento | Ambiguidade média |
|:---|---:|---:|---:|---:|---:|
| Sem comunicação | 0,0% | 0,0% | 0,0% | não aplicável | não aplicável |
| Mensagens fixas | 25,2% | 19,9% | 0,1% | 597,82 s | 0,2739 |
| Linguagem controlada | 33,8% | 33,8% | 0,4% | 81,16 s | 0,0180 |
| Cognitiva segura | 66,5% | 66,5% | 0,0% | 107,00 s | 0,0146 |

Na configuração sintética, a estratégia cognitiva segura apresentou a maior taxa de cumprimento. A linguagem controlada apresentou menor tempo médio entre os casos bem-sucedidos. O resultado indica uma troca entre rapidez e verificação adicional de coerência.

Os intervalos de confiança de 95% para a taxa de cumprimento, pelo método de Wilson, foram:

- mensagens fixas: 22,6% a 28,0%;
- linguagem controlada: 30,9% a 36,8%;
- cognitiva segura: 63,5% a 69,4%.

Esses valores descrevem somente o gerador sintético incluído neste repositório.

## Compilação

Requisitos:

- Linux;
- GCC;
- GNU Make;
- biblioteca matemática padrão.

```bash
make clean
make
```

A compilação utiliza C90 estrito:

```text
gcc -std=c89 -pedantic -Wall -Wextra -O2
```

## Execução

Simulação de referência:

```bash
make referencia
```

Comparação Monte Carlo:

```bash
make run
```

Validação completa:

```bash
make check
```

## Saídas

```text
resultados/trajetoria_referencia.csv
resultados/dialogo_referencia.txt
resultados/resumo_referencia.txt
resultados/resumo_monte_carlo.txt
```

## Estrutura

```text
.
├── Makefile
├── README.md
├── MODELO.md
├── DADOS_SINTETICOS.md
├── RELATORIO_VALIDACAO.md
├── SEGURANCA_E_LIMITES.md
├── REFERENCIAS.md
├── dados
│   └── configuracao.txt
├── include
│   └── modelo.h
├── src
│   ├── principal.c
│   ├── io.c
│   ├── util.c
│   ├── dinamica.c
│   ├── seguranca.c
│   ├── cognicao.c
│   ├── metricas.c
│   └── simulacao.c
├── testes
│   └── teste_basico.sh
└── resultados
    └── README.md
```

## Limites de uso

Este é um experimento de pesquisa computacional. Ele:

- não representa uma plataforma real;
- não implementa controle de voo certificável;
- não substitui detectar e evitar certificado;
- não produz autorizações de tráfego aéreo;
- não deve ser conectado a aeronaves;
- não deve ser utilizado para decisão operacional;
- não inclui funções cinéticas.

O uso adequado é análise acadêmica de interação entre agentes, linguagem controlada, raciocínio causal, segurança supervisória e desescalada em ambiente simulado.
