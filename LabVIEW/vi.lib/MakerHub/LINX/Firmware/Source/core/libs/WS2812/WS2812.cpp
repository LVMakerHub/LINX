/************************************************************************/
/*                                                                      */
/*    WS2812.cpp                                                        */
/*                                                                      */
/*    WS2812 3.3v library to produce the WS2812 signals                 */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2014, Digilent Inc.                                     */
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
/*    5/1/2014(KeithV): Created                                        */
/************************************************************************/
/************************************************************************/
/*                                                                      */
/*  Supports the WS2812 signaling to drive up to 1000 devices           */
/*  The output signal is 3.3v, below that which is needed to            */
/*  to drive a WS2812, so a level shifter is needed to provide          */
/*  the correct voltages. The library also supports generating an       */
/*  inverted signal should the level shift also invert the signal       */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/*                       Supported hardware:                            */
/*                                                                      */
/*  chipKIT WF32  Dout pin 11; unusable pins 12, 13-LED1                */
/*  chipKIT Max32 Dout pin 43 or (51 with JP4 in master);               */
/*          unusable pins 29,50,52                                      */
/*  Fubarino SD 1.5 Dout pin 26                                         */
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
#include <WS2812.h>

extern "C" {
    uint32_t InitWS2812(uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, uint32_t fInvert);
    void EndWS2812(void);
    uint32_t StartUpdate(void);
    void EndUpdate(void);
}

WS2812::WS2812()
{
    init();
}

WS2812::~WS2812()
{
    end();
}

/***    void WS2812::init(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      Initializes all of the class variables
 *
 * ------------------------------------------------------------ */
void WS2812::init(void)
{
    _fInit              =   false;
    _fInvert            =   false;
    _cDevices           =   0;
    _pPatternBuffer     =   NULL;
    _cbPatternBuffer    =   0;
    _iBit               =   0;
    _iByte              =   0;
    _iNextDevice        =   0;
    _pGRB               =   NULL;
    _updateState        =   INIT;
}

/***    bool WS2812::begin(uint32_t cDevices, uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, bool fInvert)
 *
 *    Parameters:
 *          cDevices:   The number of devices in the WS2812 string / chain
 *
 *          pPatternBuffer: A pointer to the pattern buffer for the DMA to use
 *                          The application allocates this and should be 
 *                          CBWS2812PATBUF(__cDevices) bytes long.
 *
 *          cbPatternBuffer: This should be CBWS2812PATBUF(__cDevices) or larger
 *
 *          fInvert:        Because the WS2812 does not have a VinH <= 3.3 when Vcc >= 4.7v
 *                          External hardware may be needed to level shift the 3.3v SDO output
 *                          to a higher Data in signal to the WS2812. This level shifter may
 *                          be a simple transistor tied to 5v - 7v. The transistor will invert
 *                          the SDO output signal and to maintain the correct signal polarity
 *                          to the WS2812 you would need to invert the signal coming out of SDO.
 *                          If fInvert is true, the SDO output signal will be inverted from what
 *                          the WS2812 would normally take. By default, the is "false".
 *
 *    Return Values:
 *          True if the WS2812 library was successfully initialized
 *          False if it was not. Probably because the pattern buffer was not the correct size
 *                  or because there were no open slots in the CoreTimer Service Routines.
 *
 *    Description:
 *
 *      Initializes the WS2812 library and starts streaming a refresh cycle out on SDO
 *
 * ------------------------------------------------------------ */
bool WS2812::begin(uint32_t cDevices, uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, bool fInvert)
{
    if(_fInit)
    {
        return(true);
    }

    if(cDevices == 0 || pPatternBuffer == NULL || cbPatternBuffer < CBWS2812PATBUF(cDevices))
    {
        return(false);
    }

    init();
    _cDevices           =   cDevices;
     _pPatternBuffer    =   pPatternBuffer;
    _cbPatternBuffer    =   cbPatternBuffer;
    _fInit              =   InitWS2812(pPatternBuffer, cbPatternBuffer, fInvert);
    _fInvert            =   fInvert;

    if(!_fInit)
    {
        end();
    }

    return(_fInit);
}

/***    void WS2812::end(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      Terminates the WS2812 library and releases the DMA and SPI peripherals.
 *
 * ------------------------------------------------------------ */
void WS2812::end(void)
{
    EndWS2812();
    init();
}

/***    void  WS2812::abortUpdate(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      Terminates updating the pattern buffer to a new
 *      data stream to the WS2812. Because the pattern
 *      buffer is most likely in an unfinished state
 *      the WS2812 will have no valid pattern buffer to
 *      refresh the chain; so the DMA will be left in
 *      the reset cycle. Since the pattern isn't 
 *      being refreshed, the WS2812 may 
 *      incure noise on the chain and start displaying
 *      unexpected results. updateLEDs() should be called
 *      with a new pattern to reingage the pattern buffer
 *      on a regular refresh cycle.
 * ------------------------------------------------------------ */
void  WS2812::abortUpdate(void)
{
        _pGRB           = NULL;
        _iNextDevice    = 0;
        _iBit           = 0;
        _iByte          = 0;
        _updateState    = INIT;
}

