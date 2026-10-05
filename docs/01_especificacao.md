# Especificação do Projeto

## 1. Identificação

**Título provisório:** Classificação de Condições Ambientais com TinyML no ESP32-S3

**Disciplina:** IA Embarcada e Modelos Compactos

## 2. Objetivo geral

Desenvolver uma aplicação de IA embarcada capaz de classificar condições ambientais a partir de dados de temperatura, umidade e luminosidade.

A inferência deverá ser executada localmente no ESP32-S3, utilizando TensorFlow Lite Micro.

## 3. Objetivos específicos

O projeto deverá:

1. realizar a leitura dos sensores ambientais;
2. armazenar ou organizar os dados coletados para formação do dataset;
3. preparar e normalizar os dados;
4. treinar um modelo de classificação;
5. avaliar o desempenho do modelo;
6. converter o modelo para TensorFlow Lite;
7. realizar quantização INT8;
8. embarcar o modelo no ESP32-S3;
9. executar a inferência diretamente no dispositivo;
10. apresentar os resultados em um dashboard automático e somente para visualização.

## 4. Hardware e ambiente

### 4.1 Dispositivo principal

- ESP32-S3 DevKitC-1

### 4.2 Sensores

#### DHT22

Variáveis utilizadas:

- temperatura;
- umidade.

#### LDR

Variável utilizada:

- luminosidade.

## 5. Entradas do modelo

O modelo receberá três características:

1. temperatura;
2. umidade;
3. luminosidade.

Os valores serão submetidos ao mesmo processo de preparação utilizado durante o treinamento.

## 6. Saídas do modelo

A classificação inicial prevê quatro classes:

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`;
- `BAIXA_ILUMINACAO`.

Os critérios quantitativos e as regiões de transição de cada classe ainda serão definidos antes da construção definitiva do dataset.

## 7. Modelo previsto

A arquitetura inicial será uma rede neural do tipo MLP — Multilayer Perceptron.

Estrutura preliminar:

- entrada com 3 características;
- camada Dense com 12 neurônios e ativação ReLU;
- camada Dense com 8 neurônios e ativação ReLU;
- camada de saída com 4 neurônios.

Essa arquitetura é apenas o ponto de partida e poderá ser modificada após a análise dos dados e dos primeiros experimentos.

## 8. Pipeline prevista

A aplicação deverá seguir o fluxo:

```text
DHT22 + LDR
     ↓
leitura dos sensores
     ↓
pré-processamento
     ↓
normalização
     ↓
quantização da entrada
     ↓
modelo TinyML
     ↓
inferência
     ↓
classe prevista
     ↓
dashboard