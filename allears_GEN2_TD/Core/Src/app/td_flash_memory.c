/*
 * echo_flash_memory.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */
#include <td_flash_memory.h>
#include <td_stim_setting.h>
#include "main.h"

/*
 * 1. Flash UNLOCK
 * 2. PAGE CALC
 * 3. PAGE ERASE >> NEW data WRITE
 * 4. Flash LOCK
 */
extern pwm_pulse_param_t ex_pwm_param;
extern uint64_t *ex_p64_pwm_param;

#define DATA_WRITE                 *ex_p64_pwm_param

uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0, MemoryProgramStatus = 0;
__IO uint64_t data64 = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

static uint32_t td_GetPage(uint32_t Address);
static uint32_t td_GetBank(uint32_t Address);

static uint32_t td_GetPage(uint32_t Addr)
{
	uint32_t page = 0;

	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		/* Bank 1 */
		page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
	}
	else
	{
		/* Bank 2 */
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}

static uint32_t td_GetBank(uint32_t Addr)
{
	return FLASH_BANK_1;
}

HAL_StatusTypeDef td_Flash_Write()
{
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/
	FirstPage = td_GetPage(FLASH_USER_START_ADDR);
	NbOfPages = td_GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
	BankNumber = td_GetBank(FLASH_USER_START_ADDR);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = BankNumber;
	EraseInitStruct.Page = FirstPage;
	EraseInitStruct.NbPages = NbOfPages;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/*Error occurred while page erase.*/
		return HAL_FLASH_GetError();
	}

	/* Program the user Flash area word by word*/
	Address = FLASH_USER_START_ADDR;

	while (Address < FLASH_USER_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, DATA_WRITE)
				== HAL_OK)
		{
			Address = Address + 4;
		}
		/* Error occurred while writing data in Flash memory.
		 User can add here some code to deal with this error */
		else
		{
			return HAL_FLASH_GetError();
		}
	}
	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return HAL_OK;
}

HAL_StatusTypeDef td_Flash_Read()
{
	memcpy(&ex_pwm_param, (pwm_pulse_param_t*) FLASH_USER_START_ADDR,
			sizeof(ex_pwm_param));
	if (ex_pwm_param.dead_time == 0 && ex_pwm_param.pulse_freq == 0
			&& ex_pwm_param.pulse_width == 0)
	{
		return HAL_ERROR;
	}
	else
	{
		return HAL_OK;
	}
}

/*
 HAL_StatusTypeDef Echo_Flash_Read()
 {
 Address = FLASH_USER_START_ADDR;
 MemoryProgramStatus = 0x0;
 while (Address < FLASH_USER_END_ADDR)
 {
 data64 = *(__IO uint64_t*) Address;

 if (data64 != DATA_WRITE)
 {
 MemoryProgramStatus++;
 }
 Address = Address + 8;
 }
 if (MemoryProgramStatus == 0)
 {
 return HAL_ERROR;
 }
 return HAL_OK;
 }
 */
