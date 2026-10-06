# Registro de Decisões Técnicas

Este documento registra as principais decisões adotadas durante o desenvolvimento do projeto, incluindo alterações realizadas em relação à concepção inicial.

---

## 1. Tema do projeto

### Decisão

Desenvolver uma aplicação de:

**Classificação de Condições Ambientais com TinyML no ESP32-S3.**

### Justificativa

O tema permite integrar em um único projeto:

- aquisição de dados por sensores;
- construção e validação de dataset;
- treinamento de modelo;
- conversão para TensorFlow Lite;
- quantização INT8;
- incorporação do modelo ao firmware;
- inferência embarcada;
- validação em ambiente simulado.

O Wokwi foi utilizado para simular o ESP32-S3 e os sensores durante o desenvolvimento e os testes.

---

## 2. Sensores utilizados

### Decisão

Utilizar:

- DHT22:
  - temperatura;
  - umidade;
- LDR:
  - luminosidade por leitura analógica.

### Alternativas avaliadas

Foram considerados sensores adicionais, como:

- PIR;
- MQ2.

### Decisão final

Não incluir sensores adicionais.

### Justificativa

Temperatura, umidade e luminosidade já permitem construir um problema multivariável suficiente para demonstrar o fluxo completo de TinyML.

A inclusão de sensores adicionais aumentaria a complexidade de coleta, treinamento e validação sem necessidade para o objetivo do projeto.

---

## 3. Plataforma embarcada

### Decisão

Utilizar:

- ESP32-S3 DevKitC-1;
- ESP-IDF 6.1;
- TensorFlow Lite Micro.

### Justificativa

O ESP32-S3 possui recursos suficientes para executar o modelo quantizado localmente e é suportado pelo ambiente de desenvolvimento utilizado no projeto.

---

## 4. Variáveis de entrada

### Decisão

Utilizar três entradas:

1. temperatura;
2. umidade;
3. luminosidade em valor ADC.

### Normalização adotada

A mesma transformação é aplicada durante o treinamento e no firmware:

```text
temperatura_normalizada = temperatura / 50
umidade_normalizada = umidade / 100
luminosidade_normalizada = ADC / 4095