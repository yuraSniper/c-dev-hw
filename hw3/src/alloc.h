#ifndef ALLOC_H
#define ALLOC_H

#include <stdint.h>
#include <stdbool.h>

bool mstart();
void mstop();
void * alloc(uint32_t size);
void mfree(void * ptr);

#endif //ALLOC_H
