# Especificação do Modelo

## 1. Definição do problema

O teatro sintético contém duas aeronaves não tripuladas principais:

- P: perseguidor;
- E: evasor.

O espaço aéreo contém ainda tráfegos cooperativos T1, ..., Tn. Eles não participam do jogo de negociação, mas impõem restrições de segurança.

O objetivo de P é estabelecer acompanhamento seguro, comunicar intenção, oferecer um corredor controlado e obter confirmação coerente de cumprimento. O objetivo de E é preservar segurança, autonomia e integridade operacional enquanto decide entre continuar a evasão, negociar ou aceitar a escolta.

## 2. Grafo causal multicamadas

O estado global é organizado em quatro camadas:

```text
G(t) = {V, E_cin, E_com, E_ris, E_cog}
```

- E_cin: dependências cinemáticas.
- E_com: mensagens, confirmações, atraso e perda.
- E_ris: proximidade, conflito previsto e restrições do espaço aéreo.
- E_cog: crenças sobre intenção, cumprimento e coerência.

O grafo é conceitual na implementação atual. Os acoplamentos são calculados diretamente a partir dos estados relativos.

## 3. Estado cinemático

Para cada aeronave:

```text
x_i = {posição, velocidade, autonomia, qualidade do enlace}
```

A integração é realizada por Euler explícito:

```text
v[k+1] = v[k] + a[k] dt
p[k+1] = p[k] + v[k+1] dt
```

O integrador é suficiente para o protótipo de baixa ordem, mas não representa dinâmica de seis graus de liberdade.

## 4. Geometria de acompanhamento

O perseguidor não aponta para a posição instantânea do evasor. Ele busca um ponto de acompanhamento atrás da direção de movimento de E:

```text
p_alvo = p_E - d_acompanhamento * direção(v_E) + deslocamento_vertical
```

O comando nominal utiliza erro de posição e erro de velocidade:

```text
a_P = Kp (p_alvo - p_P) + Kv (v_E - v_P)
```

A aceleração é limitada antes de passar ao supervisor.

Essa lei é uma aproximação de acompanhamento seguro. Não é uma lei de guiamento para emprego cinético.

## 5. Modos do evasor

```text
EVASÃO
NEGOCIAÇÃO
CUMPRIMENTO_PARCIAL
CUMPRIMENTO
EMERGÊNCIA
PERDA_DE_ENLACE
```

Na evasão, E mantém velocidade nominal e pequenas variações sintéticas de proa. No cumprimento parcial, reduz velocidade e diminui as variações. No cumprimento, segue o corredor sintético C2 com velocidade estabilizada.

## 6. Comunicação

A comunicação usa linguagem natural controlada. O vocabulário é mapeado para atos discretos:

```text
SOLICITAR_IDENTIFICAÇÃO
INFORMAR_RISCO
OFERECER_CORREDOR
SOLICITAR_CUMPRIMENTO
CONFIRMAR_CUMPRIMENTO
INFORMAR_INCOERÊNCIA
```

A mensagem possui:

```text
{origem, destino, intenção, ação, justificativa, prazo, confirmação}
```

O canal inclui perda aleatória e atraso médio. Nenhuma mensagem modifica diretamente o comando de aceleração.

## 7. Crença de cumprimento

P mantém uma variável contínua:

```text
p_cumprimento em [0, 1]
```

Ela é atualizada por:

- confirmação recebida;
- especificidade da mensagem;
- propensão latente sintética do evasor;
- coerência entre movimento e declaração;
- perda de mensagem;
- ambiguidade.

A transição para cumprimento requer:

```text
p_cumprimento >= limiar
confirmação recebida
movimento compatível com o corredor
```

## 8. Supervisor de segurança

O supervisor opera fora do módulo cognitivo.

Para cada par de aeronaves, calcula-se uma aproximação do ponto de maior proximidade horizontal dentro de um horizonte finito. Se a separação prevista entra no volume interno de conflito, é aplicada uma correção prioritariamente vertical e, em segundo plano, horizontal.

O comando executado é:

```text
u_executado = limitar(u_cognitivo + u_supervisor)
```

A linguagem e a crença não podem desativar o supervisor.

## 9. Margem normalizada

Para um par com limiares H e V:

```text
margem = máximo(d_horizontal / H, d_vertical / V)
```

- margem < 1: as duas dimensões estão dentro do volume interno de conflito;
- margem = 1: fronteira;
- margem > 1: pelo menos uma dimensão permanece fora do volume.

Essa métrica não equivale a separação regulatória.

## 10. Estratégias

### Sem comunicação

P apenas acompanha E. Não existe mecanismo de cumprimento.

### Mensagens fixas

A mesma instrução é repetida em intervalos regulares. A ambiguidade é maior e não existe interpretação de coerência.

### Linguagem controlada

As mensagens seguem uma sequência baseada em regras: identificação, aviso de risco, corredor e confirmação.

### Cognitiva segura

A sequência é adaptada pela crença, pela coerência cinemática e pelo risco observado. O agente solicita esclarecimentos quando a fala e o movimento divergem.

## 11. Critério de sucesso

Cumprimento seguro exige:

```text
modo final = CUMPRIMENTO
nenhuma violação de separação no cenário
```

A escolta é considerada concluída quando o cumprimento permanece por pelo menos 120 segundos.

## 12. Complexidade

Com N tráfegos e K passos:

```text
tempo = O(KN)
memória = O(N)
```

O Monte Carlo com M execuções possui custo O(MKN).
