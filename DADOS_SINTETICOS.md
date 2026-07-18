# Dados Sintéticos e Justificativa dos Parâmetros

## Princípio

Nenhum parâmetro foi extraído de uma aeronave militar real. Os valores foram escolhidos para criar uma simulação genérica, reproduzível e numericamente tratável.

## Estado inicial

| Elemento | Configuração |
|:---|:---|
| Perseguidor | 6 km atrás, 500 m deslocado lateralmente e 200 m acima |
| Evasor | trajetória nominal para leste |
| Tráfego cooperativo | 24 trajetórias sintéticas |
| Tráfego de teste | uma trajetória cruzando a rota nominal |

Os demais tráfegos são inicializados a pelo menos 6 km das duas aeronaves principais. Isso evita iniciar o experimento em condição artificialmente impossível de resolver.

## Bandas de altitude

Os tráfegos cooperativos aleatórios são distribuídos em duas bandas sintéticas, abaixo e acima da região principal de interceptação. Um tráfego específico é colocado na mesma região vertical para exercitar o supervisor de detectar e evitar.

Essa estrutura representa um espaço aéreo organizado, porém não segregado. Não se pretende reproduzir setores reais ou regras de separação de um provedor de serviço de navegação aérea.

## Velocidade

A faixa de 45 a 95 m/s corresponde a um regime subsônico moderado. O objetivo é permitir acompanhamento, redução de velocidade e resolução de conflitos sem reproduzir desempenho de combate.

## Aceleração

O limite de 3,0 m/s² equivale aproximadamente a 0,31 g. Trata-se de uma aceleração moderada para a simulação e não de um envelope estrutural certificado.

## Horizonte de detectar e evitar

O horizonte de 60 segundos fornece tempo para correções graduais. A escolha foi verificada por refinamento temporal e por 1.000 execuções por estratégia.

## Comunicação

- perda nominal por mensagem: 1,5%, acrescida de penalização por qualidade do enlace;
- atraso médio: 1,5 s;
- início do diálogo: até 5 km;
- limiar cognitivo de cumprimento: 0,70.

Esses valores são parâmetros de software. Não representam uma rede de comunicação real.

## Propensão ao cumprimento

Cada cenário sorteia uma variável latente entre 0,30 e 0,80. Ela representa diferenças sintéticas de comportamento do evasor e não um perfil psicológico real.

## Reprodutibilidade

A semente padrão é:

```text
20260718
```

O gerador pseudoaleatório é simples e determinístico. Ele é adequado à repetição do experimento, mas não a aplicações criptográficas.
