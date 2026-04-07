// (c) 2026 by Michael Weilguni

#include "mpu6050.h"

extern I2C_HandleTypeDef MPU6050_I2C_HANDLE;

/* Registers */
#define Mpu6050_RegSmplrtDiv       0x19U
#define Mpu6050_RegConfig          0x1AU
#define Mpu6050_RegGyroConfig      0x1BU
#define Mpu6050_RegAccelConfig     0x1CU
#define Mpu6050_RegAccelXoutH      0x3BU
#define Mpu6050_RegTempOutH        0x41U
#define Mpu6050_RegGyroXoutH       0x43U
#define Mpu6050_RegPwrMgmt1        0x6BU
#define Mpu6050_RegWhoAmI          0x75U

/* Bit masks/values */
#define Mpu6050_WhoAmI_Value       0x68U
#define Mpu6050_PwrMgmt1_Sleep     0x40U

static Mpu6050_AccelRange_t mpu6050AccelRange = Mpu6050_AccelRange_2G;
static Mpu6050_GyroRange_t  mpu6050GyroRange  = Mpu6050_GyroRange_250Dps;

static bool Mpu6050_WriteRegister(uint8_t reg, uint8_t value);
static bool Mpu6050_ReadRegister(uint8_t reg, uint8_t *value);
static bool Mpu6050_ReadRegisters(uint8_t reg, uint8_t *data, uint16_t length);
static float Mpu6050_GetAccelSensitivity(void);
static float Mpu6050_GetGyroSensitivity(void);

static bool Mpu6050_WriteRegister(uint8_t reg, uint8_t value)
{
    return (HAL_I2C_Mem_Write(&MPU6050_I2C_HANDLE, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                              &value, 1, MPU6050_I2C_TIMEOUT_MS) == HAL_OK);
}

static bool Mpu6050_ReadRegister(uint8_t reg, uint8_t *value)
{
    if (value == NULL)
    {
        return false;
    }

    return (HAL_I2C_Mem_Read(&MPU6050_I2C_HANDLE, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                             value, 1, MPU6050_I2C_TIMEOUT_MS) == HAL_OK);
}

static bool Mpu6050_ReadRegisters(uint8_t reg, uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return false;
    }

    return (HAL_I2C_Mem_Read(&MPU6050_I2C_HANDLE, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                             data, length, MPU6050_I2C_TIMEOUT_MS) == HAL_OK);
}

static float Mpu6050_GetAccelSensitivity(void)
{
    switch (mpu6050AccelRange)
    {
        case Mpu6050_AccelRange_2G:
            return 16384.0f;

        case Mpu6050_AccelRange_4G:
            return 8192.0f;

        case Mpu6050_AccelRange_8G:
            return 4096.0f;

        case Mpu6050_AccelRange_16G:
            return 2048.0f;

        default:
            return 16384.0f;
    }
}

static float Mpu6050_GetGyroSensitivity(void)
{
    switch (mpu6050GyroRange)
    {
        case Mpu6050_GyroRange_250Dps:
            return 131.0f;

        case Mpu6050_GyroRange_500Dps:
            return 65.5f;

        case Mpu6050_GyroRange_1000Dps:
            return 32.8f;

        case Mpu6050_GyroRange_2000Dps:
            return 16.4f;

        default:
            return 131.0f;
    }
}

bool Mpu6050_IsAlive(void)
{
    uint8_t whoAmI = 0U;

    if (!Mpu6050_ReadRegister(Mpu6050_RegWhoAmI, &whoAmI))
    {
        return false;
    }

    return (whoAmI == Mpu6050_WhoAmI_Value);
}

bool Mpu6050_SetAccelRange(Mpu6050_AccelRange_t range)
{
    uint8_t regValue;

    switch (range)
    {
        case Mpu6050_AccelRange_2G:
            regValue = 0x00U;
            break;

        case Mpu6050_AccelRange_4G:
            regValue = 0x08U;
            break;

        case Mpu6050_AccelRange_8G:
            regValue = 0x10U;
            break;

        case Mpu6050_AccelRange_16G:
            regValue = 0x18U;
            break;

        default:
            return false;
    }

    if (!Mpu6050_WriteRegister(Mpu6050_RegAccelConfig, regValue))
    {
        return false;
    }

    mpu6050AccelRange = range;
    return true;
}

bool Mpu6050_SetGyroRange(Mpu6050_GyroRange_t range)
{
    uint8_t regValue;

    switch (range)
    {
        case Mpu6050_GyroRange_250Dps:
            regValue = 0x00U;
            break;

        case Mpu6050_GyroRange_500Dps:
            regValue = 0x08U;
            break;

        case Mpu6050_GyroRange_1000Dps:
            regValue = 0x10U;
            break;

        case Mpu6050_GyroRange_2000Dps:
            regValue = 0x18U;
            break;

        default:
            return false;
    }

    if (!Mpu6050_WriteRegister(Mpu6050_RegGyroConfig, regValue))
    {
        return false;
    }

    mpu6050GyroRange = range;
    return true;
}

bool Mpu6050_SetSampleRateDivider(uint8_t divider)
{
    return Mpu6050_WriteRegister(Mpu6050_RegSmplrtDiv, divider);
}

bool Mpu6050_SetDlpf(uint8_t dlpf)
{
    if (dlpf > 6U)
    {
        return false;
    }

    return Mpu6050_WriteRegister(Mpu6050_RegConfig, dlpf);
}

bool Mpu6050_Init(void)
{
    HAL_Delay(50);

    if (!Mpu6050_IsAlive())
    {
        return false;
    }

    /* Wake up + Clock source = Gyro X PLL */
    if (!Mpu6050_WriteRegister(Mpu6050_RegPwrMgmt1, 0x01U))
    {
        return false;
    }

    HAL_Delay(10);

    /* DLPF = 3 */
    if (!Mpu6050_SetDlpf(3U))
    {
        return false;
    }

    /* Sample rate divider = 0 */
    if (!Mpu6050_SetSampleRateDivider(0U))
    {
        return false;
    }

    /* Default ranges */
    if (!Mpu6050_SetAccelRange(Mpu6050_AccelRange_2G))
    {
        return false;
    }

    if (!Mpu6050_SetGyroRange(Mpu6050_GyroRange_250Dps))
    {
        return false;
    }

    return true;
}

bool Mpu6050_ReadRaw(Mpu6050_RawData_t *rawData)
{
    uint8_t buffer[14];

    if (rawData == NULL)
    {
        return false;
    }

    if (!Mpu6050_ReadRegisters(Mpu6050_RegAccelXoutH, buffer, sizeof(buffer)))
    {
        return false;
    }

    rawData->accelX      = (int16_t)((buffer[0]  << 8) | buffer[1]);
    rawData->accelY      = (int16_t)((buffer[2]  << 8) | buffer[3]);
    rawData->accelZ      = (int16_t)((buffer[4]  << 8) | buffer[5]);
    rawData->temperature = (int16_t)((buffer[6]  << 8) | buffer[7]);
    rawData->gyroX       = (int16_t)((buffer[8]  << 8) | buffer[9]);
    rawData->gyroY       = (int16_t)((buffer[10] << 8) | buffer[11]);
    rawData->gyroZ       = (int16_t)((buffer[12] << 8) | buffer[13]);

    return true;
}

bool Mpu6050_ReadData(Mpu6050_Data_t *data)
{
    Mpu6050_RawData_t rawData;
    float accelSensitivity;
    float gyroSensitivity;

    if (data == NULL)
    {
        return false;
    }

    if (!Mpu6050_ReadRaw(&rawData))
    {
        return false;
    }

    accelSensitivity = Mpu6050_GetAccelSensitivity();
    gyroSensitivity  = Mpu6050_GetGyroSensitivity();

    data->accelX_g = (float)rawData.accelX / accelSensitivity;
    data->accelY_g = (float)rawData.accelY / accelSensitivity;
    data->accelZ_g = (float)rawData.accelZ / accelSensitivity;

    data->temperature_degC = ((float)rawData.temperature / 340.0f) + 36.53f;

    data->gyroX_dps = (float)rawData.gyroX / gyroSensitivity;
    data->gyroY_dps = (float)rawData.gyroY / gyroSensitivity;
    data->gyroZ_dps = (float)rawData.gyroZ / gyroSensitivity;

    return true;
}
