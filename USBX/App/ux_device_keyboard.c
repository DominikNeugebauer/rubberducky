/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_keyboard.c
  * @author  MCD Application Team
  * @brief   USBX Device HID Keyboard applicative source file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "ux_device_keyboard.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN PV */

// active HID instance, NULL when unplugged
static UX_SLAVE_CLASS_HID *keyboard_hid = UX_NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  USBD_HID_Keyboard_Activate
  *         This function is called when insertion of a HID Keyboard device.
  * @param  hid_instance: Pointer to the hid class instance.
  * @retval none
  */
VOID USBD_HID_Keyboard_Activate(VOID *hid_instance)
{
  /* USER CODE BEGIN USBD_HID_Keyboard_Activate */
  keyboard_hid = (UX_SLAVE_CLASS_HID *)hid_instance;   // host connected
  /* USER CODE END USBD_HID_Keyboard_Activate */

  return;
}

/**
  * @brief  USBD_HID_Keyboard_Deactivate
  *         This function is called when extraction of a HID Keyboard device.
  * @param  hid_instance: Pointer to the hid class instance.
  * @retval none
  */
VOID USBD_HID_Keyboard_Deactivate(VOID *hid_instance)
{
  /* USER CODE BEGIN USBD_HID_Keyboard_Deactivate */
  UX_PARAMETER_NOT_USED(hid_instance);
  keyboard_hid = UX_NULL;   // host gone
  /* USER CODE END USBD_HID_Keyboard_Deactivate */

  return;
}

/**
  * @brief  USBD_HID_Keyboard_SetReport
  *         This function is invoked when the host sends a HID SET_REPORT
  *         to the application over Endpoint 0.
  * @param  hid_instance: Pointer to the hid class instance.
  * @param  hid_event: Pointer to structure of the hid event.
  * @retval status
  */
UINT USBD_HID_Keyboard_SetReport(UX_SLAVE_CLASS_HID *hid_instance,
                                 UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN USBD_HID_Keyboard_SetReport */
  UX_PARAMETER_NOT_USED(hid_instance);
  UX_PARAMETER_NOT_USED(hid_event);
  /* USER CODE END USBD_HID_Keyboard_SetReport */

  return status;
}

/**
  * @brief  USBD_HID_Keyboard_GetReport
  *         This function is invoked when host is requesting event through
  *         control GET_REPORT request.
  * @param  hid_instance: Pointer to the hid class instance.
  * @param  hid_event: Pointer to structure of the hid event.
  * @retval status
  */
UINT USBD_HID_Keyboard_GetReport(UX_SLAVE_CLASS_HID *hid_instance,
                                 UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN USBD_HID_Keyboard_GetReport */
  UX_PARAMETER_NOT_USED(hid_instance);
  UX_PARAMETER_NOT_USED(hid_event);
  /* USER CODE END USBD_HID_Keyboard_GetReport */

  return status;
}

/* USER CODE BEGIN 1 */

// 8-byte report: [0]=modifiers [1]=0 [2..7]=keycodes (up to 6)
// one keypress = key report + empty report (release)

// queue one report; sent later by ux_system_tasks_run(). UX_SUCCESS if queued.
static UINT KEYBOARD_SendReport(UCHAR modifier, UCHAR keycode)
{
  UX_SLAVE_CLASS_HID_EVENT hid_event = {0};

  if (keyboard_hid == UX_NULL)
  {
    return UX_ERROR;   // not connected
  }

  hid_event.ux_device_class_hid_event_length    = 8;
  hid_event.ux_device_class_hid_event_buffer[0] = modifier;
  hid_event.ux_device_class_hid_event_buffer[2] = keycode;

  return ux_device_class_hid_event_set(keyboard_hid, &hid_event);
}

// type 'A' (example)
UINT KEYBOARD_TypeA(void)
{
  UINT status = KEYBOARD_SendReport(0x02, 0x04);   // shift + a
  if (status != UX_SUCCESS)
  {
    return status;
  }
  return KEYBOARD_SendReport(0x00, 0x00);   // release
}

// type one key: press then release
UINT KEYBOARD_TypeChar(UCHAR modifier, UCHAR keycode)
{
  UINT status = KEYBOARD_SendReport(modifier, keycode);
  if (status != UX_SUCCESS)
  {
    return status;
  }
  return KEYBOARD_SendReport(0x00, 0x00);
}

// type a string, a-z / A-Z only. stops on the first unknown char.
UINT KEYBOARD_TypeString(const char *str)
{
  UINT status = UX_SUCCESS;

  while (*str != '\0')
  {
    if (*str >= 'a' && *str <= 'z')
    {
      status = KEYBOARD_TypeChar(0x00, *str - 'a' + 0x04);
    }
    else if (*str >= 'A' && *str <= 'Z')
    {
      status = KEYBOARD_TypeChar(0x02, *str - 'A' + 0x04);   // + shift
    }
    else
    {
      status = UX_ERROR;
    }

    if (status != UX_SUCCESS)
    {
      return status;
    }

    str++;
  }

  return UX_SUCCESS;
}

/* USER CODE END 1 */
