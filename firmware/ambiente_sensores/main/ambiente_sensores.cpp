#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

#include "dht.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"


/* =========================================================
 * CONFIGURACAO DOS SENSORES
 * ========================================================= */

/* DHT22 conectado ao GPIO 5 */
#define DHT_GPIO GPIO_NUM_5

/*
 * LDR conectado ao GPIO 4.
 * No ESP32-S3, GPIO 4 corresponde ao ADC1 Channel 3.
 */
#define LDR_ADC_UNIT    ADC_UNIT_1
#define LDR_ADC_CHANNEL ADC_CHANNEL_3


static adc_oneshot_unit_handle_t adc_handle;


/* =========================================================
 * MODELO TINYML
 * ========================================================= */

/*
 * Símbolos gerados automaticamente pelo ESP-IDF
 * para o modelo incorporado ao firmware.
 */
extern const uint8_t modelo_tflite_start[]
    asm("_binary_modelo_ambiente_int8_tflite_start");

extern const uint8_t modelo_tflite_end[]
    asm("_binary_modelo_ambiente_int8_tflite_end");


/*
 * Arena de memoria do TensorFlow Lite Micro.
 */
constexpr size_t TENSOR_ARENA_SIZE = 20 * 1024;

alignas(16) static uint8_t tensor_arena[TENSOR_ARENA_SIZE];


/*
 * Estruturas principais do TensorFlow Lite Micro.
 */
static const tflite::Model *modelo = nullptr;
static tflite::MicroInterpreter *interpretador = nullptr;

static TfLiteTensor *tensor_entrada = nullptr;
static TfLiteTensor *tensor_saida_termica = nullptr;
static TfLiteTensor *tensor_saida_iluminacao = nullptr;


/* =========================================================
 * CLASSES DO MODELO
 * ========================================================= */

static const char *NOMES_TERMICOS[] = {
    "ADEQUADO",
    "QUENTE_E_SECO",
    "QUENTE_E_UMIDO"
};


static const char *NOMES_ILUMINACAO[] = {
    "ILUMINACAO_ADEQUADA",
    "BAIXA_ILUMINACAO"
};


/*
 * Resultado retornado por uma inferencia.
 */
struct ResultadoInferencia
{
    int classe_termica;
    int classe_iluminacao;
};


/* =========================================================
 * CONFIGURACAO DO ADC
 * ========================================================= */

static void configurar_adc(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {};

    init_config.unit_id = LDR_ADC_UNIT;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;

    ESP_ERROR_CHECK(
        adc_oneshot_new_unit(
            &init_config,
            &adc_handle
        )
    );


    adc_oneshot_chan_cfg_t channel_config = {};

    channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    channel_config.atten = ADC_ATTEN_DB_12;

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            LDR_ADC_CHANNEL,
            &channel_config
        )
    );
}


/* =========================================================
 * LEITURA DO DHT22
 * ========================================================= */

static esp_err_t ler_dht22(
    float *temperatura,
    float *umidade
)
{
    return dht_read_float_data(
        DHT_TYPE_AM2301,
        DHT_GPIO,
        umidade,
        temperatura
    );
}


/* =========================================================
 * VALIDACAO DO DOMINIO EXPERIMENTAL
 * ========================================================= */

/*
 * Verifica se temperatura e umidade pertencem a uma das
 * regiões utilizadas na construcao do dataset.
 */
static bool dominio_termico_valido(
    float temperatura,
    float umidade
)
{
    bool adequado =
        temperatura >= 20.0f &&
        temperatura <= 27.0f &&
        umidade >= 40.0f &&
        umidade <= 70.0f;


    bool quente_seco =
        temperatura >= 29.0f &&
        temperatura <= 36.0f &&
        umidade >= 20.0f &&
        umidade <= 45.0f;


    bool quente_umido =
        temperatura >= 29.0f &&
        temperatura <= 36.0f &&
        umidade >= 70.0f &&
        umidade <= 95.0f;


    return (
        adequado ||
        quente_seco ||
        quente_umido
    );
}


