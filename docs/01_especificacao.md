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
```

Após a normalização, os valores são quantizados para INT8 antes da inferência.

---

## 6. Saídas do modelo

O modelo produz duas classificações independentes.

### 6.1 Condição térmica

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`.

### 6.2 Condição de iluminação

- `ILUMINACAO_ADEQUADA`;
- `BAIXA_ILUMINACAO`.

A separação das saídas permite representar simultaneamente a condição térmica e a condição de iluminação.

Exemplo:

```text
Condição térmica: QUENTE_E_SECO
Iluminação: BAIXA_ILUMINACAO
```

---

## 7. Critérios operacionais

Os critérios foram definidos para construção e avaliação experimental do modelo e não representam norma técnica de conforto ambiental.

### 7.1 Condição térmica

| Classe | Temperatura | Umidade |
|---|---:|---:|
| `ADEQUADO` | 20 a 27 °C | 40 a 70% |
| `QUENTE_E_SECO` | 29 a 36 °C | 20 a 45% |
| `QUENTE_E_UMIDO` | 29 a 36 °C | 70 a 95% |

A faixa entre 27 e 29 °C não foi utilizada para classificação térmica.

### 7.2 Condição de iluminação

| Classe | Valor ADC |
|---|---:|
| `ILUMINACAO_ADEQUADA` | até 1800 |
| `BAIXA_ILUMINACAO` | a partir de 2200 |

A faixa entre 1800 e 2200 foi tratada como região de transição.

No sensor utilizado no Wokwi:

```text
mais luz  → menor valor ADC
menos luz → maior valor ADC
```

---

## 8. Arquitetura do modelo

O modelo é uma rede neural do tipo MLP com:

- 3 entradas;
- camada Dense com 12 neurônios e ativação ReLU;
- camada Dense com 8 neurônios e ativação ReLU;
- saída térmica com 3 neurônios;
- saída de iluminação com 2 neurônios.

O modelo possui 197 parâmetros treináveis.

Representação simplificada:

```text
temperatura ─┐
umidade ─────┼──> Dense 12 ──> Dense 8 ──┬──> condição térmica
luminosidade ┘                             │
                                          └──> condição de iluminação
```

---

## 9. Modelo embarcado

O modelo utilizado no firmware é:

```text
modelo_ambiente_int8.tflite
```

Características:

- formato TensorFlow Lite;
- entrada INT8;
- duas saídas INT8;
- tamanho: 3584 bytes.

Os operadores utilizados pelo modelo são do tipo:

```text
FULLY_CONNECTED
```

A quantização INT8 foi adotada para viabilizar execução eficiente com operações inteiras no microcontrolador. Como a rede é muito pequena, o arquivo INT8 ficou ligeiramente maior que a versão FLOAT devido ao overhead do formato quantizado.

---

## 10. Fluxo de execução

O fluxo implementado no ESP32-S3 é:

```text
DHT22 + LDR
     ↓
leitura dos sensores
     ↓
validação do domínio experimental
     ↓
normalização
     ↓
quantização INT8
     ↓
TensorFlow Lite Micro
     ↓
inferência
     ↓
classificação térmica
+
classificação de iluminação
```

---

## 11. Tratamento de condições fora do domínio

O firmware verifica se os valores de entrada pertencem às regiões experimentais utilizadas na construção do dataset.

Quando a condição térmica ou a iluminação está fora das regiões definidas, a inferência não é apresentada como classificação válida.

Exemplo:

```text
Amostra fora do dominio experimental
Condicao termica: FORA_DO_DOMINIO
Inferencia TinyML nao executada
```

---

## 12. Resultado esperado da aplicação

A aplicação deve:

1. adquirir temperatura e umidade do DHT22;
2. adquirir o valor analógico do LDR;
3. verificar se a amostra pertence ao domínio experimental;
4. normalizar as três entradas;
5. quantizar as entradas para INT8;
6. executar a inferência no ESP32-S3;
7. identificar a condição térmica;
8. identificar a condição de iluminação;
9. apresentar os resultados no monitor serial.
