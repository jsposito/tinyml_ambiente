# Registro de Decisões Técnicas

Este documento registra as principais decisões tomadas durante o desenvolvimento do projeto, incluindo alternativas consideradas e justificativas.

---

## 1. Tema do projeto

### Decisão

Desenvolver uma aplicação de:

**Classificação de Condições Ambientais com TinyML no ESP32-S3.**

### Justificativa

O tema permite integrar sensores, construção de dataset, treinamento de modelo, conversão, quantização e inferência embarcada em um único fluxo.

Também permite demonstrar o funcionamento do modelo em tempo real por meio do Wokwi.

---

## 2. Sensores utilizados

### Decisão

Utilizar:

- DHT22:
  - temperatura;
  - umidade;
- LDR:
  - luminosidade.

### Alternativas avaliadas

Foram considerados sensores adicionais, como:

- PIR;
- MQ2.

### Decisão final

Não incluir sensores adicionais nesta etapa.

### Justificativa

As três variáveis já permitem construir um problema multivariável adequado ao projeto.

A inclusão de novos sensores aumentaria a complexidade de coleta, treinamento e demonstração sem necessidade imediata.

---

## 3. Variáveis de entrada do modelo

### Decisão

O modelo utilizará três entradas:

1. temperatura;
2. umidade;
3. luminosidade.

Essas variáveis serão pré-processadas de forma consistente entre treinamento e execução embarcada.

---

## 4. Classes ambientais

### Decisão preliminar

Foram propostas quatro classes:

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`;
- `BAIXA_ILUMINACAO`.

### Observação

As classes ainda são provisórias.

Os critérios quantitativos e as zonas de transição deverão ser definidos antes da coleta definitiva do dataset.

---

## 5. Uso de Inteligência Artificial

### Decisão

A classificação será realizada por um modelo treinado e executado no ESP32-S3.

### Não será utilizado

O firmware não utilizará regras fixas do tipo:

```cpp
if (temperatura > 30 && umidade < 40) {
    classe = QUENTE_E_SECO;
}