/***    bool WS2812::updateLEDs(GRB rgGRB[], uint32_t cPass)
 *
 *    Parameters:
 *          rgGRB:  An array of GRB structures that contains the 
 *                  the value for each Green, Red, or Blue pixel in
 *                  the device. Values may be from 0 to 255.
 *                  This point must NOT change until updateLEDs() returns true.
 *
 *          cPass:  How many devices to convert in the pattern buffer per call to
 *                  updateLEDs(). This allows you to control how long you say in updateLEDs()
 *                  The default value is to convert 5 devices per call to updateLEDs().
 *
 *    Return Values:
 *          False while updateLEDs() is still working to convert devices.
 *          True when all devices have been converted.
 *
 *    Description:
 *
 *      This will return false until the DMA is stream the refresh cycle
 *      and until all devices have been converted into the pattern buffer.
 *      Once everything is done, this will return true and then you can change
 *      rgGRB to a new pattern. You need to repeatedly call updateLEDs() until it 
 *      returns true.
 *
 * ------------------------------------------------------------ */
bool WS2812::updateLEDs(GRB rgGRB[], uint32_t cPass)
{
    if(!_fInit)
    {
        return(false);
    }

    switch(_updateState)
    {
        case INIT:
            if(_pGRB == NULL)
            {
                _pGRB           = rgGRB;
                _iNextDevice    = 0;
                _iBit           = 0;
                _iByte          = 0;
                memset(_pPatternBuffer, 0, _cbPatternBuffer);
                _updateState = WAITUPD;
            }
            break;

        case WAITUPD:
            if(StartUpdate())
            {
                _updateState = CONVGRB;
            }
            break;

        case CONVGRB:
            if(_pGRB == rgGRB)
            {

                for(int i=0; i<cPass && _iNextDevice < _cDevices; i++, _iNextDevice++)
                {
                    applyGRB(rgGRB[_iNextDevice]);
                }

                if(_iNextDevice == _cDevices)
                {
                    if(_fInvert)
                    {
                        _updateState = INVERT;
                    }
                    else
                    {
                        _updateState = ENDUPD;
                    }
                }
            }
            break;

        case INVERT:
            for(int i=0; i<_cbPatternBuffer; i++)
            {
                _pPatternBuffer[i] = ~_pPatternBuffer[i];
            }
            _updateState = ENDUPD;
            break;

        case ENDUPD:
            abortUpdate();
            EndUpdate();
            return(true);
            break;

        default:
            break;
    }

    return(false);
}

/***    void WS2812::applyGRB(GRB& grb)
 *
 *    Parameters:
 *          grb:    a single device Green, Red, Blue element to be converted
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      A private method to convert 1 device into the pattern buffer
 *
 * ------------------------------------------------------------ */
void __attribute__((always_inline)) WS2812::applyGRB(GRB& grb)
{
    applyColor(grb.green);
    applyColor(grb.red);
    applyColor(grb.blue);
}

/***    void WS2812::applyColor(uint8_t color)
 *
 *    Parameters:
 *          color:    a value between 0 - 255, it is the 
 *                    intensity of a single color
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      A private method to convert 1 color into the pattern buffer
 *
 * ------------------------------------------------------------ */
void WS2812::applyColor(uint8_t color)
{
    int i = 0;

    for(i=7; i>=0; i--)
    {
        applyBit(((color & (1 << i)) != 0));
    }
}

/***    void WS2812::applyBit(uint32_t fOne)
 *
 *    Parameters:
 *          fOne:   True if a one is to be applied to the pattern buffer
 *                  False if a zero is to be applied to the pattern buffer
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      A private method to convert 1 or 0 into the pattern buffer
 *
 * ------------------------------------------------------------ */
void __attribute__((always_inline)) WS2812::applyBit(uint32_t fOne)
{
    int i = 0;

    // apply a one to the pattern buffer
    if(fOne)
    {
        i = 0;

        // .7us or 14 ones
        while(i < 14)
        {
            for(; _iBit<8 && i<14; _iBit++, i++)
            {
                _pPatternBuffer[_iByte] |= ((uint8_t)(1 << (7-_iBit)));
            }

            if(_iBit == 8)
            {
                _iBit = 0;
                _iByte++;
            }
        }

        //.6us or 12 zeros
        // since the buffer is init with zeros
        // just skip forward
        _iBit   += 12;
        _iByte  += _iBit / 8;
        _iBit   %= 8;
     }

    // apply a zero to the pattern buffer
    else
    {
        i = 0;

        // .35us or 7 ones
        while(i < 7)
        {
            for(; _iBit<8 && i<7; _iBit++, i++)
            {
                _pPatternBuffer[_iByte] |= ((uint8_t)(1 << (7-_iBit)));
            }

            if(_iBit == 8)
            {
                _iBit = 0;
                _iByte++;
            }
        }

        // .8us or 16 zeros
        // since the buffer is init with zeros
        // just skip forward
        _iBit   += 16;
        _iByte  += _iBit / 8;
        _iBit   %= 8;
    }
}