/*
 * Valores entre 1800 e 2200 representam a região de
 * transição da iluminacao e não foram utilizados como
 * classes durante o treinamento.
 */
static bool dominio_iluminacao_valido(
    int luminosidade_adc
)
{
    return (
        luminosidade_adc <= 1800 ||
        luminosidade_adc >= 2200
    );
}


/* =========================================================
 * QUANTIZACAO INT8
 * ========================================================= */

static int8_t quantizar_int8(
    float valor,
    const TfLiteTensor *tensor
)
{
    float valor_quantizado =
        valor / tensor->params.scale +
        tensor->params.zero_point;


    int32_t resultado =
        static_cast<int32_t>(
            roundf(valor_quantizado)
        );


    if (resultado < -128) {
        resultado = -128;
    }

    if (resultado > 127) {
        resultado = 127;
    }


    return static_cast<int8_t>(
        resultado
    );
}


/* =========================================================
 * ARGMAX
 * ========================================================= */

static int obter_argmax(
    const int8_t *valores,
    int quantidade
)
{
    int indice_maior = 0;

    for (int i = 1; i < quantidade; i++) {

        if (
            valores[i] >
            valores[indice_maior]
        ) {
            indice_maior = i;
        }
    }

    return indice_maior;
}


/* =========================================================
 * INICIALIZACAO DO TENSORFLOW LITE MICRO
 * ========================================================= */

static bool inicializar_tinyml(void)
{
    modelo = tflite::GetModel(
        modelo_tflite_start
    );


    if (modelo == nullptr) {

        printf(
            "Erro: nao foi possivel carregar o modelo\n"
        );

        return false;
    }


    if (
        modelo->version() !=
        TFLITE_SCHEMA_VERSION
    ) {

        printf(
            "Erro: schema %lu diferente do esperado %d\n",
            static_cast<unsigned long>(
                modelo->version()
            ),
            TFLITE_SCHEMA_VERSION
        );

        return false;
    }


    /*
     * O modelo utiliza somente FULLY_CONNECTED.
     */
    static tflite::MicroMutableOpResolver<1> resolver;


    if (
        resolver.AddFullyConnected()
        != kTfLiteOk
    ) {

        printf(
            "Erro ao registrar FULLY_CONNECTED\n"
        );

        return false;
    }


    static tflite::MicroInterpreter interpretador_estatico(
        modelo,
        resolver,
        tensor_arena,
        TENSOR_ARENA_SIZE
    );


    interpretador =
        &interpretador_estatico;


    if (
        interpretador->AllocateTensors()
        != kTfLiteOk
    ) {

        printf(
            "Erro ao alocar tensores\n"
        );

        return false;
    }


    /* -----------------------------------------------------
     * Tensor de entrada
     * ----------------------------------------------------- */

    tensor_entrada =
        interpretador->input(0);


    if (
        tensor_entrada == nullptr ||
        tensor_entrada->type != kTfLiteInt8
    ) {

        printf(
            "Erro: tensor de entrada INT8 invalido\n"
        );

        return false;
    }


    /* -----------------------------------------------------
     * Tensores de saida
     * ----------------------------------------------------- */

    TfLiteTensor *saida_0 =
        interpretador->output(0);

    TfLiteTensor *saida_1 =
        interpretador->output(1);


    if (
        saida_0 == nullptr ||
        saida_1 == nullptr
    ) {

        printf(
            "Erro: tensores de saida invalidos\n"
        );

        return false;
    }


    int classes_saida_0 =
        saida_0->dims->data[
            saida_0->dims->size - 1
        ];


    int classes_saida_1 =
        saida_1->dims->data[
            saida_1->dims->size - 1
        ];


    /*
     * Identificacao pela quantidade de classes.
     *
     * Termica: 3
     * Iluminacao: 2
     */
    if (
        classes_saida_0 == 3 &&
        classes_saida_1 == 2
    ) {

        tensor_saida_termica =
            saida_0;

        tensor_saida_iluminacao =
            saida_1;
    }
    else if (
        classes_saida_0 == 2 &&
        classes_saida_1 == 3
    ) {

        tensor_saida_iluminacao =
            saida_0;

        tensor_saida_termica =
            saida_1;
    }
    else {

        printf(
            "Erro: formato inesperado das saidas\n"
        );

        return false;
    }


    const unsigned int tamanho_modelo =
        static_cast<unsigned int>(
            modelo_tflite_end -
            modelo_tflite_start
        );


    printf("\n");
    printf("========================================\n");
    printf("TinyML inicializado com sucesso\n");
    printf(
        "Modelo INT8: %u bytes\n",
        tamanho_modelo
    );
    printf(
        "Tensor Arena: %u bytes\n",
        static_cast<unsigned int>(
            TENSOR_ARENA_SIZE
        )
    );
    printf("========================================\n");


    return true;
}


