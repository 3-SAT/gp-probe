//***************** (c) 2009 BTC Korporacja ***********************************
//                     http://www.kamami.com
// PROJECT  : KAmodTEM Library
// COMPILER : WinAVR
//
//******************************************************************************
//
//    THE SOFTWARE INCLUDED IN THIS FILE IS FOR GUIDANCE ONLY.
//    BTC KORPORACJA SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
//    OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
//    FROM USE OF THIS SOFTWARE.
//
//******************************************************************************
//
// Basic function library for KAmodTEM expansion module. 
// Requires TWI functions library.
//
//******************************************************************************
#include "TWI.h"
//-----------------------------------------------------------------------------
// Write register routine
//-----------------------------------------------------------------------------
void KAmodTEM_WriteReg(unsigned char idx, unsigned char RegAddr, int RegValue)
{
TWI_Start();
TWI_Write(0x90 | (idx << 1));
TWI_Write(RegAddr);
TWI_Write(RegValue);
}
//-----------------------------------------------------------------------------
// Read register routine
//-----------------------------------------------------------------------------
unsigned int KAmodTEM_ReadReg(unsigned char idx, unsigned char RegAddr)
{
unsigned char tmpL, tmpH;
TWI_Start();
TWI_Write(0x90 | (idx << 1));
TWI_Write(RegAddr);
TWI_Start();
TWI_Write(0x91 | (idx << 1));
tmpH = TWI_Read(ACK);
tmpL = TWI_Read(NACK);
TWI_Stop();
return (unsigned int)(tmpL + (tmpH << 8));
}
//-----------------------------------------------------------------------------
// Used hardware initialization routine
//-----------------------------------------------------------------------------
void KAmodTEM_Init(void)
{
TWI_Init();
}
//-----------------------------------------------------------------------------
// End of file
//-----------------------------------------------------------------------------
