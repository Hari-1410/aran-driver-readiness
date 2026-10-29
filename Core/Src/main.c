/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    main.c
 * @brief   ARAN – Milestones 1 + 2 + 3 + 4 + 5 + 6 super-loop.
 *
 * Milestone 6 adds:
 *   - OLED_UpdateDisplay() : live 4-line SSD1306 display (I2C1, PB6/PB7)
 *   - BT_SendAlert()       : structured packet over USART1 → ESP32 → BT SPP
 *   - BUZZER_Alert()       : GPIO PB0 beep patterns keyed to DSI zone
 *
 * Communication architecture (Milestone 6 revised):
 *   USART2 (ST-Link VCP)  → ECG replay reception (unchanged, interrupt-driven)
 *   USART1 (PA9/PA10)     → ESP32 UART2 bridge (replaces FTDI visualisation)
 *   I2C1   (PB6/PB7)      → SSD1306 OLED
 *   PB0    (GPIO_OUTPUT)  → Buzzer
 *
 * NOTE: The Python ARAN dashboard (aran_dashboard_m6.py) now connects to the
 * ESP32's Bluetooth SPP port rather than an FTDI COM port. The STM32 raw/
 * filtered/HRV/CNN/fusion data is forwarded by the ESP32 over BT after the
 * "BT," packet; all other "S," and "R,"/"F," lines are passed through by the
 * ESP32 unchanged so the dashboard parses them identically to previous
 * milestones.
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ecg_uart.h"
#include "window_manager.h"
#include "dsp_preprocessing.h"
#include "pan_tompkins.h"
#include "hrv.h"
#include "cnn.h"
#include "fusion.h"
#include "oled.h"   /* Milestone 6: SSD1306 OLED     */
#include "bluetooth.h"      /* Milestone 6: ESP32 BT bridge  */
#include "buzzer.h"         /* Milestone 6: buzzer patterns  */
#include <stdio.h>
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
CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static EcgWindow_t     s_current_window;
static float32_t       s_filtered_window[WINDOW_SIZE];
static PT_Result_t     s_pt_result;
static HRV_t           s_hrv;
static CNN_Result_t    s_cnn_result;
static Fusion_Result_t s_fusion_result;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM14_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initialises the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM14_Init();
  MX_CRC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */

  /* ---- Milestone 1: ECG UART + Window Manager ---- */
  WM_Init(&s_current_window);

  /* Single-UART mode: ECG reception and ECG status share USART2 (ST-Link VCP).
   * USART1 is now exclusively used for the ESP32 Bluetooth bridge.           */
  ECG_UART_Init(&huart2, &huart2);

  /* ---- Milestone 2: DSP ---- */
  DSP_Init();

  /* ---- Milestone 3: Pan-Tompkins ---- */
  PT_Init();

  /* ---- Milestone 4: CNN (X-CUBE-AI) ---- */
  if (!CNN_Init())
  {
      char err_msg[48];
      snprintf(err_msg, sizeof(err_msg), "CNN_INIT_FAIL,0x%08lX",
               (unsigned long)CNN_GetLastError());
      ECG_UART_SendStatus(err_msg);
      Error_Handler();
  }

  /* ---- Milestone 6: OLED, Bluetooth bridge, Buzzer ---- */
  if (OLED_Init(&hi2c1) != HAL_OK)
  {
      ECG_UART_SendStatus("WARN: OLED init failed – display disabled");
      /* Non-fatal: continue without display */
  }

  BT_Init(&huart1);       /* USART1 → ESP32 UART2 */
  BUZZER_Init();

  /* Ready banner on OLED */
  OLED_Clear();
  OLED_DrawString(0, 0, "ARAN M6");
  OLED_DrawString(2, 0, "Initialising...");
  OLED_Flush();

  /* ---- Start windowing ---- */
  WM_Reset(&s_current_window);
  ECG_UART_SendStatus("ARAN Milestone 6 ready - OLED + BT + Buzzer active");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* Drain the circular buffer into the active window.
       * Reception keeps running via interrupt in parallel.          */
      bool window_ready = WM_Update(ECG_UART_GetBuffer(), &s_current_window);

      if (window_ready)
      {
          /* ── Status ── */
          char msg[64];
          snprintf(msg, sizeof(msg), "Window %lu READY (2500 samples)",
                   (unsigned long)s_current_window.window_id);
          ECG_UART_SendStatus(msg);

          /* ── Milestone 2: DSP preprocessing ── */
          DSP_Preprocess(s_current_window.data, s_filtered_window);

          /* ── Milestone 3a: Pan-Tompkins R-peak detection ── */
          PT_Detect(s_filtered_window, &s_pt_result);

          snprintf(msg, sizeof(msg), "Peaks detected: %lu",
                   (unsigned long)s_pt_result.peak_count);
          ECG_UART_SendStatus(msg);

          /* ── Milestone 3b: HRV feature extraction ── */
          HRV_Compute(&s_pt_result, &s_hrv);

          /* ── Milestone 4: CNN inference ── */
          CNN_Run(s_filtered_window, &s_cnn_result);

          /* ── Milestone 5: Decision Fusion + Driver Safety Index ── */
          Fusion_Compute(&s_hrv, &s_cnn_result, &s_fusion_result);

          /* ══════════════════════════════════════════════════════════
           *  Milestone 6: User Interface & Alert System
           * ══════════════════════════════════════════════════════════ */

          /* 6-a: OLED – refresh all four status lines */
          OLED_UpdateDisplay(&s_hrv, &s_cnn_result, &s_fusion_result);

          /* 6-b: Bluetooth – send structured alert packet to ESP32 */
          BT_SendAlert(&s_fusion_result, &s_hrv, &s_cnn_result);

          /* 6-c: Buzzer – pattern according to DSI zone             */
          BUZZER_Alert(s_fusion_result.zone);

          /* ══════════════════════════════════════════════════════════
           *  UART reporting (dashboard / debug via ESP32 BT pass-through)
           * ══════════════════════════════════════════════════════════ */

          /* NOTE: In Milestone 6, USART1 is the ESP32 bridge channel.
           * All "R,","F,","S," telemetry is still sent via ECG_UART
           * which uses USART2 (ST-Link VCP) in single-UART mode, OR
           * you may re-route ECG_UART to USART1 if the ESP32 firmware
           * is updated to forward all lines over BT (see aran_dashboard_m6.py). */

          /* 1. Forward raw + filtered samples */
          for (uint32_t i = 0U; i < WINDOW_SIZE; i++)
          {
              ECG_UART_SendRawSample(s_current_window.window_id, i,
                                      s_current_window.data[i]);
              ECG_UART_SendFilteredSample(s_current_window.window_id, i,
                                           s_filtered_window[i]);
          }

          /* 2. R-peak indices and RR intervals */
          HRV_SendPeaksAndIntervals(s_current_window.window_id, &s_pt_result);

          /* 3. HRV feature vector */
          HRV_SendUART(s_current_window.window_id, &s_hrv);

          /* 4. CNN fatigue probability + timing */
          CNN_SendUART(s_current_window.window_id, &s_cnn_result);

          /* 5. Decision Fusion result (DSI + zone) */
          Fusion_SendUART(s_current_window.window_id, &s_fusion_result);

          /* Prepare for the next 10-second window */
          WM_Reset(&s_current_window);
      }

      if (ECG_UART_HasError())
      {
          ECG_UART_SendStatus("WARNING: UART error or buffer overflow detected");
      }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = 8;
  RCC_OscInitStruct.PLL.PLLN            = 180;
  RCC_OscInitStruct.PLL.PLLP            = 2;
  RCC_OscInitStruct.PLL.PLLQ            = 2;
  RCC_OscInitStruct.PLL.PLLR            = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  if (HAL_PWREx_EnableOverDrive() != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief CRC Initialization Function
  */
static void MX_CRC_Init(void)
{
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief I2C1 Initialization Function
  * @note  PB6 = SCL, PB7 = SDA (Nucleo F446RE default AF4 mapping).
  *        The SSD1306 is at 0x3C; I2C clock 100 kHz is more than sufficient.
  */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance             = I2C1;
  hi2c1.Init.ClockSpeed      = 100000;
  hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1     = 0;
  hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2     = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief TIM14 Initialization Function
  */
static void MX_TIM14_Init(void)
{
  htim14.Instance                = TIM14;
  htim14.Init.Prescaler          = 0;
  htim14.Init.CounterMode        = TIM_COUNTERMODE_UP;
  htim14.Init.Period             = 65535;
  htim14.Init.ClockDivision      = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload  = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief USART1 Initialization Function
  * @note  PA9 = TX → ESP32 GPIO16 (RX)
  *        PA10 = RX ← ESP32 GPIO17 (TX)   [optional, for future ACK/command path]
  *        115200-8N1, no flow control.
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance          = USART1;
  huart1.Init.BaudRate     = 115200;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief USART2 Initialization Function
  * @note  Dedicated to ECG replay reception from ST-Link VCP.
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance          = USART2;
  huart2.Init.BaudRate     = 115200;
  huart2.Init.WordLength   = UART_WORDLENGTH_8B;
  huart2.Init.StopBits     = UART_STOPBITS_1;
  huart2.Init.Parity       = UART_PARITY_NONE;
  huart2.Init.Mode         = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief GPIO Initialization Function
  *
  * PB0  – Buzzer output (GPIO_MODE_OUTPUT_PP, already present)
  * LD2  – Status LED on Nucleo (PA5)
  * B1   – User button (PC13)
  * I2C1 pins (PB6/PB7) are configured by HAL_I2C_MspInit() automatically
  * USART1 pins (PA9/PA10) configured by HAL_UART_MspInit() automatically
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Default output levels */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);   /* Buzzer OFF */

  /* B1 user button (PC13) – falling edge, no pull */
  GPIO_InitStruct.Pin  = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /* LD2 status LED (PA5) */
  GPIO_InitStruct.Pin   = LD2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* PB0 – Buzzer (push-pull, low speed) */
  GPIO_InitStruct.Pin   = GPIO_PIN_0;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/* ---------------------------------------------------------------------- */
/* HAL callback – dispatch USART2 byte-received interrupts to the driver  */
/* ---------------------------------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    ECG_UART_RxCallback(huart);
}
/* USER CODE END 4 */

/**
  * @brief  Error handler – buzzer off, halt.
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  BUZZER_Off();
  __disable_irq();
  while (1) {}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
