// blfm_imu.h
#ifndef BLFM_IMU_H
#define BLFM_IMU_H

void blfm_imu_init(void);
void blfm_imu_read(float *ax, float *ay, float *az);

#endif
