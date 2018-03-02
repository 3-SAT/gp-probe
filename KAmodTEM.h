#ifndef __KAMODTEM_h
#define __KAMODTEM_h


#define TEMPERATURE		0x00
#define CONFIGURATION	0x01
#define HYSTERESIS		0x02
#define LIMIT_SET		0x03


void KAmodTEM_WriteReg(unsigned char idx, unsigned char RegAddr, int RegValue);
int  KAmodTEM_ReadReg(unsigned char idx, unsigned char RegAddr);
void KAmodTEM_Init(void);

#endif
