
/*
* file api_errors.h
*/

#ifndef __API_ERRORS_H__
#define __API_ERRORS_H__

/** \addtogroup Errors
* @{
*/
/*
 * Квитирование на ячейку ЦОС
 */
#define OK            0x0U
#define ALL_UP        0x1U
#define OVERHEAT      0xffU
#define TURN_ON       0xEEU



/** No Error */
#define API_ERROR_OK 0
// Errors
#define API_ERROR_INVALID_HANDLE_PTR                1 //invalid argument
#define API_ERROR_INVALID_PARAM                     2 //invalid parameter
#define FAILED_TO_SET_PWDN_PIN_MODE                 3
#define FAILED_TO_SET_CLK_DUTY_CYCLE_STABILIZER     4
#define FAILED_TO_SET_INPUT_CLK_CONFIG              5
#define FAILED_TO_SET_ADC_INPUT_CONFIG              6

/** @} */
#endif /* !__API_ERRORS_H__ */


