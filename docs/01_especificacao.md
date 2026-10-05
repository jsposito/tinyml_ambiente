# Especificação do Projeto

## 1. Identificação

**Título provisório:** Classificação de Condições Ambientais com TinyML no ESP32-S3

**Disciplina:** IA Embarcada e Modelos Compactos

---

## 2. Objetivo

Desenvolver uma aplicação de IA embarcada capaz de analisar temperatura, umidade e luminosidade e classificar as condições ambientais diretamente no ESP32-S3.

A inferência será executada localmente utilizando TensorFlow Lite Micro.

---

## 3. Sensores

### DHT22

Variáveis coletadas:

- temperatura;
- umidade.

### LDR

Variável coletada:

- luminosidade.

---

## 4. Entradas do modelo

O modelo utilizará três variáveis de entrada:

1. temperatura;
2. umidade;
3. luminosidade.

Os dados utilizados no ESP32-S3 deverão passar pelo mesmo processo de preparação aplicado durante o treinamento.

---

## 5. Saídas do modelo

O modelo produzirá duas classificações independentes.

### 5.1 Condição térmica

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`.

### 5.2 Condição de iluminação

- `ILUMINACAO_ADEQUADA`;
- `BAIXA_ILUMINACAO`.

A separação das saídas evita ambiguidades.

Exemplo:

```text
Condição térmica: QUENTE_E_SECO
Iluminação: BAIXA_ILUMINACAO