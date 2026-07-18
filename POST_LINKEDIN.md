# Jogo Causal Cognitivo para Interceptação Aérea Não Cinética e Desescalada

Olá, pessoal.

Estou desenvolvendo um modelo computacional para simular uma interceptação aérea entre dois veículos aéreos não tripulados, representados inicialmente como agentes cognitivos em software.

No cenário, uma aeronave perseguidora tenta estabelecer uma condição segura de acompanhamento de uma aeronave evasora em espaço aéreo não segregado. Os agentes observam a dinâmica do ambiente, estimam as intenções um do outro e se comunicam por linguagem natural controlada. O objetivo é induzir a aeronave evasora a interromper a fuga, aceitar um corredor seguro e ingressar em uma condição de escolta.

A abordagem combina jogo causal, dinâmica híbrida, inferência de intenção, comunicação semântica e supervisão determinística de segurança. As mensagens não são convertidas diretamente em comandos de voo. Cada ação proposta passa por restrições de separação, tráfego cooperativo, autonomia e envelope de operação.

Em 1.000 cenários inteiramente sintéticos por estratégia, a configuração cognitiva segura obteve 66,5% de cumprimento seguro, contra 33,8% para linguagem controlada baseada em regras e 25,2% para mensagens fixas. Os resultados ainda dependem da parametrização e não representam validação operacional.

O estudo não inclui armamentos, seleção de alvos ou emprego autônomo de força. O foco está na interceptação não cinética, na desescalada e na escolta segura em ambiente simulado.

O modelo e o código, ainda em versão inicial, estarão disponíveis em meu GitHub para análise, críticas e contribuições.

Boa noite.

Julio.
