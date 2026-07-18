# Segurança, Escopo e Limites

## Separação entre cognição e controle

O módulo cognitivo pode sugerir mensagens e intenções de movimento. Ele não possui autoridade direta sobre a atuação.

A cadeia é:

```text
observação
-> inferência
-> ação proposta
-> supervisor de segurança
-> comando limitado
```

## Comunicação

A implementação adota linguagem controlada. Em contexto aeronáutico real, fraseologia padronizada deve ter precedência. Linguagem comum clara e concisa deve ser reservada a situações em que a fraseologia não seja suficiente.

## Detectar e evitar

O supervisor deste projeto é apenas uma aproximação geométrica baseada em maior proximidade prevista. Integração real de aeronaves não tripuladas em espaço aéreo não segregado exige meios de conformidade, avaliação de risco, procedimentos, comunicações e capacidades de detectar e evitar apropriadas.

## Serviço de tráfego aéreo

O projeto não simula autorizações ATC. Os tráfegos cooperativos e as restrições são entradas exógenas. A comunicação entre P e E não substitui comunicação com órgãos de controle.

## Funções excluídas

Não existem:

- armas;
- sensores de aquisição de alvo;
- avaliação de letalidade;
- solução de tiro;
- guiamento de munição;
- seleção autônoma de alvos;
- regras de engajamento.

## Uso permitido

- ensino de sistemas híbridos;
- estudo de jogos causais;
- pesquisa em agentes cognitivos;
- análise de linguagem controlada;
- verificação de supervisores de segurança;
- simulação de desescalada.

## Uso inadequado

- controle de aeronave real;
- planejamento operacional;
- validação regulatória;
- treinamento tático armado;
- decisão autônoma sobre emprego de força.
