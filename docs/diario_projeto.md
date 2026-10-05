# Diário do Projeto

## Projeto Final — IA Embarcada e Modelos Compactos

Este documento registra cronologicamente o desenvolvimento do projeto, incluindo decisões técnicas, alternativas avaliadas, alterações de escopo, problemas encontrados e resultados obtidos.

---

## 05/10/2026 — Início do projeto

### Definição do tema

Foi definido como tema do projeto:

**Classificação de Condições Ambientais com TinyML no ESP32-S3.**

A proposta é desenvolver uma aplicação de IA embarcada capaz de utilizar dados ambientais coletados por sensores e realizar a classificação diretamente no ESP32-S3.

### Sensores definidos

Foram escolhidos:

- DHT22:
  - temperatura;
  - umidade.

- LDR:
  - luminosidade.

O modelo terá, portanto, três variáveis de entrada:

1. temperatura;
2. umidade;
3. luminosidade.

### Sensores adicionais avaliados

Foi analisada a possibilidade de inclusão de outros sensores, como PIR e MQ2.

Decidiu-se não acrescentá-los neste momento.

A escolha foi manter o projeto concentrado no DHT22 e no LDR, evitando aumento desnecessário da complexidade e preservando um conjunto de três características suficientes para o desenvolvimento de um modelo multivariável.

### Classes inicialmente propostas

Foram definidas, provisoriamente, quatro classes:

- ADEQUADO;
- QUENTE_E_SECO;
- QUENTE_E_UMIDO;
- BAIXA_ILUMINACAO.

Os limites e critérios utilizados para formação dessas classes ainda serão estudados antes da coleta definitiva do dataset.

### Uso de Inteligência Artificial

A classificação não será realizada por regras fixas no firmware.

O ESP32-S3 deverá:

1. realizar a leitura dos sensores;
2. preparar os dados de entrada;
3. executar o modelo embarcado;
4. obter as saídas do modelo;
5. identificar a classe prevista.

A decisão ambiental será produzida pelo modelo treinado.

### Modelo inicialmente previsto

Foi escolhida como arquitetura inicial uma rede neural do tipo MLP (Multilayer Perceptron).

Arquitetura preliminar:

- 3 entradas;
- Dense com 12 neurônios e ReLU;
- Dense com 8 neurônios e ReLU;
- 4 saídas.

A arquitetura poderá ser revista após a análise do dataset e os primeiros experimentos de treinamento.

### Conversão e embarque

O fluxo inicialmente previsto para o modelo é:

TensorFlow/Keras  
→ TensorFlow Lite  
→ quantização INT8  
→ TensorFlow Lite Micro  
→ ESP32-S3.

### Ambientes de desenvolvimento

Foi definida a seguinte divisão:

#### Visual Studio Code

Ambiente principal do projeto para:

- Git;
- GitHub;
- documentação;
- ESP-IDF;
- firmware;
- Wokwi;
- dashboard;
- organização dos arquivos.

#### Google Colab

Ambiente destinado a:

- análise do dataset;
- preparação dos dados;
- treinamento;
- validação;
- métricas;
- gráficos;
- matriz de confusão;
- conversão do modelo;
- quantização INT8.

O notebook produzido no Colab será posteriormente armazenado no repositório.

### Dashboard

Foi definido que será desenvolvido um painel web automático e somente para visualização.

O dashboard não deverá:

- receber manualmente os parâmetros usados para classificação;
- possuir regras de decisão;
- executar o modelo de IA.

Sua função será exclusivamente apresentar os dados e os resultados gerados pelo ESP32-S3.

Informações inicialmente previstas:

- temperatura;
- umidade;
- luminosidade;
- classe prevista;
- confiança ou scores das classes;
- horário da última leitura;
- tempo de inferência.

### Demonstração prevista

Durante a apresentação, os sensores serão alterados no Wokwi.

O fluxo deverá ser observado automaticamente:

Sensor  
→ ESP32-S3  
→ pré-processamento  
→ modelo TinyML  
→ inferência  
→ dashboard.

A intenção é demonstrar que a alteração da condição ambiental provoca uma nova inferência sem intervenção manual no painel.

### Organização do projeto

Foi criada a pasta:

`tinyml_ambiente`

Estrutura inicial:

- dashboard/
- dataset/
- docs/
- evidencias/
- firmware/
- model/
- training/

Também foram criados:

- `.gitignore`;
- `README.md`.

### Git

O repositório Git foi inicializado localmente.

Branch principal:

`main`

Primeiro commit:

`d7a4cb9 - docs: define escopo inicial do projeto`

### Ambiente validado

Foram verificados:

- ESP-IDF v6.1;
- Git 2.53.0.windows.2;
- Visual Studio Code;
- terminal ESP-IDF com ambiente virtual ativo.

### Próxima etapa

Definir formalmente os critérios das quatro classes ambientais e a metodologia para construção do dataset antes de iniciar o treinamento.