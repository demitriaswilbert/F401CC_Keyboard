/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "GPIO_Config.h"
#include "keyboard_map.h"
#include "string.h"
// #include "keyboard_map.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct
{
    uint8_t length;
    uint8_t busy;
    uint8_t data[10];
} send_t;

extern USBD_HandleTypeDef hUsbDeviceFS;
 
volatile send_t sendMedia[2] = {
    {0, 0, {0}},
    {0, 0, {0}}
};
volatile send_t sendNormal[2] = {
    {0, 0, {0}},
    {0, 0, {0}}
};

volatile HID_StateTypeDef USBD_Keyboard_State()
{
    return ((volatile USBD_HID_HandleTypeDef*)hUsbDeviceFS.pClassData)->state;
}

PinId_t* colPins[15] = {
    PA0, PA1, PA2, PA3, PA4,
    PA5, PA6, PA7, PB0, PB9,
    PB8, PB7, PB6, PB5, PB4};

PinId_t* rowPins[5] = {
    PA8,
    PA9,
    PA10,
    PA15,
    PB3,
};

volatile int enablescan = 1;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) 
{
    if(htim == &htim3)
    {
        static int colPressed[6] = {-1, -1, -1, -1, -1, -1};
        static int rowPressed[6] = {-1, -1, -1, -1, -1, -1};
        static int alternatekey[6] = {0};
        static int nPressed = 0;
        static int update = 0;
        static int alternate = 0;
        for (int i = 0; i < 5; i++)
        {
            PinId_Write(rowPins[i], 0);
            for (volatile int j = 0; j < 10; j++)
                ;
            for (int j = 0; j < 15; j++)
            {
                if (!PinId_Read(colPins[j]))
                {
                    if (i * 15 + j == 70)
                    {
                        alternate = 1;
                        continue;
                    }

                    int emptyPos = -1;
                    for (int k = 0; k < 6 && nPressed < 6; k++)
                    {
                        if (colPressed[k] == j && rowPressed[k] == i)
                        {
                            emptyPos = -1;
                            break;
                        }
                        if (colPressed[k] == -1)
                        {
                            emptyPos = (emptyPos == -1) ? k : emptyPos;
                        }
                    }
                    if (emptyPos != -1)
                    {
                        colPressed[emptyPos] = j;
                        rowPressed[emptyPos] = i;
                        alternatekey[emptyPos] = alternate;
                        nPressed++;
                        update = 1;
                        // CDC_Transmit_FS((uint8_t*) "scanning\r\n", (uint16_t)10);
                    }
                }
                else
                {
                    if (i * 15 + j == 70)
                    {
                        alternate = 0;
                        continue;
                    }

                    for (int k = 0; k < 6 && nPressed > 0; k++)
                    {
                        if (colPressed[k] == j && rowPressed[k] == i)
                        {
                            colPressed[k] = -1;
                            rowPressed[k] = -1;
                            alternatekey[k] = 0;
                            nPressed--;
                            update = 1;
                            break;
                        }
                    }
                }
            }
            PinId_Write(rowPins[i], 1);
        }
        // uint32_t time = HAL_GetTick();
        if (update)
        {
            KeyboardHID_t myhid = {1};
            uint8_t mediaBuf[3] = {0x02, 0, 0};
            int media = 0;
            static int mediaprev = 0;
            int normal = 0;
            static int normalprev = 0;
            for (int i = 0; i < 6; i++)
            {
                if (colPressed[i] != -1)
                {
                    if (alternatekey[i])
                    {
                        uint16_t key = keys_alternate[rowPressed[i] * 15 + colPressed[i]];
                        if (key != 0)
                        {
                            if ((key & 0xff00) == 0x8000)
                            {
                                mediaBuf[1] = (uint8_t)(key & 0xffU);
                                alternate = 1;
                                media++;
                            }
                            else
                            {
                                USBD_Keyboard_press(&myhid, key);
                                normal++;
                            }
                            continue;
                        }
                    }
                    USBD_Keyboard_press(&myhid, keys[rowPressed[i] * 15 + colPressed[i]]);
                    normal++;
                }
            }
            if (normal > 0 || normalprev > 0)
            {
                if (sendNormal[0].busy == 0)
                {
                    memcpy((void*)&sendNormal[0].data, (const void*)&myhid, sizeof(myhid));
                    sendNormal[0].length = sizeof(myhid);
                }
                else
                {
                    memcpy((void*)&sendNormal[1].data, (const void*)&myhid, sizeof(myhid));
                    sendNormal[1].length = sizeof(myhid);
                }
                memset(&myhid.MODIFIER, 0, 8U);
                normalprev = normal;
            }
            if (media > 0 || mediaprev > 0)
            {
                if (sendMedia[0].busy == 0)
                {
                    memcpy((void*)&sendMedia[0].data, (const void*)&mediaBuf, sizeof(mediaBuf));
                    sendMedia[0].length = sizeof(mediaBuf);
                }
                else
                {
                    memcpy((void*)&sendMedia[1].data, (const void*)&mediaBuf, sizeof(mediaBuf));
                    sendMedia[1].length = sizeof(mediaBuf);
                }
                // while(USBD_Keyboard_State() != HID_IDLE)
                //     ;
                memset(&mediaBuf[1], 0, 2);
                mediaprev = media;
            }
            update = 0;
        }
    }
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int __attribute__((optimize("-Og"))) main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM3_Init();
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN 2 */
    TIM3->PSC = (84000000 / 1000000) - 1;
    TIM3->ARR = (1000000 / 2000) - 1;
    HAL_TIM_Base_Start_IT(&htim3);

    // while (1)
    // {
    //     KeyboardHID_t myhid = {1, 0, 0, {0x2c, 0, 0, 0, 0, 0}};
    //     USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&myhid, sizeof(myhid));
    //     while(USBD_Keyboard_State() != HID_IDLE)
    //         ;
    //     myhid.KEYCODE[0] = 0;
    //     USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&myhid, sizeof(myhid));
    //     while(USBD_Keyboard_State() != HID_IDLE)
    //         ;
    //     // memset(&myhid, 0, sizeof(myhid));
    //     // if(time > 0)
    //     GPIOC->ODR ^= (1U << 13U);
    // }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    // {
    //     static int colPressed[6] = {-1, -1, -1, -1, -1, -1};
    //     static int rowPressed[6] = {-1, -1, -1, -1, -1, -1};
    //     static int alternatekey[6] = {0};
    //     static int nPressed = 0;
    //     static int update = 0;
    //     static int alternate = 0;
    //     GPIOC->ODR ^= (1U << 13U);
    //     for (int i = 0; i < 5; i++)
    //     {
    //         PinId_Write(rowPins[i], 0);
    //         for (volatile int j = 0; j < 10; j++)
    //             ;
    //         for (int j = 0; j < 15; j++)
    //         {
    //             if (!PinId_Read(colPins[j]))
    //             {
    //                 if (i * 15 + j == 70)
    //                 {
    //                     alternate = 1;
    //                     continue;
    //                 }
    //                 int emptyPos = -1;
    //                 for (int k = 0; k < 6 && nPressed < 6; k++)
    //                 {
    //                     if (colPressed[k] == j && rowPressed[k] == i)
    //                     {
    //                         emptyPos = -1;
    //                         break;
    //                     }
    //                     if (colPressed[k] == -1)
    //                     {
    //                         emptyPos = (emptyPos == -1) ? k : emptyPos;
    //                     }
    //                 }
    //                 if (emptyPos != -1)
    //                 {
    //                     colPressed[emptyPos] = j;
    //                     rowPressed[emptyPos] = i;
    //                     alternatekey[emptyPos] = alternate;
    //                     nPressed++;
    //                     update = 1;
    //                     // CDC_Transmit_FS((uint8_t*) "scanning\r\n", (uint16_t)10);
    //                 }
    //             }
    //             else
    //             {
    //                 if (i * 15 + j == 70)
    //                 {
    //                     alternate = 0;
    //                     continue;
    //                 }
    //                 for (int k = 0; k < 6 && nPressed > 0; k++)
    //                 {
    //                     if (colPressed[k] == j && rowPressed[k] == i)
    //                     {
    //                         colPressed[k] = -1;
    //                         rowPressed[k] = -1;
    //                         alternatekey[k] = 0;
    //                         nPressed--;
    //                         update = 1;
    //                         break;
    //                     }
    //                 }
    //             }
    //         }
    //         PinId_Write(rowPins[i], 1);
    //     }
    //     // uint32_t time = HAL_GetTick();
    //     if (update)
    //     {
    //         KeyboardHID_t myhid = {1};
    //         uint8_t mediaBuf[3] = {0x02, 0, 0};
    //         int media = 0;
    //         static int mediaprev = 0;
    //         int normal = 0;
    //         static int normalprev = 0;
    //         for (int i = 0; i < 6; i++)
    //         {
    //             if (colPressed[i] != -1)
    //             {
    //                 if (alternatekey[i])
    //                 {
    //                     uint16_t key = keys_alternate[rowPressed[i] * 15 + colPressed[i]];
    //                     if (key != 0)
    //                     {
    //                         if ((key & 0xff00) == 0x8000)
    //                         {
    //                             mediaBuf[1] = (uint8_t)(key & 0xffU);
    //                             alternate = 1;
    //                             media++;
    //                         }
    //                         else
    //                         {
    //                             USBD_Keyboard_press(&myhid, key);
    //                             normal++;
    //                         }
    //                         continue;
    //                     }
    //                 }
    //                 USBD_Keyboard_press(&myhid, keys[rowPressed[i] * 15 + colPressed[i]]);
    //                 normal++;
    //             }
    //         }
    //         if (normal > 0 || normalprev > 0)
    //         {
    //             if (sendNormal[0].busy == 0)
    //             {
    //                 memcpy((void*)&sendNormal[0].data, (const void*)&myhid, sizeof(myhid));
    //                 sendNormal[0].length = sizeof(myhid);
    //             }
    //             else
    //             {
    //                 memcpy((void*)&sendNormal[1].data, (const void*)&myhid, sizeof(myhid));
    //                 sendNormal[1].length = sizeof(myhid);
    //             }
    //             memset(&myhid.MODIFIER, 0, 8U);
    //             normalprev = normal;
    //         }
    //         if (media > 0 || mediaprev > 0)
    //         {
    //             if (sendMedia[0].busy == 0)
    //             {
    //                 memcpy((void*)&sendMedia[0].data, (const void*)&mediaBuf, sizeof(mediaBuf));
    //                 sendMedia[0].length = sizeof(mediaBuf);
    //             }
    //             else
    //             {
    //                 memcpy((void*)&sendMedia[1].data, (const void*)&mediaBuf, sizeof(mediaBuf));
    //                 sendMedia[1].length = sizeof(mediaBuf);
    //             }
    //             // while(USBD_Keyboard_State() != HID_IDLE)
    //             //     ;
    //             memset(&mediaBuf[1], 0, 2);
    //             mediaprev = media;
    //         }
    //         update = 0;
    //     }
    // }
        static int iter = 0;
        static int i = 0;
        HID_StateTypeDef USB_State = ((volatile USBD_HID_HandleTypeDef*)hUsbDeviceFS.pClassData)->state;
        if (USB_State == HID_IDLE)
        {
            send_t* pSend = (send_t*)((iter)? sendMedia : sendNormal);
            if (pSend[i].length > 0 && pSend[i].busy == 0)
            {
                pSend[i].busy = 1;
                USBD_HID_SendReport(&hUsbDeviceFS, pSend[i].data, pSend[i].length);
                pSend[i].length = 0;
            }
            else if (pSend[i].length == 0)
            {
                pSend[i].busy = 0;
                i = !i;
            }
            iter = iter ^ i;
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = (84000000 / 1000000) - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = (1000000 / 1000) - 1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

    /*Configure GPIO pins : PA0 PA1 PA2 PA3
                             PA4 PA5 PA6 PA7 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB0 PB4 PB5 PB6
                             PB7 PB8 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : PA8 PA9 PA10 PA15 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PB3 */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
