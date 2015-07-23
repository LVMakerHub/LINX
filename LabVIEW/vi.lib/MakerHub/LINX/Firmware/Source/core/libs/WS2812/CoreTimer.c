/************************************************************************/
/* 
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
#include <WProgram.h>

#define TICKSPERSHORTCHECK  (5 * CORE_TICK_RATE)        // 5ms
#define TICKSPERREFRESH     (30 * CORE_TICK_RATE)       // 30ms

#define KVA_2_PA(v) (((uint32_t) (v)) & 0x1fffffff)
#define read_count(dest) __asm__ __volatile__("mfc0 %0,$9" : "=r" (dest))

static uint32_t zeros           = 0;
static uint32_t ones            = 0xFFFFFFFF;
static uint32_t tWS2812LastRun  = 0;
static uint32_t fWS2812Updating = false;

/***    uint32_t WS2812TimerService(uint32_t curTime)
 *
 *    Parameters:
 *          The current core timer time
 *
 *    Return Values:
 *          The next core timer time to be called
 *
 *    Description:
 *          This is the CoreTimer routine to handle refreshing the 
 *          WS2812. This gets called every TICKSPERREFRESH
 *          unless it is behind on a refresh because of external factors
 *          then it is called every TICKSPERSHORTCHECK until it is refreshed.
 *
 * ------------------------------------------------------------ */
uint32_t WS2812TimerService(uint32_t curTime)
{
    uint32_t deltaTime = curTime - tWS2812LastRun;
 
    // it is time to refresh
    if(!fWS2812Updating && !DCH0CONbits.CHEN && deltaTime >= TICKSPERREFRESH)
    {
        uint32_t intState = disableInterrupts();
        DCH1CONbits.CHEN = 0;
        DCH0CONbits.CHEN = 1;
        restoreInterrupts(intState);

        tWS2812LastRun += (deltaTime / TICKSPERREFRESH) * TICKSPERREFRESH;
        return(tWS2812LastRun + TICKSPERREFRESH);
    }

    // if this is in a holding pattern for a really long time
    // don't let delta get too big and wrap the uint32_t counter
    if(deltaTime >= (2 * TICKSPERREFRESH))
    {
        tWS2812LastRun += TICKSPERREFRESH;
        deltaTime -= TICKSPERREFRESH;
    }

    // if we get here, we are trying to refresh and are running behind
    // check more often to get the refresh done.
    return(tWS2812LastRun + (((deltaTime / TICKSPERSHORTCHECK) + 1) * TICKSPERSHORTCHECK));
}


/***    InitWS2812(uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, uint32_t fInvert)
 *
 *    Parameters:
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
 *          True if the core timer can be aquired and initialization succeeded.
 *
 *    Description:
 *
 *      Initialize the SPI to 20MHz which is a 50ns clock
 *      This will enable .35us pattern resolution in the SPI
 *      shift register.
 *
 *      Also, initialize 2 DMA channels, one to shift out
 *      the pattern buffer, the other to maintain zeros
 *      for the restart / reset patteren (RES).
 *
 * ------------------------------------------------------------ */
