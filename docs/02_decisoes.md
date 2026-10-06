# Registro de Decisões Técnicas

Este documento registra as principais decisões adotadas durante o desenvolvimento do projeto, incluindo alterações realizadas em relação à concepção inicial.

---

## 1. Tema do projeto

### Decisão

Desenvolver uma aplicação de:

**Classificação de Condições Ambientais com TinyML no ESP32-S3.**

### Justificativa

O tema permite integrar em um único projeto:

- aquisição de dados por sensores;
- construção e validação de dataset;
- treinamento de modelo;
- conversão para TensorFlow Lite;
- quantização INT8;
- incorporação do modelo ao firmware;
- inferência embarcada;
- validação em ambiente simulado.

O Wokwi foi utilizado para simular o ESP32-S3 e os sensores durante o desenvolvimento e os testes.

---

## 2. Sensores utilizados

### Decisão

Utilizar:

- DHT22:
  - temperatura;
  - umidade;
- LDR:
  - luminosidade por leitura analógica.

### Alternativas avaliadas

Foram considerados sensores adicionais, como:

- PIR;
- MQ2.

### Decisão final

Não incluir sensores adicionais.

### Justificativa

Temperatura, umidade e luminosidade já permitem construir um problema multivariável suficiente para demonstrar o fluxo completo de TinyML.

A inclusão de sensores adicionais aumentaria a complexidade de coleta, treinamento e validação sem necessidade para o objetivo do projeto.

---

## 3. Plataforma embarcada

### Decisão

Utilizar:

- ESP32-S3 DevKitC-1;
- ESP-IDF 6.1;
- TensorFlow Lite Micro.

### Justificativa

O ESP32-S3 possui recursos suficientes para executar o modelo quantizado localmente e é suportado pelo ambiente de desenvolvimento utilizado no projeto.

---

## 4. Variáveis de entrada

### Decisão

Utilizar três entradas:

1. temperatura;
2. umidade;
3. luminosidade em valor ADC.

### Normalização adotada

A mesma transformação é aplicada durante o treinamento e no firmware:

```text
temperatura_normalizada = temperatura / 50
umidade_normalizada = umidade / 100
luminosidade_normalizada = ADC / 4095
```

### Justificativa

Manter o mesmo pré-processamento no treinamento e na execução embarcada é necessário para preservar o comportamento do modelo.

---

## 5. Evolução das classes do modelo

### Decisão preliminar