/* =========================================================
 * EXECUCAO DA INFERENCIA
 * ========================================================= */

static bool executar_inferencia(
    float temperatura,
    float umidade,
    int luminosidade_adc,
    ResultadoInferencia *resultado
)
{
    if (
        tensor_entrada == nullptr ||
        tensor_saida_termica == nullptr ||
        tensor_saida_iluminacao == nullptr ||
        resultado == nullptr
    ) {

        return false;
    }


    /*
     * Mesma normalizacao utilizada no treinamento.
     */
    float temperatura_normalizada =
        temperatura / 50.0f;

    float umidade_normalizada =
        umidade / 100.0f;

    float luminosidade_normalizada =
        static_cast<float>(
            luminosidade_adc
        ) / 4095.0f;


    /*
     * Quantizacao das tres entradas.
     */
    tensor_entrada->data.int8[0] =
        quantizar_int8(
            temperatura_normalizada,
            tensor_entrada
        );


    tensor_entrada->data.int8[1] =
        quantizar_int8(
            umidade_normalizada,
            tensor_entrada
        );


    tensor_entrada->data.int8[2] =
        quantizar_int8(
            luminosidade_normalizada,
            tensor_entrada
        );


    /*
     * Executa a rede neural.
     */
    if (
        interpretador->Invoke()
        != kTfLiteOk
    ) {

        printf(
            "Erro durante Invoke()\n"
        );

        return false;
    }


    /*
     * Seleciona a classe com maior logit.
     */
    resultado->classe_termica =
        obter_argmax(
            tensor_saida_termica->data.int8,
            3
        );


    resultado->classe_iluminacao =
        obter_argmax(
            tensor_saida_iluminacao->data.int8,
            2
        );


    return true;
}


/* =========================================================
 * TESTE CONTROLADO
 * ========================================================= */

static bool testar_modelo_tinyml(void)
{
    /*
     * Amostra real existente no dataset.
     */
    constexpr float temperatura = 36.0f;
    constexpr float umidade = 95.0f;
    constexpr int luminosidade = 4046;


    ResultadoInferencia resultado = {};


    if (
        !executar_inferencia(
            temperatura,
            umidade,
            luminosidade,
            &resultado
        )
    ) {

        printf(
            "Erro no teste controlado\n"
        );

        return false;
    }


    printf("\n");
    printf("========================================\n");
    printf("TESTE CONTROLADO DO MODELO\n");
    printf("========================================\n");

    printf(
        "Entrada real: %.1f, %.1f, %d\n",
        temperatura,
        umidade,
        luminosidade
    );


    printf(
        "Classe termica: %s\n",
        NOMES_TERMICOS[
            resultado.classe_termica
        ]
    );


    printf(
        "Classe iluminacao: %s\n",
        NOMES_ILUMINACAO[
            resultado.classe_iluminacao
        ]
    );


    bool teste_aprovado =
        resultado.classe_termica == 2 &&
        resultado.classe_iluminacao == 1;


    if (teste_aprovado) {

        printf(
            "RESULTADO: TESTE TINYML APROVADO\n"
        );

    } else {

        printf(
            "RESULTADO: TESTE TINYML REPROVADO\n"
        );
    }


    printf("========================================\n");


    return teste_aprovado;
}


