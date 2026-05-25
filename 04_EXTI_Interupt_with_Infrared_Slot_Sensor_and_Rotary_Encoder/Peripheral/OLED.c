#include "stm32f10x.h"
#include "OLED_Font.h"

/* 腳位配置
 * SCL: GPIOC_Pin_14
 * SDA: GPIOC_Pin_15
 */
#define OLED_W_SCL(x) GPIO_WriteBit(GPIOC, GPIO_Pin_14, (BitAction)(x))
#define OLED_W_SDA(x) GPIO_WriteBit(GPIOC, GPIO_Pin_15, (BitAction)(x))

/*初始化*/
void OLED_I2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
 * @brief  I2C開始
 * @param  無
 * @retval 無
 */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
 * @brief  I2C停止
 * @param  無
 * @retval 無
 */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
 * @brief  I2C 傳送一個位元組
 * @param  Byte 要傳送的一個位元組
 * @retval 無
 */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1); // 額外時鐘，不處理應答訊號。
	OLED_W_SCL(0);
}

/**
 * @brief  寫入「命令」至 OLED
 * @param  Command 要寫入的命令
 * @retval 無
 */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78); // Slave的地址
	OLED_I2C_SendByte(0x00); // 寫命令
	OLED_I2C_SendByte(Command);
	OLED_I2C_Stop();
}

/**
 * @brief  寫入「資料」至 OLED
 * @param  Data 要寫入的資料
 * @retval 無
 */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78); // Slave的地址
	OLED_I2C_SendByte(0x40); // 寫資料
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
 * @brief  OLED 設定游標位置
 * @param  Y 以左上角為原點，向下方向的座標，範圍：0~7
 * @param  X 以左上角為原點，向右方向的座標，範圍：0~127
 * @retval 無
 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);				 // 设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));		 // 设置X位置低4位
}

/**
 * @brief  OLED 清除顯示
 * @param  無
 * @retval 無
 */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
 * @brief  OLED 顯示一個字元
 * @param  Line 行位置，範圍：1~4
 * @param  Column 列位置，範圍：1~16
 * @param  Char 要顯示的字元，範圍：ASCII 可見字元
 * @retval 無
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // 設定游標位置於上半區
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]); // 顯示上半部內容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // 設定游標位置於下半區
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]); // 顯示下半部內容
	}
}

/**
 * @brief  OLED 顯示字串
 * @param  Line 起始行位置，範圍：1~4
 * @param  Column 起始列位置，範圍：1~16
 * @param  String 要顯示的字串，範圍：ASCII 可見字元
 * @retval 無
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
 * @brief  OLED 次方函數
 * @retval 回傳值等於 X 的 Y 次方
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
 * @brief  OLED 顯示數字（十進制，正數）
 * @param  Line 起始行位置，範圍：1~4
 * @param  Column 起始列位置，範圍：1~16
 * @param  Number 要顯示的數字，範圍：0~4294967295
 * @param  Length 要顯示數字的長度，範圍：1~10
 * @retval 無
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED 顯示數字（十進制，帶符號數）
 * @param  Line 起始行位置，範圍：1~4
 * @param  Column 起始列位置，範圍：1~16
 * @param  Number 要顯示的數字，範圍：-2147483648~2147483647
 * @param  Length 要顯示數字的長度，範圍：1~10
 * @retval 無
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED 顯示數字（十六進位，正數）
 * @param  Line 起始行位置，範圍：1~4
 * @param  Column 起始列位置，範圍：1~16
 * @param  Number 要顯示的數字，範圍：0~0xFFFFFFFF
 * @param  Length 要顯示數字的長度，範圍：1~8
 * @retval 無
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED 顯示數字（二進位，正數）
 * @param  Line 起始行位置，範圍：1~4
 * @param  Column 起始列位置，範圍：1~16
 * @param  Number 要顯示的數字，範圍：0~1111 1111 1111 1111
 * @param  Length 要顯示數字的長度，範圍：1~16
 * @retval 無
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
 * @brief  OLED 初始化
 * @param  無
 * @retval 無
 */
void OLED_Init(void)
{
	uint32_t i, j;

	for (i = 0; i < 1000; i++) // 上電延時
	{
		for (j = 0; j < 1000; j++)
			;
	}

	OLED_I2C_Init(); // 埠初始化

	OLED_WriteCommand(0xAE); // 關閉顯示

	OLED_WriteCommand(0xD5); // 設定顯示時鐘分頻比/振盪器頻率
	OLED_WriteCommand(0x80);

	OLED_WriteCommand(0xA8); // 設定多路複用率
	OLED_WriteCommand(0x3F);

	OLED_WriteCommand(0xD3); // 設定顯示偏移
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40); // 設定顯示開始行

	OLED_WriteCommand(0xA1); // 設定左右方向，0xA1正常 0xA0左右反置

	OLED_WriteCommand(0xC8); // 設定上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA); // 設定 COM 腳位硬體配置
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81); // 設定對比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9); // 設定預充電週期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB); // 設定 VCOMH 取消選擇等級
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4); // 設定整體顯示開啟/關閉

	OLED_WriteCommand(0xA6); // 設定正常/反轉顯示

	OLED_WriteCommand(0x8D); // 設定充電幫浦
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF); // 開啟顯示

	OLED_Clear(); // OLED 清除顯示
}
