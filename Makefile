CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lm
ALVO = interceptacao_cognitiva
OBJ = src/principal.o src/io.o src/util.o src/dinamica.o src/seguranca.o src/cognicao.o src/metricas.o src/simulacao.o

all: $(ALVO)

$(ALVO): $(OBJ)
	$(CC) $(CFLAGS) -o $(ALVO) $(OBJ) $(LDFLAGS)

src/%.o: src/%.c include/modelo.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(ALVO)
	mkdir -p resultados
	./$(ALVO) dados/configuracao.txt | tee resultados/resumo_monte_carlo.txt

referencia: $(ALVO)
	mkdir -p resultados
	./$(ALVO) dados/configuracao.txt --referencia | tee resultados/resumo_referencia.txt

check: clean all referencia run
	sh testes/teste_basico.sh

clean:
	rm -f $(OBJ) $(ALVO)
	rm -f resultados/trajetoria_referencia.csv resultados/dialogo_referencia.txt
	rm -f resultados/resumo_referencia.txt resultados/resumo_monte_carlo.txt

.PHONY: all run referencia check clean
