# Dataset do Projeto

## 1. Finalidade

O dataset será utilizado para treinar e avaliar o modelo de classificação de condições ambientais.

Cada registro deverá representar uma condição ambiental formada pela combinação de:

- temperatura;
- umidade;
- luminosidade.

A classe atribuída ao registro será utilizada como rótulo durante o treinamento supervisionado.

---

## 2. Variáveis de entrada

O dataset possuirá inicialmente três características de entrada.

### Temperatura

Origem:

- sensor DHT22.

Unidade:

- graus Celsius (°C).

### Umidade

Origem:

- sensor DHT22.

Unidade:

- porcentagem de umidade relativa (%).

### Luminosidade

Origem:

- sensor LDR.

Representação inicial:

- valor bruto obtido pelo conversor analógico-digital do ESP32-S3.

Observação:

A utilização de valor ADC ou outra forma de representação da luminosidade será confirmada após os primeiros testes no Wokwi.

---

## 3. Classes previstas

O dataset deverá conter inicialmente quatro classes:

| classe_id | classe |
|---|---|
| 0 | ADEQUADO |
| 1 | QUENTE_E_SECO |
| 2 | QUENTE_E_UMIDO |
| 3 | BAIXA_ILUMINACAO |

Os critérios quantitativos utilizados para rotular cada classe ainda serão definidos.

Nenhum intervalo será considerado definitivo antes da análise das condições de simulação e da metodologia de geração das amostras.

---

## 4. Estrutura prevista do arquivo

O arquivo principal deverá utilizar o formato CSV.

Nome previsto:

`dataset_ambiente.csv`

Estrutura:

```text
temperatura,umidade,luminosidade,classe_id,classe