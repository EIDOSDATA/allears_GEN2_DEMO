/*
 * echo_flash_memory.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: ECHO
 */
#include <echo_flash_memory.h>
#include <echo_stim_setting.h>
#include "main.h"

/*
 * 1. Flash UNLOCK
 * 2. PAGE CALC
 * 3. PAGE ERASE >> NEW data WRITE
 * 4. Flash LOCK
 */
extern pwm_pulse_param_t ex_pwm_param;
extern uint64_t *ex_p64_pwm_param;

#define ECHO_DATA_WRITE                 *ex_p64_pwm_param

uint32_t first_page = 0, number_of_pages = 0, bank_number = 0;
uint32_t address = 0, page_error = 0;
__IO uint32_t data32 = 0, mem_program_status = 0;
__IO uint64_t data64 = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

static uint32_t Echo_GetPage(uint32_t addr);
static uint32_t Echo_GetBank(uint32_t addr);

static uint32_t Echo_GetPage(uint32_t addr)
{
	uint32_t page = 0;

	if (addr < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		/* Bank 1 */
		page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
	}
	else
	{
		/* Bank 2 */
		page = (addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}

static uint32_t Echo_GetBank(uint32_t addr)
{
	return FLASH_BANK_1;
}

HAL_StatusTypeDef Echo_Flash_Write()
{
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area*/
	first_page = Echo_GetPage(FLASH_USER_START_ADDR);
	number_of_pages = Echo_GetPage(FLASH_USER_END_ADDR) - first_page + 1;
	bank_number = Echo_GetBank(FLASH_USER_START_ADDR);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = bank_number;
	EraseInitStruct.Page = first_page;
	EraseInitStruct.NbPages = number_of_pages;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK)
	{
		/*Error occurred while page erase.*/
		return HAL_FLASH_GetError();
	}

	/* Program the user Flash area word by word*/
	address = FLASH_USER_START_ADDR;

	while (address < FLASH_USER_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, ECHO_DATA_WRITE)
				== HAL_OK)
		{
			address = address + 4;
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

HAL_StatusTypeDef Echo_Flash_Read()
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
