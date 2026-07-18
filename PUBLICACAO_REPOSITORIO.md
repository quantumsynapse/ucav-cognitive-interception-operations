# Publicação do Repositório

Nome sugerido:

```text
jogo-causal-cognitivo-interceptacao-aerea
```

Descrição sugerida:

```text
Simulação não cinética de interceptação aérea entre agentes cognitivos, com linguagem controlada, inferência de intenção e supervisor independente de segurança.
```

Comandos:

```bash
git init
git branch -M main

GITHUB_ID=$(gh api user --jq '.id')
git config user.name "Julio Antonio do Amaral"
git config user.email "${GITHUB_ID}+quantumsynapse@users.noreply.github.com"

git add .
git commit -m "Implementa modelo inicial de interceptação aérea cognitiva não cinética"

gh repo create quantumsynapse/jogo-causal-cognitivo-interceptacao-aerea \
  --public \
  --source=. \
  --remote=origin \
  --push \
  --description "Simulação não cinética de interceptação aérea entre agentes cognitivos com linguagem controlada e supervisão de segurança."
```
