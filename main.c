
#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "allocator.h"

#define MAX_TEST_PTR_STORE 30


void assert(const char*file, unsigned int cc){
//  while(1);
}


void task() {
  BYTE pCnt = 0;
  BYTE mCnt = 0;
  int iter = 0;
  int allocIndex = 0;
  int freeIndex = 0;
  int al, fr;

  Unit_t * ptrStore[MAX_TEST_PTR_STORE];
  memset(ptrStore, 0, sizeof (ptrStore));
  while (1) {
    al = rand() % 10;
    fr = rand() % 10;
    iter++;
    if (iter == 24)
      Nop();
    int anticycle = MAX_TEST_PTR_STORE + 1;
    for (int i = 0; i < al; i++) {
      Unit_t*ptr = allocate();
      if (ptr){
        do {
          allocIndex++;
          if (allocIndex >= MAX_TEST_PTR_STORE)allocIndex = 0;
        } while ((ptrStore[allocIndex] != 0) && (--anticycle));
        if (anticycle) {
          ptrStore[allocIndex] = ptr;
          for (int j = 0; j < BLOCK_SIZE_BYTES; j++) ptr[j] = allocIndex;
        }
      }
    }
    anticycle = MAX_TEST_PTR_STORE + 1;
    for (int i = 0; i < fr; i++) {
      do {
        freeIndex++;
        if (freeIndex >= MAX_TEST_PTR_STORE)freeIndex = 0;
      } while ((ptrStore[freeIndex] == 0) && (--anticycle));
      if (anticycle){
        Unit_t*ptr = ptrStore[freeIndex];
        ptrStore[freeIndex] = 0;
        for (int i = 0; i < BLOCK_SIZE_BYTES; i++) {
          if (ptr[i] != freeIndex) {
            assert(__FILE__, __LINE__);
          } 
        }
        free(ptr);
      }
    }

    pCnt = 0;
    mCnt = 0;
    for (int i = 0; i < MAX_TEST_PTR_STORE; i++)if (ptrStore[i])pCnt++;

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 8; j++)
        if ((map[i] & (1 << j)) == 0)mCnt++;
    }
    mCnt -= 4;
    if (pCnt != mCnt)
      Nop();
  }
}


int main() {
  allocator_init();
  xTaskCreate(task, "Tsk1", configMINIMAL_STACK_SIZE * 5, NULL, 2, NULL); 
  xTaskCreate(task, "Tsk2", configMINIMAL_STACK_SIZE * 5, NULL, 2, NULL); 
  portENABLE_INTERRUPTS();
  vTaskStartScheduler();  
 
}