#include <xc.h>
#include <math.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "allocator.h"

Unit_t pool[TOTAL_SIZE_BYTES];
Address_t totalBlocks = 0;

//Address_t topMap = 0;

BYTE* map;
BYTE* last;
BYTE* topMap;
SemaphoreHandle_t allocatorMutex;
void allocator_init(){
  memset(pool, 0, sizeof(pool));
  Address_t totblo = (TOTAL_SIZE_BYTES / BLOCK_SIZE_BYTES);
  Address_t mapsiz = ceilf((float)totblo/8/sizeof(Unit_t)); //Size of control map in Unit_t
  totalBlocks = (TOTAL_SIZE_BYTES - mapsiz)/BLOCK_SIZE_BYTES;
  map = (BYTE*) &pool[TOTAL_SIZE_BYTES - mapsiz];
  Address_t i;
  for(i = 0; i < totalBlocks/8; i++)map[i] = 0xFF;
  last = &map[i];
  int finBits = totalBlocks % 8;
  for(int j = 0; j < finBits; j++)*last |=(1 << j);
  topMap = map;
  allocatorMutex = xSemaphoreCreateMutex();
  
}

static int getBitNo(Address_t byte){
  for(int i = 0; i < 8; i++)if(byte & (1<< i))return i;
  return 0xFFFF;
}

void* allocate() {
  xSemaphoreTake(allocatorMutex, portMAX_DELAY);
  Address_t cc = totalBlocks / 8 + 1;
  BYTE*cursor = topMap + 1;
  do {
    if (cursor > last)cursor = map;
    if (*cursor) break;
    cursor++;
  } while (--cc);
  void*ptr = 0;
  if(cc){
    topMap = cursor;
    int bitNo = getBitNo(*cursor);
    *cursor &= ~(1 << bitNo);
    Address_t pooladr = ((bitNo + (Address_t)(cursor - map)* 8) * BLOCK_SIZE_BYTES);
    ptr = &pool[pooladr];
  }
  xSemaphoreGive(allocatorMutex);
  return ptr;
}

void free(void*ptr) {
  if(ptr == NULL)return;
  xSemaphoreTake(allocatorMutex, portMAX_DELAY);
  Address_t no = (ptr - (void*)pool) / BLOCK_SIZE_BYTES / sizeof(Unit_t);
  Address_t coarse = no / 8;
  Address_t fine = no - coarse * 8;
  map[coarse] |=(1 << fine);
  xSemaphoreGive(allocatorMutex);
}




