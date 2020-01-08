#include "OSAL_Nv.h"

void KeusThemeSwitchInit(void)
{

 KeusThemeSwitchMiniMemoryInit();
 KeusThemeSwitchMiniReadConfigDataIntoMemory();

}

/*

Memory

*/

/*********************************************************************
* @fn          KeusThemeSwitchMiniMemoryInit
* @brief       Initialize memory for storing clickId-SceneId mapping
* @return      success/fail
*/

bool KeusThemeSwitchMiniMemoryInit(void)
{
 for (uint8 i = 0; i < CLICK_TYPES; i++)
 {
   themeManager.btnThemeMap[i] = 255;
 }

 uint8 res = osal_nv_item_init(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, CLICK_TYPES, (void *)themeManager.btnThemeMap);

 if (res == SUCCESS || res == NV_ITEM_UNINIT)
 {
   return true;
 }
 else
 {
   return false;
 }
}

/*********************************************************************
* @fn          KeusThemeSwitchMiniReadConfigDataIntoMemory
* @brief       Read clickId-SceneId mapping from memory
* @return      success/fail
*/

bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void)
{
 uint8 res = osal_nv_read(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, 0, CLICK_TYPES, (void *)themeManager.btnThemeMap);

 if (res == SUCCESS)
 {
   return true;
 }
 else
 {
   return false;
 }
}

/*********************************************************************
* @fn          KeusThemeSwitchMiniWriteConfigDataIntoMemory
* @brief       Write clickId-SceneId mapping from memory
* @return      success/fail
*/

bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void)
{
 uint8 res = osal_nv_write(KEUS_EMBEDDEDSWITCH_BUTTONSCENEIDMAP_MEMORYID, 0, CLICK_TYPES, (void *)themeManager.btnThemeMap);

 if (res == SUCCESS)
 {
   return true;
 }
 else
 {
   return false;
 }
}