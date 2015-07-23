/************************************************************************/
/*                                                                      */
/*    LBling.pde -- Larissa's Bling                                     */
/*                                                                      */
/*    Test code to verify the WS2812 bit stream                         */
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

#define CDEVICES 20

WS2812::GRB rgGRB[CDEVICES] =
{
    {0,     0,      0xFF    },
    {0,     0xFF,   0       },
    {0xFF,  0,      0       },
    {0,     0xFF,   0xFF    },
    {0xFF,  0,      0       },
    {0,     0,      0xFF    },
    {0x10,  0,      0       },
    {0,     0x10,   0       },
    {0,     0,      0x10    },
    {0,     0xFF,   0xFF    },
    {0,     0xFF,   0       },
    {0x55,  0x55,   0x55    },
    {0,     0,      0xFF    },
    {0x20,  0x20,   0       },
    {0x01,  0,      0       },
    {0,     0x01,   0       },
    {0,     0,      0x01    },
    {0,     0xFF,   0       },
    {0xFF,  0,      0       },
    {0,     0,      0xFF    }
};

uint32_t    tStart  = 0;
uint32_t    led     = HIGH;

WS2812      ws2812;
uint8_t     rgbPatternBuffer[CBWS2812PATBUF(CDEVICES)];

typedef enum {
    LOADPAT,
    SHIFT,
    WAIT,
    SPIN
} STATE;

STATE state = LOADPAT;
uint32_t tWaitShift = 0;
#define MSSHIFT 250

/***    void setup()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      Initialize the LED output pins and SPI and DMA
 *      
 * ------------------------------------------------------------ */
void setup() 
{                
    // initialize the digital pin as an output.
    // Pin PIN_LED1 and PIN_LED2 have LEDs connecton on most boards:
    // Early Max32 boards do not have a LED2, so just comment LED2 out 
    // if LED 2 does not exist.
    pinMode(PIN_LED1, OUTPUT); 
    digitalWrite(PIN_LED1, led);

    tStart = millis();

    ws2812.begin(CDEVICES, rgbPatternBuffer, sizeof(rgbPatternBuffer), false);
    tWaitShift = millis();
}

/***    void loop()
 *
 *    Parameters:
 *          None
 *              
 *    Return Values:
 *          None
 *
 *    Description: 
 *    
 *      Arduino loop function.
 *      
 *      Toggles LED 1 & 2 back and forth 
 *      Each blink is a half a second
 *      Total cycle is 1 second.
 *
 * ------------------------------------------------------------ */
void loop() 
{
    WS2812::GRB grbT;

    switch(state)
    {
        case LOADPAT:
            if(ws2812.updateLEDs(rgGRB))
            {
                state = SHIFT;
            }
            break;

        case SHIFT:
            memcpy(&grbT, &rgGRB[CDEVICES-1], sizeof(WS2812::GRB));
            for(int i = CDEVICES-2; i>=0; i--)
            {
                memcpy(&rgGRB[i+1], &rgGRB[i], sizeof(WS2812::GRB));
            }
            memcpy(&rgGRB[0], &grbT, sizeof(WS2812::GRB));
            state = WAIT;
            break;

        case WAIT:
            if(millis() - tWaitShift >= MSSHIFT)
            {
                tWaitShift = millis();
                state = LOADPAT;
            }
            break;

        case SPIN:
        default:
            break;

    }

    if(millis() - tStart > 500)
    {
        led ^= HIGH;
        digitalWrite(PIN_LED1, led);
        tStart = millis();
    }
}
