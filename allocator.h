/* 
 * File:   allocator.h
 * Author: Ali
 *
 * Created on 10 ?????? 2023 ?., 12:25
 */

#ifndef ALLOCATOR_H
#define	ALLOCATOR_H


//SETTINGS--------------------
#define TOTAL_SIZE_BYTES 303
#define BLOCK_SIZE_BYTES 15

typedef unsigned int Address_t; 
typedef unsigned int Unit_t;
//----------------------------


typedef unsigned char BYTE;
extern BYTE* map;
void  allocator_init();
void* allocate();
void  free(void*);

#endif	/* ALLOCATOR_H */

