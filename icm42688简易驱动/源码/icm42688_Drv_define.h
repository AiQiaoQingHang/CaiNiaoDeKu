#ifndef icm42688_DEFINE_H
#define icm42688_DEFINE_H
/*icm42688�Ĵ�����ַ�궨��*/
/*ע�⣺����ֻ�о��˼Ĵ����顾0���ļĴ�����ַ*/
#define DRIVE_CONFIG_0 0x13      // ����IIC��ƽ��ת��
#define INTF_CONFIG_1 0x4D       // ���ü��ٶȼ�ʱ��
#define icm42688_PWR 0x4E        // �����ǣ����ٶȼƹ���ģʽ
#define GYRO_CONFIG_0 0x4F       // ��������ز���
#define ACCEL_CONFIG_0 0x50      // ���ٶȼ���ز���
#define GYRO_CONFIG_1 0x51       // �¶ȼ��˲����������˲���˳��
#define GYRO_ACCEL_CONFIG_0 0x52 // �����Ǻͼ��ٶȼ��˲�����
#define ACCEL_CONFIG_1 0x53      // ���ٶȼ��˲���˳��
#define WHO_AM_I 0x75            // �豸��
#define REG_BANK_SEL 0x76        // ѡ��Ĵ�����

/*------------------------------- �� �� ---------------------------------*/
#define icm42688_TEMP_OUT_H 0x1D // �¶�
#define icm42688_TEMP_OUT_L 0x1E

#define icm42688_ACCEL_XOUT_H 0x1F // ���ٶ�
#define icm42688_ACCEL_XOUT_L 0x20
#define icm42688_ACCEL_YOUT_H 0x21
#define icm42688_ACCEL_YOUT_L 0x22
#define icm42688_ACCEL_ZOUT_H 0x23
#define icm42688_ACCEL_ZOUT_L 0x24

#define icm42688_GYRO_XOUT_H 0x25 // ת��������
#define icm42688_GYRO_XOUT_L 0x26
#define icm42688_GYRO_YOUT_H 0x27
#define icm42688_GYRO_YOUT_L 0x28
#define icm42688_GYRO_ZOUT_H 0x29
#define icm42688_GYRO_ZOUT_L 0x2A

#endif
