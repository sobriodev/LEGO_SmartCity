#ifndef TASK_DEF_H_
#define TASK_DEF_H_

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Main emWin GUI task
 *
 * \param pvParameters : Task parameter
 */
void vTaskGUI(void *pvParameters);

/*!
 * \brief Backlight task
 *
 * @param pvParameters : Task parameter
 */
void vTaskBacklight(void *pvParameters);

#endif /* TASK_DEF_H_ */
