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
make run # será executado na porta 1883 (padrão do MQTT)
```
onde p é a porta utilizada pelo broker.

**Atenção:** quando um cliente é desconectado forçadamente, ele pode deixar os arquivos FIFO no sistema sem serem deletados, causando bloqueio de escrita dos publishers. Por isso, lembre-se de apague os arquivos temporários criados em pastas como "/tmp/MAC0352_mqtt_broker/" ou "/var/tmp/MAC0352_mqtt_broker/", especialmente quando for parar todos os clientes e rodar novamente.

### Restrições
- Suporata apenas QoS = 0
- Não considera muitas das flags de escrita e subscription (ex: autenticação, dup,...)
- Não irá se desconectar dos clientes se o cliente não enviar o PINGREQ independente do Keep Alive
- O packet UNSUB não está sendo considerado
- O broker considera que não haverão falhas de comunicação ou internet: o sistema não é resiliente nem resistente a falhas
- Um subscriber se inscrever em mais de um tópico não é suportado (nem pela utilização de wildcards)
- Um cliente não pode ser publisher e subscriber simultaneamente

### Slides
- [URL](https://docs.google.com/presentation/d/1I2aE9jhjssRu1Ofp3MfY8_dwsTfiIXfTl-sInsvFobM/edit#slide=id.p)

### Autor
- Nome: Luca Assumpção Dillenburg
- NUSP: 11796580
