#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


#define FLASH_DATA_ADDR  (0x10000 - 1024)

struct calibration_data
{
    int  magic;
    float mxg;       /* mag x gain */
    float myg;
    float mzg;
    int   mxo;
    int   myo;
    int   mzo;       /* mag z offset */
    
    int axo;
    int ayo;
    int azo; 
    
    int gxo;
    int gyo;
    int gzo; 
};


#endif
