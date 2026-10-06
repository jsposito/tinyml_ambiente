# Dataset do Projeto

## 1. Objetivo

Este documento registra a estrutura, os critérios de rotulagem, a metodologia de coleta, a validação e o tratamento do dataset utilizado no treinamento do modelo de classificação ambiental.

O dataset é composto por dados de:

- temperatura;
- umidade;
- luminosidade.

Cada registro possui duas saídas independentes:

- condição térmica;
- condição de iluminação.

---

## 2. Arquivo do dataset

O dataset utilizado no treinamento está armazenado em:

`dataset/dataset_ambiente.csv`

Estrutura:

```text
temperatura,umidade,luminosidade,classe_termica,classe_iluminacao
```

As três primeiras colunas correspondem às entradas do modelo e as duas últimas aos rótulos utilizados durante o treinamento.

---

## 3. Variáveis de entrada

### 3.1 Temperatura

- Origem: sensor DHT22.
- Unidade: graus Celsius (°C).

### 3.2 Umidade

- Origem: sensor DHT22.
- Unidade: porcentagem de umidade relativa (%).

### 3.3 Luminosidade

- Origem: sensor LDR.
- Representação: valor bruto do conversor analógico-digital do ESP32-S3.

Durante os testes no Wokwi foi confirmado:

```text
maior iluminação → menor valor ADC
menor iluminação → maior valor ADC
```

---

## 4. Critérios operacionais adotados

Os critérios foram utilizados para construção e avaliação experimental do modelo e não representam norma técnica de conforto ambiental.

### 4.1 Condição térmica

| Classe | Temperatura | Umidade |
|---|---:|---:|
| `ADEQUADO` | 20 a 27 °C | 40 a 70% |
| `QUENTE_E_SECO` | 29 a 36 °C | 20 a 45% |
| `QUENTE_E_UMIDO` | 29 a 36 °C | 70 a 95% |

A faixa entre 27 °C e 29 °C foi excluída da construção do conjunto principal.

Combinações de temperatura e umidade que não pertencem a uma das três regiões também são consideradas fora do domínio experimental.

### 4.2 Condição de iluminação

| Classe | Leitura do LDR |
|---|---:|
| `ILUMINACAO_ADEQUADA` | ADC até 1800 |
| `BAIXA_ILUMINACAO` | ADC a partir de 2200 |

A faixa entre ADC 1800 e 2200 foi tratada como região de transição.

---

## 5. Validação experimental do LDR

A resposta do LDR foi verificada no Wokwi mediante variação gradual da iluminação.

Durante os testes, a leitura ADC variou aproximadamente entre:

```text
baixa luminosidade → ADC próximo de 4063
alta luminosidade  → ADC próximo de 32
```

Também foi observada leitura próxima de ADC 2012, situada na região de transição definida entre 1800 e 2200.

O comportamento observado confirmou a relação inversa entre luminosidade e leitura ADC adotada no projeto.

---

## 6. Independência das saídas

A condição térmica e a condição de iluminação são tratadas de forma independente.

Exemplos:

```text
QUENTE_E_SECO + BAIXA_ILUMINACAO
QUENTE_E_UMIDO + ILUMINACAO_ADEQUADA
```

Essa separação permite representar simultaneamente as duas características do ambiente.

---

## 7. Combinações utilizadas

O dataset contém seis combinações:

1. `ADEQUADO + ILUMINACAO_ADEQUADA`;
2. `ADEQUADO + BAIXA_ILUMINACAO`;
3. `QUENTE_E_SECO + ILUMINACAO_ADEQUADA`;
4. `QUENTE_E_SECO + BAIXA_ILUMINACAO`;
5. `QUENTE_E_UMIDO + ILUMINACAO_ADEQUADA`;
6. `QUENTE_E_UMIDO + BAIXA_ILUMINACAO`.

---

## 8. Quantidade de amostras

O dataset final possui 60 registros.

| Condição térmica | Condição de iluminação | Amostras |
|---|---|---:|
| `ADEQUADO` | `ILUMINACAO_ADEQUADA` | 10 |
| `ADEQUADO` | `BAIXA_ILUMINACAO` | 10 |
| `QUENTE_E_SECO` | `ILUMINACAO_ADEQUADA` | 10 |
| `QUENTE_E_SECO` | `BAIXA_ILUMINACAO` | 10 |
| `QUENTE_E_UMIDO` | `ILUMINACAO_ADEQUADA` | 10 |
| `QUENTE_E_UMIDO` | `BAIXA_ILUMINACAO` | 10 |

Total: **60 amostras**.

---

## 9. Metodologia de coleta

A coleta foi realizada com os sensores simulados no Wokwi.

Fluxo:

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

Os valores foram variados dentro das regiões definidas para cada combinação.

---

## 10. Validação do dataset

A validação foi realizada pelo script:

`training/validar_dataset.py`

Foram verificados:

- quantidade total de registros;
- distribuição das seis combinações;
- duplicatas exatas;
- valores fora das faixas definidas;
- consistência dos rótulos.

Resultado:

```text
Total de amostras: 60
Duplicatas exatas: 0
Erros encontrados: 0
RESULTADO: DATASET VALIDADO COM SUCESSO.
```

---

## 11. Normalização

As entradas são normalizadas antes do treinamento e da inferência:

```text
temperatura_normalizada = temperatura / 50
umidade_normalizada = umidade / 100
luminosidade_normalizada = ADC / 4095
```

O mesmo procedimento foi implementado no firmware do ESP32-S3.

---

## 12. Divisão do dataset

O dataset foi dividido de forma estratificada pelas seis combinações ambientais:

| Conjunto | Quantidade |
|---|---:|
| Treinamento | 36 |
| Validação | 12 |
| Teste | 12 |

Total: **60 amostras**.

---

## 13. Zonas e condições fora do domínio

### Região térmica de transição

```text
temperatura entre 27 °C e 29 °C
```

Além dessa faixa, combinações de temperatura e umidade que não pertencem a nenhuma das três regiões térmicas também são consideradas fora do domínio experimental.

### Região de transição da iluminação

```text
ADC entre 1800 e 2200
```

No firmware final, essas verificações evitam que uma classificação seja apresentada como válida quando a entrada estiver fora do domínio experimental.

---

## 14. Versionamento

O dataset utilizado no treinamento final está armazenado em:

`dataset/dataset_ambiente.csv`

As alterações na estrutura, nos critérios de rotulagem e no processo de validação foram registradas no Git.

---

## 15. Evidências preservadas

O projeto mantém como evidências:

- arquivo CSV utilizado no treinamento;
- script de validação;
- resultados da validação do dataset;
- registros do treinamento;
- métricas do modelo;
- modelo TensorFlow Lite INT8;
- testes realizados no Wokwi;
- resultados da inferência embarcada.

As evidências de execução estão organizadas em:

`evidencias/`

---

## 16. Limitações

- O dataset possui finalidade experimental.
- As 60 amostras foram produzidas em ambiente simulado e controlado.
- Os critérios adotados não representam norma técnica de conforto ambiental.
- Os resultados do conjunto de teste não garantem generalização para ambientes reais ou condições não representadas no dataset.

---

## 17. Situação final do dataset

A construção do dataset foi concluída com:

```text
60 registros
6 combinações ambientais
10 amostras por combinação
0 duplicatas exatas
0 erros de validação
```

O dataset validado foi utilizado no treinamento e na geração do modelo posteriormente embarcado no ESP32-S3.
