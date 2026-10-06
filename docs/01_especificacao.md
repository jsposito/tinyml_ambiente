# Especificação do Projeto

## 1. Identificação

**Título:** Classificação de Condições Ambientais com TinyML no ESP32-S3

**Disciplina:** IA Embarcada e Modelos Compactos

---

## 2. Objetivo

Desenvolver uma aplicação de IA embarcada capaz de analisar temperatura, umidade e luminosidade e classificar condições ambientais diretamente no ESP32-S3.

A inferência é executada localmente por meio do TensorFlow Lite Micro.

---

## 3. Plataforma

O projeto utiliza:

- ESP32-S3 DevKitC-1;
- ESP-IDF 6.1;
- TensorFlow Lite Micro;
- Wokwi para simulação.

---

## 4. Sensores

### 4.1 DHT22

Variáveis coletadas:

- temperatura;
- umidade.

Ligação utilizada:

- DATA → GPIO 5.

### 4.2 LDR

Variável coletada:

- luminosidade por leitura analógica.

Ligação utilizada:

- saída analógica → GPIO 4.

No ESP32-S3, o GPIO 4 corresponde ao ADC1 Channel 3.

---

## 5. Entradas do modelo

O modelo utiliza três variáveis de entrada:

1. temperatura;
2. umidade;
3. luminosidade em valor ADC.

Os dados utilizados no ESP32-S3 passam pelo mesmo processo de preparação aplicado durante o treinamento.

### 5.1 Normalização

A normalização utilizada é:

```text
temperatura_normalizada = temperatura / 50
umidade_normalizada = umidade / 100
luminosidade_normalizada = ADC / 4095