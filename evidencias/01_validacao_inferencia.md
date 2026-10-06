# Evidência de Validação da Inferência TinyML

## Ambiente

- Placa: ESP32-S3 DevKitC-1
- Framework: ESP-IDF 6.1
- Simulação: Wokwi
- Modelo: `modelo_ambiente_int8.tflite`
- Tamanho do modelo: 3584 bytes
- Tensor Arena: 20480 bytes
- Runtime: TensorFlow Lite Micro

---

## 1. Inicialização do modelo

Saída observada no monitor serial:

```text
TinyML inicializado com sucesso
Modelo INT8: 3584 bytes
Tensor Arena: 20480 bytes
```

---

## 2. Teste controlado

Amostra conhecida do dataset:

```text
Temperatura: 36.0 °C
Umidade: 95.0 %
LDR ADC: 4046
```

Resultado observado:

```text
Classe termica: QUENTE_E_UMIDO
Classe iluminacao: BAIXA_ILUMINACAO
RESULTADO: TESTE TINYML APROVADO
```

Resultado: **APROVADO**

---

## 3. Inferência contínua

### Teste 1/6

Entrada:

```text
Temperatura: 36.0 °C
Umidade: 95.0 %
LDR ADC: 3969
```

Saída:

```text
Classe termica     : QUENTE_E_UMIDO
Classe iluminacao  : BAIXA_ILUMINACAO
```

Resultado: **APROVADO**

---

### Teste 2/6

Entrada:

```text
Temperatura: 36.0 °C
Umidade: 95.0 %
LDR ADC: 1407
```

Saída:

```text
Classe termica     : QUENTE_E_UMIDO
Classe iluminacao  : ILUMINACAO_ADEQUADA
```

Resultado: **APROVADO**

---

### Teste 3/6

Entrada:

```text
Temperatura: 33.0 °C
Umidade: 30.0 %
LDR ADC: 1407
```

Saída:

```text
Classe termica     : QUENTE_E_SECO
Classe iluminacao  : ILUMINACAO_ADEQUADA
```

Resultado: **APROVADO**

---

### Teste 4/6

Entrada:

```text
Temperatura: 33.0 °C
Umidade: 30.0 %
LDR ADC: 3788
```

Saída:

```text
Classe termica     : QUENTE_E_SECO
Classe iluminacao  : BAIXA_ILUMINACAO
```

Durante o teste, a classificação permaneceu estável com valores ADC entre aproximadamente 3295 e 4058.

Resultado: **APROVADO**

---

### Teste 5/6

Entrada:

```text
Temperatura: 24.0 °C
Umidade: 50.0 %
LDR ADC: 3295
```

Saída:

```text
Classe termica     : ADEQUADO
Classe iluminacao  : BAIXA_ILUMINACAO
```

Resultado: **APROVADO**

---

### Teste 6/6

Entrada:

```text
Temperatura: 24.0 °C
Umidade: 50.0 %
LDR ADC: 1074
```

Saída:

```text
Classe termica     : ADEQUADO
Classe iluminacao  : ILUMINACAO_ADEQUADA
```

A classificação de iluminação permaneceu adequada com a redução progressiva do ADC até valores inferiores a 100.

Resultado: **APROVADO**

---

## 4. Teste fora do domínio experimental

Entrada observada:

```text
Temperatura: 19.7 °C
Umidade: 66.5 %
LDR ADC: 3969
```

Saída:

```text
Amostra fora do dominio experimental
Condicao termica: FORA_DO_DOMINIO
Inferencia TinyML nao executada
```

Resultado: **COMPORTAMENTO ESPERADO**

---

## 5. Resultado geral

| Teste | Resultado |
|---|---|
| Teste controlado | Aprovado |
| 1/6 | Aprovado |
| 2/6 | Aprovado |
| 3/6 | Aprovado |
| 4/6 | Aprovado |
| 5/6 | Aprovado |
| 6/6 | Aprovado |
| Fora do domínio | Comportamento esperado |

A inferência embarcada foi validada para todas as seis combinações experimentais previstas no dataset.
