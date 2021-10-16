#include "at24cxx.h"

void I2C1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��������Ϊ��©�����ʵ��iic�������߼�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
}

void I2C1_Configuration(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0XA0;//�����ĵ�ַ        
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100KHZ
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);                                             
}

void I2C1_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   
    I2C1_GPIO_Configuration();
    I2C1_Configuration();
}

uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress)
{
    if(NumByteToWrite==0)
        return 1;
    /* 1.��ʼ*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.�豸��ַ��/д */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.����д���� */
    while(NumByteToWrite--)
    {
      I2C_SendData(I2Cx, *pBuffer);
      while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      pBuffer++;
    }

    /* 4.ֹͣ */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while ((I2Cx->CR1&0x200) == 0x200);
    return 0;
}

uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress)
{
    if(NumByteToRead==0)
        return 1;

    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));  
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    /* 1.��ʼ*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /* 2.�豸��ַ��/д */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.��ʼ*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /* 4.�豸��ַ��/�� */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 5.����д���� */
    while (NumByteToRead)
    {
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
        *pBuffer++ = I2C_ReceiveData(I2Cx);
        NumByteToRead--;
    }
		
		//6.ֹͣ����Ӧ��
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		I2C_GenerateSTOP(I2Cx, ENABLE);
		
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    return 0;
}   







