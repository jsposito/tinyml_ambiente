from pathlib import Path
import random

import numpy as np
import pandas as pd
import tensorflow as tf

from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, confusion_matrix


# ============================================================
# CONFIGURACAO
# ============================================================

SEED = 42

random.seed(SEED)
np.random.seed(SEED)
tf.random.set_seed(SEED)

RAIZ = Path(__file__).resolve().parent.parent
ARQUIVO_DATASET = RAIZ / "dataset" / "dataset_ambiente.csv"
PASTA_MODELO = RAIZ / "model"

PASTA_MODELO.mkdir(parents=True, exist_ok=True)


# ============================================================
# CLASSES
# ============================================================

CLASSES_TERMICAS = {
    "ADEQUADO": 0,
    "QUENTE_E_SECO": 1,
    "QUENTE_E_UMIDO": 2,
}

CLASSES_ILUMINACAO = {
    "ILUMINACAO_ADEQUADA": 0,
    "BAIXA_ILUMINACAO": 1,
}


# ============================================================
# CARREGAMENTO DO DATASET
# ============================================================

df = pd.read_csv(ARQUIVO_DATASET)

print("=" * 60)
print("TREINAMENTO DO MODELO TINYML")
print("=" * 60)

print(f"\nDataset: {ARQUIVO_DATASET}")
print(f"Amostras: {len(df)}")


# ============================================================
# ENTRADAS
# ============================================================

X = df[
    [
        "temperatura",
        "umidade",
        "luminosidade",
    ]
].to_numpy(dtype=np.float32)


# ============================================================
# NORMALIZACAO
# ============================================================

# Normalizacao fixa para permitir a mesma operacao
# posteriormente no firmware do ESP32-S3.

X[:, 0] = X[:, 0] / 50.0
X[:, 1] = X[:, 1] / 100.0
X[:, 2] = X[:, 2] / 4095.0


# ============================================================
# ROTULOS
# ============================================================

y_termica = (
    df["classe_termica"]
    .map(CLASSES_TERMICAS)
    .to_numpy(dtype=np.int32)
)

y_iluminacao = (
    df["classe_iluminacao"]
    .map(CLASSES_ILUMINACAO)
    .to_numpy(dtype=np.int32)
)


# Classe combinada usada apenas para garantir
# distribuicao equilibrada nos conjuntos.

grupo = (
    df["classe_termica"].astype(str)
    + "__"
    + df["classe_iluminacao"].astype(str)
).to_numpy()


# ============================================================
# DIVISAO TREINO / VALIDACAO / TESTE
#
# 60 amostras:
# 36 treino
# 12 validacao
# 12 teste
#
# Cada conjunto preserva os seis grupos.
# ============================================================

(
    X_temp,
    X_teste,
    y_termica_temp,
    y_termica_teste,
    y_iluminacao_temp,
    y_iluminacao_teste,
    grupo_temp,
    _,
) = train_test_split(
    X,
    y_termica,
    y_iluminacao,
    grupo,
    test_size=0.20,
    random_state=SEED,
    stratify=grupo,
)

(
    X_treino,
    X_validacao,
    y_termica_treino,
    y_termica_validacao,
    y_iluminacao_treino,
    y_iluminacao_validacao,
) = train_test_split(
    X_temp,
    y_termica_temp,
    y_iluminacao_temp,
    test_size=0.25,
    random_state=SEED,
    stratify=grupo_temp,
)

print("\nDivisao do dataset:")
print(f"Treino:     {len(X_treino)}")
print(f"Validacao:  {len(X_validacao)}")
print(f"Teste:      {len(X_teste)}")


# ============================================================
# MODELO
#
# Entrada:
# temperatura + umidade + luminosidade
#
# Saidas independentes:
# 1. classificacao termica
# 2. classificacao de iluminacao
# ============================================================

entrada = tf.keras.Input(
    shape=(3,),
    name="entrada",
)

x = tf.keras.layers.Dense(
    12,
    activation="relu",
    name="dense_1",
)(entrada)

x = tf.keras.layers.Dense(
    8,
    activation="relu",
    name="dense_2",
)(x)

saida_termica = tf.keras.layers.Dense(
    3,
    name="termica",
)(x)

saida_iluminacao = tf.keras.layers.Dense(
    2,
    name="iluminacao",
)(x)

modelo = tf.keras.Model(
    inputs=entrada,
    outputs=[
        saida_termica,
        saida_iluminacao,
    ],
)


# ============================================================
# COMPILACAO
# ============================================================

modelo.compile(
    optimizer=tf.keras.optimizers.Adam(
        learning_rate=0.01
    ),
    loss={
        "termica":
            tf.keras.losses.SparseCategoricalCrossentropy(
                from_logits=True
            ),
        "iluminacao":
            tf.keras.losses.SparseCategoricalCrossentropy(
                from_logits=True
            ),
    },
    metrics={
        "termica": ["accuracy"],
        "iluminacao": ["accuracy"],
    },
)

modelo.summary()


# ============================================================
# TREINAMENTO
# ============================================================

early_stop = tf.keras.callbacks.EarlyStopping(
    monitor="val_loss",
    patience=40,
    restore_best_weights=True,
)

historico = modelo.fit(
    X_treino,
    {
        "termica": y_termica_treino,
        "iluminacao": y_iluminacao_treino,
    },
    validation_data=(
        X_validacao,
        {
            "termica": y_termica_validacao,
            "iluminacao": y_iluminacao_validacao,
        },
    ),
    epochs=300,
    batch_size=8,
    callbacks=[early_stop],
    verbose=0,
)