uint32_t InitWS2812(uint8_t * pPatternBuffer, uint32_t cbPatternBuffer, uint32_t fInvert)
{
    // Disable SPI and DMA
    SPI2CON             = 0;
    DMACON              = 0;

    // set up SPI2
    SPI2CONbits.MSTEN   = 1;    // SPI in master mode
    SPI2CONbits.ENHBUF  = 1;    // enable 16 byte transfer buffer
    SPI2CONbits.STXISEL = 0b10; // trigger DMA event when the ENBUF is half empty
//    SPI2CONbits.DISSDI  = 1;  // Disable the SDI pin, allow it to be used for GPIO (not implemented on an MX6)
    SPI2STAT            = 0;    // clear status register
    SPI2BRG             = (__PIC32_pbClk / (2 * 20000000)) - 1;  // 20MHz, 50ns

    IEC1bits.SPI2RXIE   = 0;    // disable SPI interrupts
    IEC1bits.SPI2TXIE   = 0;    // disable SPI interrupts
    IEC1bits.SPI2EIE    = 0;    // disable SPI interrupts
    IFS1bits.SPI2RXIF   = 0;    // disable SPI interrupts
    IFS1bits.SPI2TXIF   = 0;    // disable SPI interrupts
    IFS1bits.SPI2EIF    = 0;    // disable SPI interrupts

    IEC1bits.DMA0IE     = 0;
    IFS1bits.DMA0IF     = 0;

    IEC1bits.DMA1IE     = 0;
    IFS1bits.DMA1IF     = 0;

    DMACONbits.ON       = 1;    // turn on the DMA controller
    IEC1CLR=0x00010000;         // disable DMA channel 0 interrupts
    IFS1CLR=0x00010000;         // clear existing DMA channel 0 interrupt flag

    // Set up DMA channel 0
    DCH0CON             = 0;    // clear it
    DCH0CONbits.CHAED   = 0;    // do not allow events to be remembered when disabled
    DCH0CONbits.CHAEN   = 0;    // do not Allow continuous operation
    DCH0CONbits.CHPRI   = 0b11; // highest priority

    DCH0ECON            = 0;    // clear it
    DCH0ECONbits.CHSIRQ = _SPI2_TX_IRQ;   // SPI2TX 1/2 empty notification
    DCH0ECONbits.SIRQEN = 1;    // enable IRQ transfer enables

    DCH0INT             = 0;    // do not trigger any events
    DCH0INTbits.CHBCIF  = 0;    // clear IF bit
    IPC9bits.DMA0IP     = 7;    // priority 7
    IPC9bits.DMA1IS     = 0;    // sub priority 0

    DCH0SSA             = KVA_2_PA(pPatternBuffer); // source address of transfer
    DCH0SSIZ            = cbPatternBuffer;          // number of bytes in source
    DCH0DSA             = KVA_2_PA(&SPI2BUF);       // destination address is the SPI2 buffer
    DCH0DSIZ            = 1;                        // 1 byte at the destination
    DCH0CSIZ            = 1;                        // only transfer 1 byte per event

    // Set up DMA channel 1
    DCH1CON             = 0;    // clear it
    DCH1CONbits.CHCHNS  = 0;    // allow chaining to the next higher priority DMA channel 0
    DCH1CONbits.CHCHN   = 1;    // Turn on chaining
    DCH1CONbits.CHAED   = 0;    // do not allow events to be remembered when disabled
    DCH1CONbits.CHAEN   = 1;    // turn on continuous operation
    DCH1CONbits.CHPRI   = 0b11; // highest priority

    DCH1ECON            = 0;    // clear it
    DCH1ECONbits.CHSIRQ = _SPI2_TX_IRQ;   // SPI2TX 1/2 empty notification
    DCH1ECONbits.SIRQEN = 1;    // enable IRQ transfer enables

    DCH1INT             = 0;    // do not trigger any events

    if(fInvert)
    {
        DCH1SSA         = KVA_2_PA(&ones);     // refresh cycle is inverted streaming 1s
    }
    else
    {
        DCH1SSA         = KVA_2_PA(&zeros);     // normal refresh cycle streaming 0s
    }
    DCH1SSIZ            = 1;                    // number of bytes in source

    DCH1DSA             = KVA_2_PA(&SPI2BUF);   // destination address is the SPI2 buffer
    DCH1DSIZ            = 1;                    // 1 byte at the destination
    DCH1CSIZ            = 1;                    // only transfer 1 byte per event

    // initial time for the core serivce routine
    read_count(tWS2812LastRun);

    // attach the core service routine
    if(attachCoreTimerService(WS2812TimerService))
    {
        // Enable DMA channel 1 and SPI 2
        // we enable in the refresh cycle until a pattern
        // is loaded in the main sketch.
        fWS2812Updating = true;
        IFS1bits.DMA0IF     = 0;
        IEC1bits.DMA0IE     = 1;
        DCH1CONbits.CHEN    = 1;    // turn DMA channel 1 ON; just zero output
        SPI2CONbits.ON      = 1;
        return(1);                  // success
    }

    // Things are not good, disable SPI and DMA
    SPI2CON             = 0;
    DMACON              = 0;

    // error out
    return(0);
}

/***    void EndWS2812(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          None
 *
 *    Description:
 *
 *      Disables the WS2812 controller
 *
 * ------------------------------------------------------------ */
void EndWS2812(void)
{
    SPI2CON             = 0;
    DMACON              = 0;
    DCH0CONbits.CHEN    = 0;
    DCH1CONbits.CHEN    = 0;
}

/***    uint32_t StartUpdate(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          Return true when the DMA is streaming the refresh cycle,
 *          false if still streaming the pattern out
 *
 *    Description:
 *
 *      Attempt to start an update cycle, hold the core timer
 *      service routine in the refresh cycle so the main pattern
 *      buffer can be updated.
 *
 * ------------------------------------------------------------ */
uint32_t StartUpdate(void)
{
    fWS2812Updating = DCH1CONbits.CHEN;
    return(fWS2812Updating);
}

/***    void EndUpdate(void)
 *
 *    Parameters:
 *          None
 *
 *    Return Values:
 *          none
 *
 *    Description:
 *
 *      Release the core timer service to update from the pattern buffer
 *
 * ------------------------------------------------------------ */
void EndUpdate(void)
{
    fWS2812Updating = false;
}

