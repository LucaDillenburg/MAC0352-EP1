# MAC0352-EP1: Pub/Sub
Solução do Exercício-Projeto 1 de MAC0352 (Redes de Computadores e Sistemas Distribuídos do IME-USP), ministrada pelo Prof. Dr. Daniel Macêdo Batista.

### Compilar
Para compilar os binários basta rodar:
```sh
make
```

### Executar
Para executar o ep1:
```sh
./broker # será executado na porta 1883 (padrão do MQTT)
```
onde p é a porta utilizada pelo broker.

### Restrições
- QoS = 0
- Não considera as Flags
- Não considera o Keep Alive para disconectar se demorar muito tempo para receber os bytes
- Não foi programado o UNSUB
- Não foi programado sistemas de recuperação a falhas nem autenticação
- Não foi programado a inscrição de um subscriber em mais de um tópico nem utilizando wildcards
- Não foi programado a inscrição de um cliente ser publisher e subscriber

### Autor
- Nome: Luca Assumpção Dillenburg
- NUSP: 11796580
