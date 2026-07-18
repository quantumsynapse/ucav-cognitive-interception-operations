#!/bin/sh
set -eu

REPO="quantumsynapse/jogo-causal-cognitivo-interceptacao-aerea"

if [ -d .git ]; then
    echo "Este diretório já contém um repositório Git." >&2
    exit 1
fi

gh auth status
GITHUB_ID=$(gh api user --jq '.id')

git init
git branch -M main
git config user.name "Julio Antonio do Amaral"
git config user.email "${GITHUB_ID}+quantumsynapse@users.noreply.github.com"
git add .
git commit -m "Implementa modelo inicial de interceptação aérea cognitiva não cinética"

gh repo create "$REPO" \
    --public \
    --source=. \
    --remote=origin \
    --push \
    --description "Simulação não cinética de interceptação aérea entre agentes cognitivos com linguagem controlada e supervisão de segurança."
