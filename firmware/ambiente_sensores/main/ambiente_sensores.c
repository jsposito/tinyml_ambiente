#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

#include "dht.h"

#define DHT_GPIO GPIO_NUM_5

#define LDR_ADC_UNIT    ADC_UNIT_1
#define LDR_ADC_CHANNEL ADC_CHANNEL_3   // GPIO 4 no ESP32-S3

static adc_oneshot_unit_handle_t adc_handle;

static void configurar_adc(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = LDR_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };

    ESP_ERROR_CHECK(
        adc_oneshot_new_unit(&init_config, &adc_handle)
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

static esp_err_t ler_dht22(float *temperatura, float *umidade)
{
    return dht_read_float_data(
        DHT_TYPE_AM2301,
        DHT_GPIO,
        umidade,
        temperatura
    );
}

void app_main(void)
{
    configurar_adc();

    printf("\n");
    printf("========================================\n");
    printf(" Monitor Ambiental - ESP32-S3\n");
    printf(" DHT22 + LDR\n");
    printf("========================================\n");

    while (1) {

        float temperatura = 0.0f;
        float umidade = 0.0f;
        int luminosidade_adc = 0;

        esp_err_t resultado_dht =
            ler_dht22(&temperatura, &umidade);

        ESP_ERROR_CHECK(
            adc_oneshot_read(
                adc_handle,
                LDR_ADC_CHANNEL,
                &luminosidade_adc
            )
        );

        if (resultado_dht == ESP_OK) {

            printf(
                "Temperatura: %.1f C | "
                "Umidade: %.1f %% | "
                "LDR ADC: %d\n",
                temperatura,
                umidade,
                luminosidade_adc
            );

        } else {

            printf(
                "Erro DHT22: %s | "
                "LDR ADC: %d\n",
                esp_err_to_name(resultado_dht),
                luminosidade_adc
            );
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}