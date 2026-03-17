#include <stdio.h>
#include <stdbool.h>

/* USER CODE BEGIN PV */
#define ADC_RING_SIZE 128

volatile uint16_t adcRing[ADC_RING_SIZE];
volatile uint16_t adcWriteIdx = 0;
volatile uint16_t adcReadIdx  = 0;
volatile uint32_t adcOverflowCount = 0;
/* USER CODE END PV */

/* USER CODE BEGIN 0 */
static bool ADC_RingPush(uint16_t value)
{
    uint16_t next = (adcWriteIdx + 1) % ADC_RING_SIZE;

    if (next == adcReadIdx)
    {
        // Puffer voll
        adcOverflowCount++;
        return false;
    }

    adcRing[adcWriteIdx] = value;
    adcWriteIdx = next;
    return true;
}

static bool ADC_RingPop(uint16_t *value)
{
    if (adcReadIdx == adcWriteIdx)
    {
        // Puffer leer
        return false;
    }

    *value = adcRing[adcReadIdx];
    adcReadIdx = (adcReadIdx + 1) % ADC_RING_SIZE;
    return true;
}
/* USER CODE END 0 */
