#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

#include "dht.h"

/* DHT22 conectado ao GPIO 5 */
#define DHT_GPIO GPIO_NUM_5

/* LDR conectado ao GPIO 4
 * No ESP32-S3, GPIO 4 corresponde ao ADC1 Channel 3.
 */
#define LDR_ADC_UNIT    ADC_UNIT_1
#define LDR_ADC_CHANNEL ADC_CHANNEL_3

static adc_oneshot_unit_handle_t adc_handle;


/* ---------------------------------------------------------
 * Configuração do ADC utilizado pelo LDR
 * --------------------------------------------------------- */
static void configurar_adc(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = LDR_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    ESP_ERROR_CHECK(
        adc_oneshot_new_unit(
            &init_config,
            &adc_handle
        )
    );

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            LDR_ADC_CHANNEL,
            &channel_config
        )
    );
}


/* ---------------------------------------------------------
 * Leitura do DHT22
 * --------------------------------------------------------- */
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


/* ---------------------------------------------------------
 * Aplicação principal
 * --------------------------------------------------------- */
void app_main(void)
{
    configurar_adc();

    /*
     * Cabeçalho CSV.
     *
     * Cada leitura válida será apresentada no formato:
     *
     * temperatura,umidade,luminosidade
     * 24.0,50.0,1001
     */
    printf("\n");
    printf("temperatura,umidade,luminosidade\n");

    while (1) {

        float temperatura = 0.0f;
        float umidade = 0.0f;
        int luminosidade_adc = 0;

        /* Leitura do DHT22 */
        esp_err_t resultado_dht =
            ler_dht22(
                &temperatura,
                &umidade
            );

        /* Leitura do LDR */
        ESP_ERROR_CHECK(
            adc_oneshot_read(
                adc_handle,
                LDR_ADC_CHANNEL,
                &luminosidade_adc
            )
        );

        /*
         * Somente leituras válidas do DHT22
         * são apresentadas como registros CSV.
         */
        if (resultado_dht == ESP_OK) {

            printf(
                "%.1f,%.1f,%d\n",
                temperatura,
                umidade,
                luminosidade_adc
            );

        } else {

            /*
             * O caractere # permite identificar facilmente
             * linhas que não pertencem ao dataset.
             */
            printf(
                "# Erro DHT22: %s\n",
                esp_err_to_name(resultado_dht)
            );
        }

        /*
         * Intervalo de 2 segundos entre as leituras.
         */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}