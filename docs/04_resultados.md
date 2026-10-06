# Resultados e Validação do Projeto

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

O dataset experimental possui 60 amostras.

Foram utilizadas seis combinações entre as classes térmicas e de iluminação, com 10 amostras para cada combinação.

A validação do dataset apresentou:

- 60 amostras;
- 0 duplicatas exatas;
- 0 erros de classificação segundo os critérios operacionais definidos;
- 10 amostras para cada uma das seis combinações.

---

## 3. Treinamento do modelo

O modelo utilizado foi uma rede neural do tipo MLP com:

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

### Resultados antes da quantização

Acurácia térmica:

```text
1.0000
```

Acurácia de iluminação:

```text
1.0000
```

Matriz de confusão térmica:

```text
[[4 0 0]
 [0 4 0]
 [0 0 4]]
```

Matriz de confusão de iluminação:

```text
[[6 0]
 [0 6]]
```

Os resultados correspondem ao conjunto de teste utilizado no experimento e não devem ser interpretados como medida de generalização para ambientes ou condições não representados no dataset.

---

## 4. Conversão e quantização INT8

O modelo foi convertido para TensorFlow Lite em duas versões:

- FLOAT: 3148 bytes;
- INT8: 3584 bytes.

A versão embarcada utiliza quantização completa INT8.

Neste modelo pequeno, o arquivo INT8 ficou ligeiramente maior que o FLOAT devido ao overhead do formato quantizado. A quantização foi utilizada principalmente para permitir execução eficiente com operações inteiras no microcontrolador.

### Entrada

```text
tipo: INT8
shape: [1, 3]
scale: 0.0037664296105504036
zero_point: -128
```

### Saída de iluminação

```text
shape: [1, 2]
scale: 0.08659937232732773
zero_point: -36
```

### Saída térmica

```text
shape: [1, 3]
scale: 0.11728253960609436
zero_point: 29
```

O modelo TFLite INT8 foi novamente validado no conjunto de teste.

Resultados:

```text
Acurácia térmica: 1.0
Acurácia de iluminação: 1.0
```

Matriz térmica:

```text
[[4 0 0]
 [0 4 0]
 [0 0 4]]
```

Matriz de iluminação:

```text
[[6 0]
 [0 6]]
```

Os operadores identificados no modelo foram quatro operações `FULLY_CONNECTED`.

---

## 5. Implantação no ESP32-S3

O modelo `modelo_ambiente_int8.tflite` foi incorporado ao firmware e executado utilizando TensorFlow Lite Micro.

Durante a inicialização foram confirmados:

```text
TinyML inicializado com sucesso
Modelo INT8: 3584 bytes
Tensor Arena: 20480 bytes
```

O firmware foi compilado para ESP32-S3 utilizando ESP-IDF 6.1.

A versão com inferência contínua apresentou aproximadamente 78% de espaço livre na partição de aplicação.

---

## 6. Teste controlado

Antes da execução contínua foi realizada uma inferência com uma amostra conhecida:

```text
Temperatura: 36.0 °C
Umidade: 95.0 %
LDR ADC: 4046
```

Resultado obtido:

```text
Classe termica: QUENTE_E_UMIDO
Classe iluminacao: BAIXA_ILUMINACAO
RESULTADO: TESTE TINYML APROVADO
```

O teste confirmou o funcionamento da cadeia:

```text
modelo treinado
→ quantização INT8
→ incorporação ao firmware
→ TensorFlow Lite Micro
→ inferência no ESP32-S3
```

---

## 7. Validação da inferência contínua

Foram verificadas as seis combinações previstas no dataset.

| Teste | Temperatura | Umidade | LDR ADC | Classe térmica obtida | Classe de iluminação obtida | Resultado |
|---|---:|---:|---:|---|---|---|
| 1/6 | 36.0 °C | 95.0% | 3969 | `QUENTE_E_UMIDO` | `BAIXA_ILUMINACAO` | Aprovado |
| 2/6 | 36.0 °C | 95.0% | 1407 | `QUENTE_E_UMIDO` | `ILUMINACAO_ADEQUADA` | Aprovado |
| 3/6 | 33.0 °C | 30.0% | 1407 | `QUENTE_E_SECO` | `ILUMINACAO_ADEQUADA` | Aprovado |
| 4/6 | 33.0 °C | 30.0% | 3295 a 4058 | `QUENTE_E_SECO` | `BAIXA_ILUMINACAO` | Aprovado |
| 5/6 | 24.0 °C | 50.0% | 3295 | `ADEQUADO` | `BAIXA_ILUMINACAO` | Aprovado |
| 6/6 | 24.0 °C | 50.0% | 1074 ou inferior | `ADEQUADO` | `ILUMINACAO_ADEQUADA` | Aprovado |

As seis combinações previstas foram corretamente classificadas durante a simulação no Wokwi.

---

## 8. Validação de amostras fora do domínio experimental

O firmware também verifica se a leitura pertence ao domínio utilizado na construção do dataset.

Exemplo observado:

```text
Temperatura: 19.7 °C
Umidade: 66.5 %
LDR ADC: 3969
```

Resultado:

```text
Amostra fora do dominio experimental
Condicao termica: FORA_DO_DOMINIO
Inferencia TinyML nao executada
```

Também foi validada a zona de transição da iluminação. Em uma leitura com ADC 2111, o firmware identificou:

```text
Iluminacao: ZONA_DE_TRANSICAO
Inferencia TinyML nao executada
```

Esse comportamento evita que o sistema apresente como válida uma classificação para uma condição fora das regiões experimentais estabelecidas.

---

## 9. Resultado final

A implementação comprovou o fluxo completo:

```text
DHT22 + LDR
     ↓
validação do domínio experimental
     ↓
normalização das entradas
     ↓
quantização INT8
     ↓
TensorFlow Lite Micro
     ↓
modelo embarcado
     ↓
classificação térmica
+
classificação de iluminação
```

O modelo foi treinado, convertido, quantizado, incorporado ao firmware e executado no ESP32-S3 em ambiente simulado no Wokwi.

Todas as seis combinações previstas no experimento foram classificadas corretamente durante os testes realizados.
