# Classificação de Condições Ambientais com TinyML no ESP32-S3

## Projeto Final

**Disciplina:** IA Embarcada e Modelos Compactos

## 1. Objetivo

Desenvolver uma aplicação de IA embarcada capaz de classificar condições ambientais a partir de dados de temperatura, umidade e luminosidade.

A inferência é executada localmente em um ESP32-S3 utilizando TensorFlow Lite Micro, sem necessidade de processamento externo para realizar a classificação.

O sistema utiliza duas saídas independentes:

- condição térmica;
- condição de iluminação.

---

## 2. Hardware e ambiente de simulação

O projeto utiliza:

- ESP32-S3 DevKitC-1;
- sensor DHT22;
- sensor LDR;
- ESP-IDF 6.1;
- TensorFlow Lite Micro;
- Wokwi para simulação.

### Ligações utilizadas

#### DHT22

- VCC → 3,3 V;
- GND → GND;
- DATA → GPIO 5.

#### LDR

- VCC → 3,3 V;
- GND → GND;
- saída analógica → GPIO 4.

No ESP32-S3, o GPIO 4 corresponde ao ADC1 Channel 3.

---

## 3. Entradas do modelo

O modelo utiliza três variáveis:

1. temperatura;
2. umidade;
3. luminosidade em valor ADC.

Antes da inferência, as entradas são normalizadas da mesma forma utilizada no treinamento:

```text
temperatura_normalizada = temperatura / 50
umidade_normalizada = umidade / 100
luminosidade_normalizada = ADC / 4095