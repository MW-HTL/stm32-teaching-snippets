/* USER CODE BEGIN PV */
#define ADC_DMA_SIZE        128
#define ADC_DMA_HALF_SIZE   (ADC_DMA_SIZE / 2)

uint16_t adcDmaBuffer[ADC_DMA_SIZE];
volatile uint32_t adcDroppedSamples = 0;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
static void ADC_CopyBlockToRing(const uint16_t *src, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        if (!ADC_RingPush(src[i]))
        {
            adcDroppedSamples++;
        }
    }
}
/* USER CODE END 0 */

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        ADC_CopyBlockToRing(&adcDmaBuffer[0], ADC_DMA_HALF_SIZE);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        ADC_CopyBlockToRing(&adcDmaBuffer[ADC_DMA_HALF_SIZE], ADC_DMA_HALF_SIZE);
    }
}
/* USER CODE END 4 */

/* kommt später in USER CODE 0, siehe Angabe */
typedef struct
{
    uint16_t min;
    uint16_t max;
    uint16_t peakToPeak;
    float mean;
} ADC_BlockStats_t;
/* USER CODE END 0 */

