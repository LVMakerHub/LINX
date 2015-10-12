/****************************************************************************************
**  Header file for LINX Device Lib (Shared Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_DEVICELIB_H
#define LINX_DEVICELIB_H

int LinxOpen();
int LinxClose();
int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);

#endif //LINX_DEVICELIB_H