/************************************************************************/
/* 
*
* Copyright (c) 2014, Digilent <www.digilentinc.com>
* Contact Digilent for the latest version.
*
* This program is free software; distributed under the terms of 
* BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    5/1/2014(KeithV): Created                                         */
/************************************************************************/
/************************************************************************/
/*                                                                      */
/*  Supports the WS2812 signaling to drive up to 1000 devices           */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/*                       Supported hardware:                            */
/*                                                                      */
/*  chipKIT WF32  Dout pin 11; unusable pins 12, 13-LED1                */
/*  chipKIT Max32 Dout pin 43 or (51 with JP4 in master);               */
/*          unusable pins 29,50,52                                      */
/*                                                                      */
/*  WARNING: currently this code assumes SPI2. Of the chipKIT boards    */
/*  this works on (WF32/Max32), the standard Arduino SPI just happens   */
/*  to always be SPI2, but this is NOT generalized code!                */
/*                                                                      */
/*  The spec says that Dout Vih = .7Vdd and Vdd = 6v-7v However...      */
/*  it seems to work with Vdd == 4.5v -> 5v and Vih == 3.3              */
/*  But this is "out of spec" operation. If you must be in              */
/*  spec you will need to put a level shifter on Dout to bring 3.3v     */
/*  up to .7Vdd. If your level shifter also inverts the data signal     */
/*  you can specify fInvert=true on begin() to invert the 3.3v signal   */
/*                                                                      */
/************************************************************************/
#include <WProgram.h>

#if !(defined(_BOARD_WF32_) ||  defined(_BOARD_FUBARINO_SD_)|| defined(_BOARD_MEGA_))
#error Board does not support needed DMA or SPI resources
#endif

// use this define to provide a pattern buffer big enough
// to support the number of devices in the string.
#define CBWS2812PATBUF(__cDevices)  (78 * __cDevices)

class WS2812 {
   
public:

    typedef struct _GRB
    {
        uint8_t green;
        uint8_t red;
        uint8_t blue;
    } GRB;

    WS2812();
    ~WS2812();

    bool begin(uint32_t cDevices, uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, bool fInvert=false);
    bool updateLEDs(GRB rgGRB[], uint32_t cPass = 5);
    void abortUpdate(void);
    void end(void);

private:

    typedef enum
    {
        INIT,
        WAITUPD,
        CONVGRB,
        INVERT,
        ENDUPD
    } UST;

    bool            _fInit;
    bool            _fInvert;
    uint32_t        _cDevices;
    uint32_t        _iNextDevice;
    uint8_t *       _pPatternBuffer;
    uint32_t        _cbPatternBuffer;
    uint32_t        _iByte;
    uint32_t        _iBit;
    GRB *           _pGRB;
    UST             _updateState;

    void init(void);
    void applyGRB(GRB& grb);
    void applyColor(uint8_t color);
    void applyBit(uint32_t fOne);
};