print(
    f"\nEpocas executadas: "
    f"{len(historico.history['loss'])}"
)


# ============================================================
# AVALIACAO
# ============================================================

predicoes = modelo.predict(
    X_teste,
    verbose=0,
)

pred_termica = np.argmax(
    predicoes[0],
    axis=1,
)

pred_iluminacao = np.argmax(
    predicoes[1],
    axis=1,
)

acc_termica = accuracy_score(
    y_termica_teste,
    pred_termica,
)

acc_iluminacao = accuracy_score(
    y_iluminacao_teste,
    pred_iluminacao,
)

matriz_termica = confusion_matrix(
    y_termica_teste,
    pred_termica,
)

matriz_iluminacao = confusion_matrix(
    y_iluminacao_teste,
    pred_iluminacao,
)

print("\nResultados no conjunto de teste:")
print(
    f"Acuracia termica: "
    f"{acc_termica:.4f}"
)
print(
    f"Acuracia iluminacao: "
    f"{acc_iluminacao:.4f}"
)

print("\nMatriz de confusao - termica:")
print(matriz_termica)

print("\nMatriz de confusao - iluminacao:")
print(matriz_iluminacao)


# ============================================================
# MODELO KERAS
# ============================================================

arquivo_keras = (
    PASTA_MODELO
    / "modelo_ambiente.keras"
)

modelo.save(arquivo_keras)


# ============================================================
# TFLITE FLOAT
# ============================================================

conversor_float = (
    tf.lite.TFLiteConverter.from_keras_model(
        modelo
    )
)

modelo_tflite_float = (
    conversor_float.convert()
)

arquivo_tflite_float = (
    PASTA_MODELO
    / "modelo_ambiente_float.tflite"
)

arquivo_tflite_float.write_bytes(
    modelo_tflite_float
)


# ============================================================
# TFLITE INT8
# ============================================================

def representative_dataset():
    for amostra in X_treino:
        yield [
            np.expand_dims(
                amostra.astype(np.float32),
                axis=0,
            )
        ]


conversor_int8 = (
    tf.lite.TFLiteConverter.from_keras_model(
        modelo
    )
)

conversor_int8.optimizations = [
    tf.lite.Optimize.DEFAULT
]

conversor_int8.representative_dataset = (
    representative_dataset
)

conversor_int8.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
]

conversor_int8.inference_input_type = tf.int8
conversor_int8.inference_output_type = tf.int8

modelo_tflite_int8 = conversor_int8.convert()

arquivo_tflite_int8 = (
    PASTA_MODELO
    / "modelo_ambiente_int8.tflite"
)

arquivo_tflite_int8.write_bytes(
    modelo_tflite_int8
)


# ============================================================
# INSPECAO DO MODELO QUANTIZADO
# ============================================================

interpretador = tf.lite.Interpreter(
    model_path=str(arquivo_tflite_int8)
)

interpretador.allocate_tensors()

entrada_info = (
    interpretador.get_input_details()
)

saidas_info = (
    interpretador.get_output_details()
)


# ============================================================
# HISTORICO
# ============================================================

pd.DataFrame(
    historico.history
).to_csv(
    PASTA_MODELO
    / "historico_treinamento.csv",
    index=False,
)


# ============================================================
# RELATORIO
# ============================================================

arquivo_metricas = (
    PASTA_MODELO
    / "metricas_modelo.txt"
)

with arquivo_metricas.open(
    "w",
    encoding="utf-8",
) as arquivo:

    arquivo.write(
        "MODELO TINYML - CLASSIFICACAO AMBIENTAL\n"
    )

    arquivo.write("=" * 50 + "\n\n")

    arquivo.write(
        f"Total de amostras: {len(df)}\n"
    )

    arquivo.write(
        f"Treino: {len(X_treino)}\n"
    )

    arquivo.write(
        f"Validacao: {len(X_validacao)}\n"
    )

    arquivo.write(
        f"Teste: {len(X_teste)}\n\n"
    )

    arquivo.write(
        f"Acuracia termica: "
        f"{acc_termica:.4f}\n"
    )

    arquivo.write(
        f"Acuracia iluminacao: "
        f"{acc_iluminacao:.4f}\n\n"
    )

    arquivo.write(
        "Matriz de confusao termica:\n"
    )

    arquivo.write(
        str(matriz_termica)
    )

    arquivo.write("\n\n")

    arquivo.write(
        "Matriz de confusao iluminacao:\n"
    )

    arquivo.write(
        str(matriz_iluminacao)
    )

    arquivo.write("\n\n")

    arquivo.write(
        f"Tamanho TFLite FLOAT: "
        f"{len(modelo_tflite_float)} bytes\n"
    )

    arquivo.write(
        f"Tamanho TFLite INT8: "
        f"{len(modelo_tflite_int8)} bytes\n\n"
    )

    arquivo.write(
        f"Entrada INT8:\n"
        f"{entrada_info}\n\n"
    )

    arquivo.write(
        f"Saidas INT8:\n"
        f"{saidas_info}\n"
    )


print("\nArquivos gerados:")

print(
    f"  {arquivo_keras}"
)

print(
    f"  {arquivo_tflite_float}"
)

print(
    f"  {arquivo_tflite_int8}"
)

print(
    f"  {arquivo_metricas}"
)

print(
    "\nTREINAMENTO CONCLUIDO."
)