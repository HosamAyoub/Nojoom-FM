#include "LIB/STD_TYPES.h"
#include "LIB/BIT_MATH.h"
#include "MCAL/RCC/RCC_interface.h"
#include "MCAL/GPIO/GPIO_interface.h"
#include "MCAL/STK/STK_interface.h"
#include "MCAL/NVIC/NVIC_interface.h"
#include "MCAL/EXTI/EXTI_interface.h"
#include "MCAL/SPI/SPI_interface.h"
#include "HAL/IR/IR_interface.h"
#include "HAL/R2RDAC/R2RDAC_interface.h"
#include "HAL/TFT/TFT_interface.h"
#include "HAL/STP/STP_interface.h"
#include "Image.h"
#include "Audio.h"

void SwitchChannel (void);
void APP_voidWrapperForSTP_LEDMATRIX_Display (u8 *Copy_u8Arr);
void Stop (void);

u8 Global_u8AmrDiab[59]  = {0, 124, 18, 17, 18, 124, 0, 0, 127, 2, 4, 2, 127, 0, 0, 127, 25, 41, 70, 0, 0, 0, 0, 127, 65, 65, 34, 28, 0, 0, 65, 65, 127, 65, 65, 0, 0, 124, 18, 17, 18, 124, 0, 0, 127, 73, 73, 73, 54, 0, 0, 0, 0, 0, 0, 0, 's'},
		Global_u8MohamedMounir[102] = {0, 127, 2, 4, 2, 127, 0, 0, 28, 34, 65, 65, 34, 28, 0, 0, 127, 8, 8, 8, 127, 0, 0, 124, 18, 17, 18, 124, 0, 0, 127, 2, 4, 2, 127, 0, 0, 127, 73, 73, 73, 0, 0, 127, 65, 65, 34, 28, 0, 0, 0, 127, 2, 4, 2, 127, 0, 0, 28, 34, 65, 65, 34, 28, 0, 0, 63, 64, 64, 64, 63, 0, 0, 127, 2, 4, 8, 16, 32, 127, 0, 0, 65, 65, 127, 65, 65, 0, 0, 127, 25, 41, 70, 0, 0, 0, 0, 0, 0, 0, 0, 's'};

u8 *Global_pu8Name = Global_u8AmrDiab, *Global_pu8Song = Global_u8AmrDiabSong, Global_u8Stop, Global_u8LEDMATRIX, Global_u8Break;
u32 Global_u32Size = 74583;

void main (void)
{

	u8 Local_u8Iterator1, Local_u8Iterator2 = 0, Local_u8State = 0;

	TFT_voidInit();
	R2RDAC_voidInit();
	STP_voidInit();
	RCC_voidInit();
	STK_voidInit();
	TFT_voidDisplay (Global_u16Images[0]);
	IR_voidInit();
	IR_voidStart(&SwitchChannel);

	RCC_voidEnablePeripheral(RCC_AHB1, RCC_AHB1_GPIOB);
	RCC_voidEnablePeripheral(RCC_AHB1, RCC_AHB1_GPIOC);

	GPIO_voidSetMode(GPIO_PORTC, GPIO_PIN15, GPIO_OUTPUT);
	GPIO_voidOutputPinMode(GPIO_PORTC, GPIO_PIN15, GPIO_OUTPUT_PUSHPULL, GPIO_MEDIUM_SPEED);
	GPIO_voidSetPinValue(GPIO_PORTC, GPIO_PIN15, GPIO_PIN_LOW);

	GPIO_voidSetMode(GPIO_PORTB, GPIO_PIN1, GPIO_INPUT);
	GPIO_voidInputPinMode(GPIO_PORTB, GPIO_PIN1, GPIO_INPUT_PULL_UP);

	NVIC_voidEnablePeripheralInterrupt(NVIC_PERIPHERAL_EXTI1);

	EXTI_voidSetPortInterrupt(EXTI_PORTB, EXTI_LINE1);
	EXTI_voidCallBack(EXTI_LINE1, &Stop);
	EXTI_voidInterruptTrigger(EXTI_LINE1, EXTI_FALLING);
	EXTI_voidInterruptEnableDisable(EXTI_LINE1, EXTI_ENABLE);

	while(1)
	{
		if (!Global_u8Stop)
		{
			while ((Global_pu8Name[Local_u8Iterator2+7] != 's') && (!Global_u8Break))
			{
				for (Local_u8Iterator1 = 0; Local_u8Iterator1 < 6; Local_u8Iterator1++)
				{
					if (Global_u8Break)
					{
						break;
					}
					else
					{
						APP_voidWrapperForSTP_LEDMATRIX_Display(Global_pu8Name + Local_u8Iterator2);
					}
				}
				Local_u8Iterator2++;
			}

			if (Global_u8Break)
			{
				Global_u8Break = 0;
				Local_u8Iterator2 = 0;
			}
			else
			{
				R2RDAC_voidSetData(Global_pu8Song, Global_u32Size);
			}

		}
		else
		{
			GPIO_voidSetPinValue(GPIO_PORTC, GPIO_PIN15, TOG_BIT(Local_u8State, 0));
			STK_voidDelayMS(500);
		}
	}
}

void SwitchChannel (void)
{
	GPIO_voidSetPinValue(GPIO_PORTC, GPIO_PIN15, GPIO_PIN_LOW);
	switch (Global_u8Data)
	{
	case 12:
		Global_pu8Name = Global_u8AmrDiab;
		Global_pu8Song = Global_u8AmrDiabSong;
		Global_u32Size = 74583;
		TFT_voidDisplay (Global_u16Images[0]);
		Global_u8Stop = 0;
		Global_u8Break = 1;
		Global_u8LEDMATRIX = 0;
		break;
	case 24:
		Global_pu8Name = Global_u8MohamedMounir;
		Global_pu8Song = Global_u8MounirSong;
		Global_u32Size = 87400;
		TFT_voidDisplay (Global_u16Images[1]);
		Global_u8Stop = 0;
		Global_u8Break = 1;
		Global_u8LEDMATRIX = 0;
		break;
	default: ; break;
	}
}


void APP_voidWrapperForSTP_LEDMATRIX_Display (u8 *Copy_u8Arr)
{
	u8 Local_u8Iterator, Local_u8ColumnsValue = 0xFF;
	/* Disable Columns */
	STP_voidSendDataSynchronouseOneSTP(STP2, Local_u8ColumnsValue);
	for (Local_u8Iterator = 0; Local_u8Iterator < 8; Local_u8Iterator++)
	{
		if (Global_u8Break)
		{
			break;
		}
		else
		{
			/* Set Rows Value */
			STP_voidSendDataSynchronouseOneSTP(STP1, Copy_u8Arr[Local_u8Iterator]);
			STK_voidDelayUS(2000);
			STP_voidSendDataSynchronouseOneSTP(STP2, CLR_BIT(Local_u8ColumnsValue, Local_u8Iterator));
			STP_voidSendDataSynchronouseOneSTP(STP2, SET_BIT(Local_u8ColumnsValue, Local_u8Iterator));
		}
	}
}

void Stop (void)
{
	Global_u8Stop = 1;
	Global_u8Break = 1;
	TFT_voidDisplayColor(0x0000);
	Global_pu8Name = NULL;
	Global_pu8Song = NULL;
}