Na concepção inicial foram consideradas quatro classes em uma única saída:

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`;
- `BAIXA_ILUMINACAO`.

### Problema identificado

A classe `BAIXA_ILUMINACAO` representa uma dimensão diferente das classes térmicas.

Uma condição pode ser, por exemplo:

```text
QUENTE_E_SECO
+
BAIXA_ILUMINACAO
```

Portanto, utilizar uma única saída produziria perda de informação ou ambiguidade.

### Decisão final

Adotar duas saídas independentes.

#### Saída térmica

- `ADEQUADO`;
- `QUENTE_E_SECO`;
- `QUENTE_E_UMIDO`.

#### Saída de iluminação

- `ILUMINACAO_ADEQUADA`;
- `BAIXA_ILUMINACAO`.

### Justificativa

A arquitetura com duas saídas permite representar simultaneamente as condições térmica e de iluminação.

---

## 6. Critérios operacionais do dataset

### Decisão

Adotar critérios explícitos para a construção experimental do dataset.

### Condição térmica

| Classe | Temperatura | Umidade |
|---|---:|---:|
| `ADEQUADO` | 20 a 27 °C | 40 a 70% |
| `QUENTE_E_SECO` | 29 a 36 °C | 20 a 45% |
| `QUENTE_E_UMIDO` | 29 a 36 °C | 70 a 95% |

A faixa entre 27 e 29 °C foi tratada como região de transição.

### Condição de iluminação

| Classe | ADC |
|---|---:|
| `ILUMINACAO_ADEQUADA` | até 1800 |
| `BAIXA_ILUMINACAO` | a partir de 2200 |

A faixa entre 1800 e 2200 foi tratada como região de transição.

### Observação

Esses critérios foram definidos para construção e avaliação experimental do modelo e não representam norma técnica de conforto ambiental.

---

## 7. Relação entre luminosidade e ADC

### Decisão

Utilizar diretamente o valor ADC fornecido pelo LDR como entrada do modelo.

### Validação experimental

Nos testes realizados no Wokwi foi observado:

```text
mais luz  → menor valor ADC
menos luz → maior valor ADC
```

### Justificativa

O comportamento foi verificado antes da definição definitiva das faixas utilizadas no dataset.

---

## 8. Dataset

### Decisão

Construir um dataset próprio com as seis combinações possíveis entre:

- três classes térmicas;
- duas classes de iluminação.

### Configuração final

Foram utilizadas:

- 60 amostras;
- 10 amostras por combinação;
- 0 duplicatas exatas;
- 0 erros na validação dos critérios.

### Justificativa

A distribuição equilibrada evita que uma combinação tenha maior representação que as demais no conjunto experimental.

---

## 9. Arquitetura da rede neural

### Decisão

Utilizar uma MLP com:

- 3 entradas;
- Dense 12 com ReLU;
- Dense 8 com ReLU;
- saída térmica com 3 neurônios;
- saída de iluminação com 2 neurônios.

### Resultado

O modelo possui:

```text
197 parâmetros treináveis
```

### Justificativa

A arquitetura é pequena e compatível com o objetivo de execução embarcada em microcontrolador.

---

## 10. Uso de inteligência artificial

### Decisão

As classificações térmica e de iluminação são realizadas pelo modelo treinado e executado no ESP32-S3.

### Decisão de implementação

O firmware não utiliza regras fixas para determinar uma classe válida, por exemplo:

```cpp
if (temperatura > 30 && umidade < 40) {
    classe = QUENTE_E_SECO;
}
```

A classe é obtida pela saída do modelo após a execução de:

```text
Invoke()
```

### Exceção

Regras determinísticas são utilizadas somente para verificar se a leitura pertence ao domínio experimental estabelecido.

Quando a entrada está fora desse domínio, a inferência não é apresentada como classificação válida.

### Justificativa

Essa separação evita confundir:

- regra de segurança/validação da entrada;
- decisão produzida pela rede neural.

---

## 11. Quantização

### Decisão

Converter o modelo treinado para TensorFlow Lite e utilizar uma versão completamente quantizada para INT8 no ESP32-S3.

### Modelos gerados

```text
modelo_ambiente_float.tflite
modelo_ambiente_int8.tflite
```

### Modelo embarcado

```text
modelo_ambiente_int8.tflite
```

Tamanho:

```text
3584 bytes
```

### Observação

A versão FLOAT possui 3148 bytes. Como a rede é muito pequena, o arquivo INT8 ficou ligeiramente maior devido ao overhead do formato quantizado.

### Justificativa

A quantização INT8 foi adotada principalmente para permitir execução eficiente com operações inteiras no microcontrolador, e não como evidência de redução do tamanho final do arquivo.

---

## 12. Operadores do modelo

### Resultado da análise

O modelo TFLite utiliza quatro operações:

```text
FULLY_CONNECTED
```

### Decisão

Registrar somente o operador `FULLY_CONNECTED` no `MicroMutableOpResolver`.

### Justificativa

Registrar apenas os operadores utilizados reduz dependências desnecessárias no interpretador.

---

## 13. Tensor Arena

### Decisão

Reservar:

```text
20 KB
```

para a Tensor Arena.

### Resultado

A alocação dos tensores foi concluída com sucesso durante a execução no ESP32-S3.

---

## 14. Teste controlado antes da inferência contínua

### Decisão

Executar, após a inicialização, uma amostra conhecida do dataset antes de iniciar as leituras contínuas.

### Amostra utilizada

```text
Temperatura: 36.0 °C
Umidade: 95.0 %
ADC: 4046
```

### Resultado esperado

```text
QUENTE_E_UMIDO
BAIXA_ILUMINACAO
```

### Resultado observado

```text
RESULTADO: TESTE TINYML APROVADO
```

### Justificativa

O autoteste permite verificar, durante a inicialização, se o modelo incorporado ao firmware está produzindo o resultado esperado.

---

## 15. Tratamento de amostras fora do domínio

### Decisão

Não apresentar classificação TinyML como válida quando a entrada estiver fora das regiões utilizadas na construção do dataset.

### Exemplo observado

```text
Temperatura: 19.7 °C
Umidade: 66.5 %
```

Resultado:

```text
Condicao termica: FORA_DO_DOMINIO
Inferencia TinyML nao executada
```

### Justificativa

O modelo sempre produzirá uma das classes disponíveis, mesmo para entradas não representadas adequadamente durante o treinamento.

A validação do domínio evita interpretar essa saída como classificação confiável.

---

## 16. Ambiente de treinamento

### Decisão

Utilizar Google Colab para:

- treinamento;
- avaliação;
- conversão para TensorFlow Lite;
- quantização INT8;
- validação do modelo quantizado.

### Ambiente embarcado

Utilizar Visual Studio Code com ESP-IDF para:

- firmware;
- integração do TensorFlow Lite Micro;
- Wokwi;
- Git;
- documentação.

---

## 17. Estratégia de versionamento

### Decisão

Utilizar Git com desenvolvimento por features.

Foram utilizadas, entre outras:

```text
feature/dataset
feature/firmware
feature/coleta-dataset
feature/treinamento-modelo
feature/inferencia-tinyml
feature/documentacao-final
```

### Justificativa

A separação por etapas permite registrar a evolução do projeto e manter o histórico técnico das implementações.

---

## 18. Resultado das decisões adotadas

A arquitetura final ficou composta por:

```text
DHT22 + LDR
     ↓
ESP32-S3
     ↓
validação do domínio experimental
     ↓
normalização
     ↓
quantização INT8
     ↓
TensorFlow Lite Micro
     ↓
MLP quantizada
     ↓
saída térmica + saída de iluminação
```

A solução foi validada no Wokwi nas seis combinações previstas no dataset.
