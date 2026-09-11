#include "stm32f1xx_hal.h"

#define LED_PIN       GPIO_PIN_13
#define LED_GPIO_PORT GPIOC
#define BLINK_INTERVAL_MS 250U

static void SystemClock_Config(void);
static void Error_Handler(void);
static void GPIO_Init(void);

/* Global + volatile so the debugger's Live Watch engine can poll this
 * SRAM address while the CPU keeps running, without a breakpoint. */
volatile uint32_t blink_counter;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();

    uint32_t lastToggleTick = HAL_GetTick();

    while (1)
    {
        if ((HAL_GetTick() - lastToggleTick) >= BLINK_INTERVAL_MS)
        {
            lastToggleTick = HAL_GetTick();
            HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
            blink_counter++;
        }
    }
}

static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* LED is active-low: start with the pin HIGH (LED off) */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

/* Without this, SysTick_Handler stays aliased to Default_Handler (an
 * infinite loop) and the first SysTick interrupt hangs the CPU before
 * HAL_GetTick() ever advances. */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif
