# Dataset do Projeto

## 1. Objetivo

Este documento registra a estrutura, os critérios de rotulagem, a metodologia de coleta e o tratamento do dataset utilizado no treinamento do modelo de classificação ambiental.

O dataset será formado por dados de:

- temperatura;
- umidade;
- luminosidade.

Cada registro possuirá duas saídas independentes:

- condição térmica;
- condição de iluminação.

---

## 2. Estrutura do dataset

O arquivo principal será armazenado em:

`dataset/dataset_ambiente.csv`

Estrutura prevista:

```text
temperatura,umidade,luminosidade,classe_termica,classe_iluminacao
```

Exemplo de registro:

```text
23.5,52.0,700,ADEQUADO,ILUMINACAO_ADEQUADA
```

As três primeiras colunas correspondem às entradas do modelo.

As duas últimas correspondem aos rótulos utilizados durante o treinamento.

---

## 3. Variáveis de entrada

### 3.1 Temperatura

Origem:

- sensor DHT22.

Unidade:

- graus Celsius (°C).

### 3.2 Umidade

Origem:

- sensor DHT22.

Unidade:

- porcentagem de umidade relativa (%).

### 3.3 Luminosidade

Origem:

- sensor LDR.

Representação:

- valor bruto obtido pelo conversor analógico-digital do ESP32-S3.

No comportamento esperado do sensor:

```text
maior iluminação -> menor valor ADC
menor iluminação -> maior valor ADC
```

A faixa efetiva do LDR será confirmada durante os testes no Wokwi antes da coleta definitiva.

---

## 4. Critérios operacionais adotados

Os critérios abaixo serão utilizados para construção e avaliação experimental do modelo.

Eles não representam norma técnica de conforto ambiental.

### 4.1 Condição térmica

O primeiro rótulo representa a condição térmica.

| Classe | Temperatura | Umidade |
|---|---:|---:|
| ADEQUADO | 20 a 27 °C | 40 a 70% |
| QUENTE_E_SECO | 29 a 36 °C | 20 a 45% |
| QUENTE_E_UMIDO | 29 a 36 °C | 70 a 95% |

A faixa de temperatura entre 27 °C e 29 °C será inicialmente considerada região de transição.

Combinações de temperatura e umidade que não estejam dentro das faixas definidas não serão utilizadas na primeira versão do dataset.

### 4.2 Condição de iluminação

O segundo rótulo representa a condição de iluminação.

| Classe | Leitura do LDR |
|---|---:|
| ILUMINACAO_ADEQUADA | ADC até 1800 |
| BAIXA_ILUMINACAO | ADC a partir de 2200 |

A faixa entre ADC 1800 e 2200 será considerada região de transição.

Esses limites deverão ser confirmados após a validação do LDR no Wokwi.

---

## 5. Independência das saídas

A condição térmica e a condição de iluminação serão tratadas de forma independente.

Assim, uma mesma amostra poderá apresentar, por exemplo:

```text
QUENTE_E_SECO + BAIXA_ILUMINACAO
```

ou:

```text
QUENTE_E_UMIDO + ILUMINACAO_ADEQUADA
```

Essa separação permite representar simultaneamente diferentes características do ambiente.

---

## 6. Combinações previstas

A primeira versão do dataset utilizará seis combinações:

1. ADEQUADO + ILUMINACAO_ADEQUADA;
2. ADEQUADO + BAIXA_ILUMINACAO;
3. QUENTE_E_SECO + ILUMINACAO_ADEQUADA;
4. QUENTE_E_SECO + BAIXA_ILUMINACAO;
5. QUENTE_E_UMIDO + ILUMINACAO_ADEQUADA;
6. QUENTE_E_UMIDO + BAIXA_ILUMINACAO.

---

## 7. Quantidade inicial de amostras

A primeira versão terá 60 registros.

Distribuição prevista:

| Condição térmica | Condição de iluminação | Amostras |
|---|---|---:|
| ADEQUADO | ILUMINACAO_ADEQUADA | 10 |
| ADEQUADO | BAIXA_ILUMINACAO | 10 |
| QUENTE_E_SECO | ILUMINACAO_ADEQUADA | 10 |
| QUENTE_E_SECO | BAIXA_ILUMINACAO | 10 |
| QUENTE_E_UMIDO | ILUMINACAO_ADEQUADA | 10 |
| QUENTE_E_UMIDO | BAIXA_ILUMINACAO | 10 |

Total:

**60 amostras.**

A quantidade poderá ser ampliada caso a análise dos dados ou os resultados do treinamento indiquem necessidade.

---

## 8. Metodologia de coleta

A coleta inicial será realizada com os sensores simulados no Wokwi.

Fluxo previsto:

```text
configuração dos sensores
        ↓
leitura pelo ESP32-S3
        ↓
registro de temperatura, umidade e luminosidade
        ↓
atribuição dos dois rótulos
        ↓
armazenamento no arquivo CSV
```

Os valores deverão variar dentro das faixas estabelecidas para cada condição.

Não serão utilizados apenas valores repetidos.

---

## 9. Zonas de transição

As regiões de transição não serão utilizadas inicialmente para formar o conjunto principal de treinamento.

Serão reservadas para avaliar o comportamento do modelo próximo às fronteiras.

Regiões previstas:

### Temperatura

```text
27 °C a 29 °C
```

### Luminosidade

```text
ADC entre 1800 e 2200
```

Os resultados nessas regiões serão analisados após o treinamento.

---

## 10. Validação do dataset

Antes do treinamento serão verificados:

- quantidade total de registros;
- distribuição das classes;
- valores mínimos e máximos;
- registros duplicados;
- valores ausentes;
- valores fora das faixas previstas;
- possíveis anomalias de coleta.

Também serão analisadas as relações entre:

- temperatura e umidade;
- temperatura e luminosidade;
- umidade e luminosidade.

---

## 11. Normalização

As três entradas serão normalizadas antes do treinamento.

O método utilizado será registrado no notebook de treinamento.

O mesmo procedimento deverá ser reproduzido no firmware do ESP32-S3.

Isso garante que os dados utilizados durante a inferência tenham a mesma representação utilizada durante o treinamento.

---

## 12. Divisão do dataset

Os registros serão separados em conjuntos de:

- treinamento;
- validação;
- teste.

A proporção final será definida durante a etapa de treinamento.

A divisão deverá preservar, sempre que possível, a distribuição das seis combinações ambientais.

---

## 13. Versionamento

Os arquivos do dataset serão armazenados na pasta:

`dataset/`

Versões intermediárias poderão utilizar nomes como:

```text
dataset_ambiente_v01.csv
dataset_ambiente_v02.csv
```

A versão utilizada no treinamento final será identificada no repositório.

Alterações nos critérios de rotulagem ou na estrutura do dataset deverão ser registradas no Git.

---

## 14. Evidências

Durante a construção do dataset deverão ser preservadas:

- capturas do Wokwi;
- leituras do terminal;
- arquivo CSV;
- distribuição das classes;
- gráficos de análise;
- versão utilizada no treinamento final.

---

## 15. Limitações

Os critérios adotados possuem finalidade experimental.

O sistema não será apresentado como instrumento certificado de avaliação de conforto ambiental.

Os resultados serão utilizados para demonstrar o desenvolvimento completo de uma aplicação TinyML, desde a coleta de dados até a inferência embarcada.

---

## 16. Próxima etapa

Montar e validar no Wokwi:

- ESP32-S3;
- DHT22;
- LDR.

Antes da coleta definitiva serão confirmadas:

- leitura de temperatura;
- leitura de umidade;
- leitura ADC do LDR;
- comportamento das faixas de luminosidade.