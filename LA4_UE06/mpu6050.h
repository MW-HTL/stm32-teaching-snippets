// (c) 2026 by Michael Weilguni

#ifndef MPU6050_H
#define MPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#define MPU6050_I2C_HANDLE         hi2c1          /* handle to MPU-6050 I2C */
#define MPU6050_I2C_ADDR           (0x68U << 1)   /* use 0x69 if AD0 is connected to VCC */
#define MPU6050_I2C_TIMEOUT_MS     100U

/* acceleration range */
typedef enum
{
    Mpu6050_AccelRange_2G = 0,
    Mpu6050_AccelRange_4G,
    Mpu6050_AccelRange_8G,
    Mpu6050_AccelRange_16G
} Mpu6050_AccelRange_t;

/* gyroscope range */
typedef enum
{
    Mpu6050_GyroRange_250Dps = 0,
    Mpu6050_GyroRange_500Dps,
    Mpu6050_GyroRange_1000Dps,
    Mpu6050_GyroRange_2000Dps
} Mpu6050_GyroRange_t;

typedef struct
{
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t temperature;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
} Mpu6050_RawData_t;

typedef struct
{
    float accelX_g;
    float accelY_g;
    float accelZ_g;
    float temperature_degC;
    float gyroX_dps;
    float gyroY_dps;
    float gyroZ_dps;
} Mpu6050_Data_t;

bool Mpu6050_Init(void);
bool Mpu6050_IsAlive(void);

bool Mpu6050_ReadRaw(Mpu6050_RawData_t *rawData);
bool Mpu6050_ReadData(Mpu6050_Data_t *data);

bool Mpu6050_SetAccelRange(Mpu6050_AccelRange_t range);
bool Mpu6050_SetGyroRange(Mpu6050_GyroRange_t range);
bool Mpu6050_SetSampleRateDivider(uint8_t divider);
bool Mpu6050_SetDlpf(uint8_t dlpf);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_H */
