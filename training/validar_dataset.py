import csv
from collections import Counter
from pathlib import Path

ARQUIVO = Path(__file__).resolve().parent.parent / "dataset" / "dataset_ambiente.csv"

COLUNAS = [
    "temperatura",
    "umidade",
    "luminosidade",
    "classe_termica",
    "classe_iluminacao",
]


def validar_termica(temp, umidade, classe):
    if classe == "ADEQUADO":
        return 20.0 <= temp <= 27.0 and 40.0 <= umidade <= 70.0

    if classe == "QUENTE_E_SECO":
        return 29.0 <= temp <= 36.0 and 20.0 <= umidade <= 45.0

    if classe == "QUENTE_E_UMIDO":
        return 29.0 <= temp <= 36.0 and 70.0 <= umidade <= 95.0

    return False


def validar_iluminacao(adc, classe):
    if classe == "ILUMINACAO_ADEQUADA":
        return adc <= 1800

    if classe == "BAIXA_ILUMINACAO":
        return adc >= 2200

    return False


def main():
    erros = []
    registros = []

    with ARQUIVO.open("r", encoding="utf-8-sig", newline="") as arquivo:
        leitor = csv.DictReader(arquivo)

        if leitor.fieldnames != COLUNAS:
            print("ERRO: cabecalho inesperado.")
            print("Encontrado:", leitor.fieldnames)
            print("Esperado :", COLUNAS)
            return

        for numero_linha, linha in enumerate(leitor, start=2):
            try:
                temperatura = float(linha["temperatura"])
                umidade = float(linha["umidade"])
                luminosidade = int(linha["luminosidade"])
                classe_termica = linha["classe_termica"].strip()
                classe_iluminacao = linha["classe_iluminacao"].strip()

                registro = (
                    temperatura,
                    umidade,
                    luminosidade,
                    classe_termica,
                    classe_iluminacao,
                )

                registros.append(registro)

                if not validar_termica(
                    temperatura,
                    umidade,
                    classe_termica
                ):
                    erros.append(
                        f"Linha {numero_linha}: classe termica incompatível "
                        f"({temperatura}, {umidade}, {classe_termica})"
                    )

                if not validar_iluminacao(
                    luminosidade,
                    classe_iluminacao
                ):
                    erros.append(
                        f"Linha {numero_linha}: classe de iluminacao incompatível "
                        f"({luminosidade}, {classe_iluminacao})"
                    )

                if 1800 < luminosidade < 2200:
                    erros.append(
                        f"Linha {numero_linha}: ADC {luminosidade} "
                        f"esta na zona de transicao"
                    )

            except (ValueError, TypeError) as erro:
                erros.append(
                    f"Linha {numero_linha}: erro de formato - {erro}"
                )

    combinacoes = Counter(
        (registro[3], registro[4])
        for registro in registros
    )

    duplicados = [
        registro
        for registro, quantidade in Counter(registros).items()
        if quantidade > 1
    ]

    print("=" * 60)
    print("VALIDACAO DO DATASET")
    print("=" * 60)

    print(f"\nArquivo: {ARQUIVO}")
    print(f"Total de amostras: {len(registros)}")

    print("\nDistribuicao por grupo:")
    for grupo, quantidade in sorted(combinacoes.items()):
        print(
            f"  {grupo[0]} + {grupo[1]}: "
            f"{quantidade} amostras"
        )

    print(f"\nDuplicatas exatas: {len(duplicados)}")
    print(f"Erros encontrados: {len(erros)}")

    if duplicados:
        print("\nDuplicatas:")
        for registro in duplicados:
            print(" ", registro)

    if erros:
        print("\nProblemas:")
        for erro in erros:
            print(" ", erro)

    if (
        len(registros) == 60
        and len(combinacoes) == 6
        and all(quantidade == 10 for quantidade in combinacoes.values())
        and not duplicados
        and not erros
    ):
        print("\nRESULTADO: DATASET VALIDADO COM SUCESSO.")
    else:
        print("\nRESULTADO: DATASET REQUER REVISAO.")


if __name__ == "__main__":
    main()