/* =========================================================
 * APLICACAO PRINCIPAL
 * ========================================================= */

extern "C" void app_main(void)
{
    configurar_adc();


    /*
     * Inicializa o modelo.
     */
    if (!inicializar_tinyml()) {

        printf(
            "Falha na inicializacao do TinyML\n"
        );

        return;
    }


    /*
     * Autoteste utilizando uma amostra conhecida.
     */
    if (!testar_modelo_tinyml()) {

        printf(
            "Falha no teste controlado\n"
        );

        return;
    }


    printf("\n");
    printf("========================================\n");
    printf("INFERENCIA CONTINUA INICIADA\n");
    printf("========================================\n");


    while (1) {

        float temperatura = 0.0f;
        float umidade = 0.0f;

        int luminosidade_adc = 0;


        /* -------------------------------------------------
         * Leitura do DHT22
         * ------------------------------------------------- */

        esp_err_t resultado_dht =
            ler_dht22(
                &temperatura,
                &umidade
            );


        /* -------------------------------------------------
         * Leitura do LDR
         * ------------------------------------------------- */

        ESP_ERROR_CHECK(
            adc_oneshot_read(
                adc_handle,
                LDR_ADC_CHANNEL,
                &luminosidade_adc
            )
        );


        printf("\n");
        printf("----------------------------------------\n");


        /*
         * Se o DHT22 falhar, a inferencia nao deve ser
         * executada.
         */
        if (
            resultado_dht != ESP_OK
        ) {

            printf(
                "Erro DHT22: %s\n",
                esp_err_to_name(
                    resultado_dht
                )
            );

            printf(
                "Inferencia nao executada\n"
            );


            vTaskDelay(
                pdMS_TO_TICKS(2000)
            );

            continue;
        }


        printf(
            "Temperatura : %.1f C\n",
            temperatura
        );

        printf(
            "Umidade     : %.1f %%\n",
            umidade
        );

        printf(
            "LDR ADC     : %d\n",
            luminosidade_adc
        );


        /* -------------------------------------------------
         * Validacao do dominio experimental
         * ------------------------------------------------- */

        bool termico_valido =
            dominio_termico_valido(
                temperatura,
                umidade
            );


        bool iluminacao_valida =
            dominio_iluminacao_valido(
                luminosidade_adc
            );


        if (
            !termico_valido ||
            !iluminacao_valida
        ) {

            printf("\n");
            printf(
                "Amostra fora do dominio experimental\n"
            );


            if (!termico_valido) {

                printf(
                    "Condicao termica: FORA_DO_DOMINIO\n"
                );
            }


            if (!iluminacao_valida) {

                printf(
                    "Iluminacao: ZONA_DE_TRANSICAO\n"
                );
            }


            printf(
                "Inferencia TinyML nao executada\n"
            );


            vTaskDelay(
                pdMS_TO_TICKS(2000)
            );

            continue;
        }


        /* -------------------------------------------------
         * Inferencia TinyML
         * ------------------------------------------------- */

        ResultadoInferencia resultado = {};


        if (
            executar_inferencia(
                temperatura,
                umidade,
                luminosidade_adc,
                &resultado
            )
        ) {

            printf("\n");

            printf(
                "Classe termica     : %s\n",
                NOMES_TERMICOS[
                    resultado.classe_termica
                ]
            );


            printf(
                "Classe iluminacao  : %s\n",
                NOMES_ILUMINACAO[
                    resultado.classe_iluminacao
                ]
            );

        } else {

            printf(
                "Falha durante a inferencia TinyML\n"
            );
        }


        /*
         * Intervalo entre as leituras.
         */
        vTaskDelay(
            pdMS_TO_TICKS(2000)
        );
    }
}