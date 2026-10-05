# Classificação de Condições Ambientais com TinyML no ESP32-S3

## Projeto Final

Disciplina: IA Embarcada e Modelos Compactos

## Objetivo

Desenvolver uma aplicação de IA embarcada capaz de classificar condições ambientais a partir de dados de temperatura, umidade e luminosidade.

A inferência será executada localmente no ESP32-S3 utilizando TensorFlow Lite Micro.

## Sensores

O projeto utilizará:

- DHT22
  - temperatura
  - umidade
- LDR
  - luminosidade

## Entradas do modelo

O modelo utilizará três variáveis de entrada:

1. temperatura;
2. umidade;
3. luminosidade.

## Classes previstas

Inicialmente, serão utilizadas quatro classes:

- ADEQUADO;
- QUENTE_E_SECO;
- QUENTE_E_UMIDO;
- BAIXA_ILUMINACAO.

Os critérios definitivos de cada classe serão definidos antes da coleta do dataset.

## Modelo previsto

Será utilizada uma rede neural do tipo MLP (Multilayer Perceptron), com arquitetura inicial:

- 3 entradas;
- camada Dense com 12 neurônios e ativação ReLU;
- camada Dense com 8 neurônios e ativação ReLU;
- camada de saída com 4 neurônios.

Após o treinamento, o modelo será convertido para TensorFlow Lite e quantizado para INT8.

## Arquitetura geral

DHT22 e LDR fornecem os dados ambientais ao ESP32-S3.

O dispositivo realizará:

1. leitura dos sensores;
2. pré-processamento dos dados;
3. normalização;
4. quantização da entrada;
5. inferência com TensorFlow Lite Micro;
6. identificação da classe prevista;
7. disponibilização do resultado para visualização.

## Dashboard

Será desenvolvido um painel web automático e somente para visualização.

O dashboard não realizará nenhuma classificação e não permitirá alteração manual dos parâmetros utilizados pela IA.

A decisão será realizada exclusivamente pelo modelo embarcado no ESP32-S3.

O painel deverá apresentar:

- temperatura;
- umidade;
- luminosidade;
- classificação atual;
- confiança ou scores do modelo;
- horário da última leitura;
- tempo de inferência, quando aplicável.

## Ambientes de desenvolvimento

### Visual Studio Code

Será utilizado como ambiente principal para:

- ESP-IDF;
- firmware;
- Wokwi;
- Git e GitHub;
- documentação;
- dashboard;
- organização geral do projeto.

### Google Colab

Será utilizado para:

- análise do dataset;
- preparação dos dados;
- treinamento do modelo;
- avaliação;
- geração de gráficos;
- matriz de confusão;
- conversão para TensorFlow Lite;
- quantização INT8.

## Estrutura inicial

```text
tinyml_ambiente/
├── dashboard/
├── dataset/
├── docs/
├── evidencias/
├── firmware/
├── model/
├── training/
├── .gitignore
└── README.md