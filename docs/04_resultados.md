# Resultados e validação do projeto

## 1. Visão geral

O projeto implementa um sistema de classificação ambiental executado diretamente em um ESP32-S3, utilizando dados de temperatura, umidade e luminosidade.

Os dados são obtidos por:

- DHT22: temperatura e umidade;
- LDR: luminosidade por leitura analógica no ADC.

O modelo possui duas saídas independentes:

### Condição térmica

- `ADEQUADO`
- `QUENTE_E_SECO`
- `QUENTE_E_UMIDO`

### Condição de iluminação

- `ILUMINACAO_ADEQUADA`
- `BAIXA_ILUMINACAO`

A inferência é executada localmente no ESP32-S3 por meio do TensorFlow Lite Micro.

---

## 2. Dataset

O dataset experimental utilizado no projeto possui 60 amostras.

Foram utilizadas seis combinações entre as classes térmicas e de iluminação, com 10 amostras para cada combinação.

A validação do dataset apresentou:

- 60 amostras;
- 0 duplicatas exatas;
- 0 erros de classificação segundo os critérios operacionais definidos;
- 10 amostras para cada uma das seis combinações.

---

## 3. Treinamento do modelo

O modelo utilizado foi uma rede neural do tipo MLP com a seguinte arquitetura:

- 3 entradas;
- camada Dense com 12 neurônios e ReLU;
- camada Dense com 8 neurônios e ReLU;
- saída térmica com 3 classes;
- saída de iluminação com 2 classes.

O modelo possui 197 parâmetros treináveis.

A divisão do dataset foi:

- treino: 36 amostras;
- validação: 12 amostras;
- teste: 12 amostras.

### Resultados do modelo antes da quantização

Acurácia térmica:

```text
1.0000