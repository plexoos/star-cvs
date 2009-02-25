 /***************************************************************************
 *
 * $Id: StTriggerData2009.cxx,v 2.7 2009/02/25 15:22:55 ullrich Exp $
 *
 * Author: Akio Ogawa,Jan 2009
 ***************************************************************************
 *
 * Description:  Concrete implementation of StTriggerData for 2009.
 *
 ***************************************************************************
 *
 * $Log: StTriggerData2009.cxx,v $
 * Revision 2.7  2009/02/25 15:22:55  ullrich
 * DSM codes for BBC earliest TAC and TAC-differences and changes to VPD part.
 *
 * Revision 2.6  2009/02/23 22:31:09  ullrich
 * Fixed problem when running over 2009 data (solution by Pibero) and new VPD access functions.
 *
 * Revision 2.5  2009/02/13 23:04:50  ullrich
 * Updates necessary for use in Online QA (P) plots.
 *
 * Revision 2.4  2009/02/11 23:33:55  jeromel
 * Modifications by Akio to support getDsm0_BEMCE and getDsm0_BEMCW as well as
 * getDsm1_BEMC. However, use of const=0 impose implementation (was not done
 * in years < 2009). Added methods with return 0.
 *
 * Revision 2.3  2009/02/09 19:31:17  jeromel
 * not y9 and not early y9 version + returns internalBusy
 *
 * Revision 2.2  2009/01/20 18:10:15  ullrich
 * Bug fix and new ZDC access functions.
 *
 * Revision 2.1  2009/01/14 17:56:14  ullrich
 * Initial Revision.
 *
 *
 **************************************************************************/
#include <string.h>
#include <assert.h>
#include <iostream>
#include "StTriggerData2009.h"

ClassImp(StTriggerData2009)

StTriggerData2009::StTriggerData2009()
{
    mYear=2009;
    mData=0;
}

StTriggerData2009::StTriggerData2009(const TriggerDataBlk2009* data, int run)
{
    mYear=2009;
    mRun = run;
    mData= new TriggerDataBlk2009;
    debug = 0;
    
    unsigned int ver = data->FormatVersion; 
    swapI(&ver);

    if (ver != y9FORMAT_VERSION  && ver != 0x08121140) {
        gMessMgr->Warning() << "StTriggerData2009: Data format version = " << data->FormatVersion
                            << " is different from program format version = " << y9FORMAT_VERSION
                            << endm;
        assert(0);
    }
    if (debug>0) printf("StTriggerData2009: version = %x\n",ver);
    
    unsigned int size = data->totalTriggerLength; 
    swapI(&size);
    if (size > y9MAX_TRG_BLK_SIZE) {
        gMessMgr->Warning() << "StTriggerData2009: Data length = " << size
                            << " is bigger than max = " << y9MAX_TRG_BLK_SIZE
                            << endm;
        assert(0);
    }
    if (debug>0) printf("StTriggerData2009: size = %d, maxsize = %d\n",size,y9MAX_TRG_BLK_SIZE);
    
    memcpy(mData,data,size); 
    memset((char*)mData+size,0,sizeof(TriggerDataBlk2009)-size);
    swapDataBlk(mData);
    if (debug>0){
        printf("EventDesc  length=%10d   offset=%10d\n",mData->EventDesc_ofl.length,mData->EventDesc_ofl.offset);
        printf("L1_DSM     length=%10d   offset=%10d\n",mData->L1_DSM_ofl.length,mData->L1_DSM_ofl.offset);
        printf("Summary    length=%10d   offset=%10d\n",mData->Summary_ofl.length,mData->Summary_ofl.offset);
    }
    
    EvtDesc=0; L1_DSM=0; TrgSum=0;
    if (mData->EventDesc_ofl.length > 0) EvtDesc = (EvtDescData2009*)((char*)mData + mData->EventDesc_ofl.offset);
    if (mData->L1_DSM_ofl.length > 0)    L1_DSM  = (L1_DSM_Data2009*)((char*)mData + mData->L1_DSM_ofl.offset);
    if (mData->Summary_ofl.length   > 0) TrgSum  = (TrgSumData2009* )((char*)mData + mData->Summary_ofl.offset);
    if (EvtDesc) swapEvtDesc(EvtDesc);
    if (L1_DSM)  swapL1_DSM(L1_DSM);
    if (TrgSum)  swapTrgSum(TrgSum);
    if (EvtDesc==0 || L1_DSM==0 || TrgSum==0){
        gMessMgr->Warning() << "StTriggerData2009: EvtDesc, L1_DSM or TrgSum is missing" << endm;
        assert(0);        
    }
    
    int npre  = numberOfPreXing();
    int npost = numberOfPostXing();
    if (npre<0 || npre>5 && npost<0 && npost>5){
        gMessMgr->Warning() << "StTriggerData2009: Invalid npre/post  = "
                            << npre << " / " << npost << endm;
        assert(0);
    }
    if (debug>0) printf("StTriggerData2009: pre=%d post=%d\n",npre,npost);
    
    memset(mBC1,0,sizeof(mBC1));
    memset(mMXQ,0,sizeof(mMXQ));
    memset(mMIX,0,sizeof(mMIX));
    memset(mBCW,0,sizeof(mBCW));
    memset(mBCE,0,sizeof(mBCE));
    memset(mFEQ,0,sizeof(mFEQ));
    memset(mBBC,0,sizeof(mBBC));
    memset(mBBQ,0,sizeof(mBBQ));
    memset(mFMS,0,sizeof(mFMS));
    memset(mQT1,0,sizeof(mQT1));
    memset(mQT2,0,sizeof(mQT2));
    memset(mQT3,0,sizeof(mQT3));
    memset(mQT4,0,sizeof(mQT4));
    memset(mxq,0,sizeof(mxq)); memset(tmxq,0,sizeof(tmxq));
    memset(feq,0,sizeof(feq)); memset(tfeq,0,sizeof(tfeq));
    memset(bbq,0,sizeof(bbq)); memset(tbbq,0,sizeof(tbbq));
    memset(qt1,0,sizeof(qt1)); memset(tqt1,0,sizeof(tqt1));
    memset(qt1,0,sizeof(qt2)); memset(tqt1,0,sizeof(tqt2));
    memset(qt1,0,sizeof(qt3)); memset(tqt1,0,sizeof(tqt3));
    memset(qt1,0,sizeof(qt4)); memset(tqt1,0,sizeof(tqt4));
    TrgOfflen2009* offlen;
    for (int i=0; i<1+npre+npost; i++){
        if (i==0)
            {offlen = mData->MainX;}
        else
            {offlen = (TrgOfflen2009*) ((char*)mData + mData->PrePostList[i-1]);}
        swapRawDetOfflen(offlen);
        if (offlen[y9BC1_CONF_NUM].length>0) {mBC1[i] = (BELayerBlock2009*)((char*)mData + offlen[y9BC1_CONF_NUM].offset); swapRawDet((DataBlock2009*)mBC1[i],y9BC1_CONF_NUM); }
        if (offlen[y9MXQ_CONF_NUM].length>0) {mMXQ[i] = (QTBlock2009*     )((char*)mData + offlen[y9MXQ_CONF_NUM].offset); swapRawDet((DataBlock2009*)mMXQ[i],y9MXQ_CONF_NUM); }
        if (offlen[y9MIX_CONF_NUM].length>0) {mMIX[i] = (MIXBlock2009*    )((char*)mData + offlen[y9MIX_CONF_NUM].offset); swapRawDet((DataBlock2009*)mMIX[i],y9MIX_CONF_NUM); }
        if (offlen[y9BCW_CONF_NUM].length>0) {mBCW[i] = (BWestBlock2009*  )((char*)mData + offlen[y9BCW_CONF_NUM].offset); swapRawDet((DataBlock2009*)mBCW[i],y9BCW_CONF_NUM); }
        if (offlen[y9BCE_CONF_NUM].length>0) {mBCE[i] = (BEastBlock2009*  )((char*)mData + offlen[y9BCE_CONF_NUM].offset); swapRawDet((DataBlock2009*)mBCE[i],y9BCE_CONF_NUM); }
        if (offlen[y9FEQ_CONF_NUM].length>0) {mFEQ[i] = (QTBlock2009*     )((char*)mData + offlen[y9FEQ_CONF_NUM].offset); swapRawDet((DataBlock2009*)mFEQ[i],y9FEQ_CONF_NUM); }
        if (offlen[y9BBC_CONF_NUM].length>0) {mBBC[i] = (BBCBlock2009*    )((char*)mData + offlen[y9BBC_CONF_NUM].offset); swapRawDet((DataBlock2009*)mBBC[i],y9BBC_CONF_NUM); }
        if (offlen[y9BBQ_CONF_NUM].length>0) {mBBQ[i] = (QTBlock2009*     )((char*)mData + offlen[y9BBQ_CONF_NUM].offset); swapRawDet((DataBlock2009*)mBBQ[i],y9BBQ_CONF_NUM); }
        if (offlen[y9FMS_CONF_NUM].length>0) {mFMS[i] = (FMSBlock2009*    )((char*)mData + offlen[y9FMS_CONF_NUM].offset); swapRawDet((DataBlock2009*)mFMS[i],y9FMS_CONF_NUM); }
        if (offlen[y9QT1_CONF_NUM].length>0) {mQT1[i] = (QTBlock2009*     )((char*)mData + offlen[y9QT1_CONF_NUM].offset); swapRawDet((DataBlock2009*)mQT1[i],y9QT1_CONF_NUM); }
        if (offlen[y9QT2_CONF_NUM].length>0) {mQT2[i] = (QTBlock2009*     )((char*)mData + offlen[y9QT2_CONF_NUM].offset); swapRawDet((DataBlock2009*)mQT2[i],y9QT2_CONF_NUM); }
        if (offlen[y9QT3_CONF_NUM].length>0) {mQT3[i] = (QTBlock2009*     )((char*)mData + offlen[y9QT3_CONF_NUM].offset); swapRawDet((DataBlock2009*)mQT3[i],y9QT3_CONF_NUM); }
        if (offlen[y9QT4_CONF_NUM].length>0) {mQT4[i] = (QTBlock2009*     )((char*)mData + offlen[y9QT4_CONF_NUM].offset); swapRawDet((DataBlock2009*)mQT4[i],y9QT4_CONF_NUM); }
        if (mMXQ[i]) decodeQT(mMXQ[i]->length/4, mMXQ[i]->data, mxq[i], tmxq[i]); 
        if (mFEQ[i]) decodeQT(mFEQ[i]->length/4, mFEQ[i]->data, feq[i], tfeq[i]); 
        if (mBBQ[i]) decodeQT(mBBQ[i]->length/4, mBBQ[i]->data, bbq[i], tbbq[i]); 
        if (mQT1[i]) decodeQT(mQT1[i]->length/4, mQT1[i]->data, qt1[i], tqt1[i]); 
        if (mQT2[i]) decodeQT(mQT2[i]->length/4, mQT2[i]->data, qt2[i], tqt2[i]); 
        if (mQT3[i]) decodeQT(mQT3[i]->length/4, mQT3[i]->data, qt3[i], tqt3[i]); 
        if (mQT4[i]) decodeQT(mQT4[i]->length/4, mQT4[i]->data, qt4[i], tqt4[i]); 
    }
    dump();
}

StTriggerData2009::~StTriggerData2009() {delete mData;}

unsigned int StTriggerData2009::version() const
{
    return EvtDesc->TrgDataFmtVer;
}

unsigned int StTriggerData2009::token() const
{
    return EvtDesc->TrgToken;
}

unsigned int StTriggerData2009::triggerWord() const
{
    return EvtDesc->TriggerWord;
}

unsigned int StTriggerData2009::actionWord() const
{
    return
        ( (unsigned short)(EvtDesc->actionWdTrgCommand) * 16 * 16 * 16 ) +
        ( (unsigned short)(EvtDesc->actionWdDaqCommand) * 16 * 16      ) +
        (                  EvtDesc->actionWdDetectorBitMask & 0x00ff   );
}

unsigned int StTriggerData2009::numberOfPreXing() const
{
    return EvtDesc->npre;
}

unsigned int StTriggerData2009::numberOfPostXing() const
{
    return EvtDesc->npost;
}

unsigned short StTriggerData2009::busyStatus() const{
    return EvtDesc->internalBusy;
}

unsigned short StTriggerData2009::dsmInput() const{
    return EvtDesc->DSMInput;
}

unsigned short StTriggerData2009::trgToken() const{
    return EvtDesc->TrgToken;
}

unsigned short StTriggerData2009::dsmAddress() const{
    return EvtDesc->DSMAddress;
}

unsigned short StTriggerData2009::mAddBits() const{
    return EvtDesc->addBits;
}

unsigned short StTriggerData2009::bcData(int channel) const{
    return L1_DSM->BCdata[channel];
}

unsigned short StTriggerData2009::lastDSM(int channel) const{
    return L1_DSM->lastDSM[channel];
}

unsigned short StTriggerData2009::tcuBits() const
{
    return EvtDesc->DSMInput;
}

unsigned int StTriggerData2009::bunchCounterHigh() const
{
    return EvtDesc->bunchXing_hi;
}

unsigned int StTriggerData2009::bunchCounterLow() const
{
    return EvtDesc->bunchXing_lo;
}

unsigned int StTriggerData2009::bunchId48Bit() const
{
    unsigned long long bxinghi,bxing1,bxinglo, bx;
    bxinghi = L1_DSM->BCdata[3];
    bxing1 =  L1_DSM->BCdata[10];
    bxinglo = (bxing1 << 16) + L1_DSM->BCdata[11];
    bx = (bxinghi << 32) + bxinglo;
    return (int)(bx % 120);
} 

unsigned int StTriggerData2009::bunchId7Bit() const
{
    int b7=0, b7dat; 
    b7dat = L1_DSM->BCdata[2];
    b7 = b7dat & 0x7f;
    return b7;
}

unsigned int StTriggerData2009::spinBit() const
{
    return (L1_DSM->lastDSM[7]/16)%256;
}

unsigned int StTriggerData2009::spinBitYellowFilled() const
{
    unsigned int sb = spinBit();
    return sb%2;
}

unsigned int StTriggerData2009::spinBitYellowUp() const
{
    unsigned int sb = spinBit();
    return (sb/2)%2;
}

unsigned int StTriggerData2009::spinBitYellowDown() const
{
    unsigned int sb = spinBit();
    return (sb/4)%2;
}

unsigned int StTriggerData2009::spinBitYellowUnpol() const
{
    unsigned int sb = spinBit();
    return (sb/8)%2;
}

unsigned int StTriggerData2009::spinBitBlueFilled() const
{
    unsigned int sb = spinBit();
    return (sb/16)%2;
}

unsigned int StTriggerData2009::spinBitBlueUp() const
{
    unsigned int sb = spinBit();
    return (sb/32)%2;
}

unsigned int StTriggerData2009::spinBitBlueDown() const
{
    unsigned int sb = spinBit();
    return (sb/64)%2;
}

unsigned int StTriggerData2009::spinBitBlueUnpol() const
{
    unsigned int sb = spinBit();
    return (sb/128)%2;
}

unsigned short StTriggerData2009::bbcADC(StBeamDirection eastwest, int pmt, int prepost) const
{
    const int addrmap[2][24] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   4, 4, 4, 4, 4, 4, 4, 4},
                                 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                   4, 4, 4, 4, 4, 4, 4, 4} };
    const int chmap[2][24]   = { { 0, 3, 8,16,19,24, 1, 2, 9,10,11,17,18,25,26,27,
                                   0, 1, 2, 3, 8, 9,10,11},
                                 { 0, 3, 8,16,19,24, 1, 2, 9,10,11,17,18,25,26,27,
                                   16,17,18,19,24,25,26,27} };
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && pmt>=1 && pmt<=24) return bbq[buffer][addrmap[eastwest][pmt-1]][chmap[eastwest][pmt-1]];
    return 0;
}

unsigned short StTriggerData2009::bbcTDC(StBeamDirection eastwest, int pmt, int prepost) const
{
    const int addrmap[2][24] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   4, 4, 4, 4, 4, 4, 4, 4},
                                 { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                   4, 4, 4, 4, 4, 4, 4, 4} };
    const int chmap[2][24]   = { { 0, 3, 8,16,19,24, 1, 2, 9,10,11,17,18,25,26,27,
                                   0, 1, 2, 3, 8, 9,10,11},
                                 { 0, 3, 8,16,19,24, 1, 2, 9,10,11,17,18,25,26,27,
                                   16,17,18,19,24,25,26,27} };
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && pmt>=1 && pmt<=24) return bbq[buffer][addrmap[eastwest][pmt-1]][chmap[eastwest][pmt-1]+4];
    return 0;
}

unsigned short StTriggerData2009::bbcADCSum(StBeamDirection eastwest, int prepost) const
{
    unsigned short sum=0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) for(int i=1; i<=16; i++) {sum+=bbcADC(eastwest,i,prepost);}
    return sum;
}

unsigned short StTriggerData2009::bbcADCSumLargeTile(StBeamDirection eastwest, int prepost) const
{
    unsigned short sum=0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) for(int i=17; i<=24; i++) {sum+=bbcADC(eastwest,i,prepost);}
    return sum;
}

unsigned short StTriggerData2009::bbcEarliestTDC(StBeamDirection eastwest, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if(buffer >=0){
      if(mBBC[buffer]){
            if(eastwest==east) {return mBBC[buffer]->BBClayer1[2]%4096;}
            else               {return mBBC[buffer]->BBClayer1[0]%4096;}
        }
    }
    return 0;
}

unsigned short StTriggerData2009::bbcTimeDifference() const
{
    return L1_DSM->VTX[3]%8192;
}

unsigned short StTriggerData2009::fpd(StBeamDirection eastwest, int module, int pmt, int prepost) const
{
    return 0;
}

unsigned short StTriggerData2009::fpdSum(StBeamDirection eastwest, int module) const
{
    return 0;
}

unsigned short StTriggerData2009::fpdLayer1DSMRaw(StBeamDirection eastwest, int channel, int prepost) const{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0){
        if (eastwest==east) { if (mMIX[buffer]) return mMIX[buffer]->FPDEastNSLayer1[channel]; }
        else                { if (mFMS[buffer]) return mFMS[buffer]->FMS[channel]; }
    }
    return 0;
}

unsigned short StTriggerData2009::fpdLayer2DSMRaw(int channel) const{
    if (channel<8) return L1_DSM->FPD[channel];
    return 0;
}

unsigned short StTriggerData2009::zdcAtChannel(int channel, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && channel>=0 && channel<32) return bbq[buffer][14][channel];
    return 0;
}

unsigned short StTriggerData2009::zdcAtAddress(int address, int prepost) const
{
    return zdcAtChannel(address,prepost);
}

unsigned short StTriggerData2009::zdcUnAttenuated(StBeamDirection eastwest, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) {
        if(eastwest == east) return bbq[buffer][14][2];
        else                 return bbq[buffer][14][18];
    }
    return 0;
}

unsigned short StTriggerData2009::zdcAttenuated(StBeamDirection eastwest, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) {
        if(eastwest == east) return bbq[buffer][14][3];
        else                 return bbq[buffer][14][19];
    }
    return 0;
}

unsigned short StTriggerData2009::zdcADC(StBeamDirection eastwest, int pmt, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && pmt>=1 && pmt<=3) {
        if(eastwest == east) {
            if(pmt == 1) return bbq[buffer][14][0];
            if(pmt == 2) return bbq[buffer][14][8];
            if(pmt == 3) return bbq[buffer][14][9];
        }
        else{
            if(pmt == 1) return bbq[buffer][14][16];
            if(pmt == 2) return bbq[buffer][14][24];
            if(pmt == 3) return bbq[buffer][14][25];
        }
    }
    return 0;
}

unsigned short StTriggerData2009::zdcTDC(StBeamDirection eastwest, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) {
        if(eastwest == east) return bbq[buffer][14][6];
        else                 return bbq[buffer][14][22];
    }
    return 0;
}

unsigned short StTriggerData2009::zdcPmtTDC(StBeamDirection eastwest, int pmt, int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && pmt>=1 && pmt<=3) {
        if(eastwest == east) {
            if(pmt == 1) return bbq[buffer][14][4];
            if(pmt == 2) return bbq[buffer][14][12];
            if(pmt == 3) return bbq[buffer][14][13];
        }
        else{
            if(pmt == 1) return bbq[buffer][14][20];
            if(pmt == 2) return bbq[buffer][14][28];
            if(pmt == 3) return bbq[buffer][14][29];
        }
    }
    return 0;
}

unsigned short StTriggerData2009::zdcHardwareSum(int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return bbq[buffer][14][11];
    return 0;
}

unsigned short StTriggerData2009::zdcSMD(StBeamDirection eastwest, int verthori, int strip, int prepost) const
{
    static const int zdcsmd_map[2][2][8] ={
        { { 31, 30, 29, 28, 27, 26, 25, 19} ,
          { 24, 23, 22, 21, 20, 16, 18, 17} } ,
        { { 15, 14, 13, 12, 11, 10,  9,  2} , 
          {  8,  7,  6,  5,  4,  3,  0,  1} }
    };
    if (verthori<0 || verthori>1) return 0;
    if (strip<1 || strip>8) return 0;  
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return mxq[buffer][4][zdcsmd_map[eastwest][verthori][strip-1]];
    return 0;
}

unsigned short StTriggerData2009::pp2ppADC(StBeamDirection eastwest, int vh, int udio, int ch, int prepost) const
{
    static const int map[2][2][2][2] ={ { { { 0, 1}, { 2, 3} } , { {16,17}, {18,19} } , } ,
                                        { { { 8, 9}, {10,11} } , { {24,25}, {26,27} } , } };
    if(vh<0   || vh>1)   return 0;
    if(udio<0 || udio>1) return 0;
    if(ch<0   || ch>1)   return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return mxq[buffer][2][map[eastwest][vh][udio][ch]];
    return 0;  
}

unsigned short StTriggerData2009::pp2ppTAC(StBeamDirection eastwest, int vh, int udio, int ch, int prepost) const
{
    static const int map[2][2][2][2] ={ { { { 0, 1}, { 2, 3} } , { {16,17}, {18,19} } , } ,
                                        { { { 8, 9}, {10,11} } , { {24,25}, {26,27} } , } };
    if(vh<0   || vh>1)   return 0;
    if(udio<0 || udio>1) return 0;
    if(ch<0   || ch>1)   return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return mxq[buffer][2][map[eastwest][vh][udio][ch]+4];
    return 0;  
}

unsigned short StTriggerData2009::bemcLayer1DSM(int channel, int prepost) const {
    const int n_bemc_layer1=48;
    if (channel<0 || channel >=n_bemc_layer1) {
        gMessMgr->Warning() << "Barrel DSM layer 1 out of range (" << channel << ")" << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBC1[buffer]) return mBC1[buffer]->BEMClayer1[channel];
    return 0;
} 

unsigned short StTriggerData2009::eemcLayer1DSM(int channel, int prepost) const {
    const int n_eemc_layer1=16;
    if (channel<0 || channel >=n_eemc_layer1) {
        gMessMgr->Warning() << "Endap DSM layer 1 out of range (" << channel << ")" << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBC1[buffer]) return mBC1[buffer]->EEMClayer1[channel];
    return 0;
} 

unsigned short StTriggerData2009::emcLayer2DSM(int channel) const {
    const int n_emc_layer2=8;
    if (channel<0 || channel >=n_emc_layer2) {
        gMessMgr->Warning() << "EMC DSM layer 2 out of range (" << channel << ")" << endm;
        return 0;
    }
    return L1_DSM->EMC[channel];
} 

unsigned char StTriggerData2009::bemcHighTower(int patch_id, int prepost) const {
    // Unpacking of Bemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=300; // Full barrel
    if ( patch_id < 0 || patch_id >= m_max_patch) {
        gMessMgr->Warning() << "Invalid Barrel patch id: " << patch_id << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) {
        int dsm=patch_id/10;
        int channel=patch_id%10;
        unsigned short trg_word;
        if (dsm>=15) {
            if (mBCE[buffer])
                trg_word=decodeEmc12bit(dsm-15,channel,mBCE[buffer]->BEMCEast);
            else
                return 0;
        }
        else {
            if (mBCW[buffer])
                trg_word=decodeEmc12bit(dsm,channel,mBCW[buffer]->BEMCWest);
            else
                return 0;
        }
        return trg_word & 0x3F;
    }
    return 0;
}

unsigned char StTriggerData2009::bemcJetPatch (int patch_id, int prepost) const
{
    // Unpacking of Bemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=300; // Full barrel
    if ( patch_id < 0 || patch_id >= m_max_patch) {
        gMessMgr->Warning() << "Invalid Barrel patch id: " << patch_id << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) {
        int dsm=patch_id/10;
        int channel=patch_id%10;
        unsigned short trg_word;
        if (dsm>=15) {
            if (mBCE[buffer])
                trg_word=decodeEmc12bit(dsm-15,channel,mBCE[buffer]->BEMCEast);
            else
                return 0;
        }
        else {
            if (mBCW[buffer])
                trg_word=decodeEmc12bit(dsm,channel,mBCW[buffer]->BEMCWest);
            else
                return 0;
        }
        return trg_word >> 6;
    }
    return 0;
}


unsigned char StTriggerData2009::eemcHighTower(int patch_id, int prepost) const
{
    // Unpacking of Eemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=90;
    if ( patch_id < 0 || patch_id >= m_max_patch) {
        gMessMgr->Warning() << "Invalid Endcap patch id" << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && mBC1[buffer]) {
        int dsm=patch_id/10;
        int channel=patch_id%10;
        unsigned short trg_word = decodeEmc12bit(dsm,channel,mBC1[buffer]->EEMC);
        return trg_word & 0x3F;
    }
    return 0;
}

unsigned char StTriggerData2009::eemcJetPatch (int patch_id, int prepost) const
{
    // Unpacking of Eemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=90;
    if ( patch_id < 0 || patch_id >= m_max_patch) {
        gMessMgr->Warning() << "Invalid Endcap patch id" << endm;
        return 0;
    }
    int buffer = prepostAddress(prepost);
    if (buffer >= 0 && mBC1[buffer]) {
        int dsm=patch_id/10;
        int channel=patch_id%10;
        unsigned short trg_word = decodeEmc12bit(dsm,channel,mBC1[buffer]->EEMC);
        return trg_word >> 6;
    }
    return 0;
}

unsigned char StTriggerData2009::bemcHighestTowerADC(int prepost) const {
    // Unpacking of Bemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=300; // Full barrel
    unsigned char h=0;
    for (int i=1; i<m_max_patch; i++){
        unsigned char hh=bemcHighTower(i,prepost);
        if (h>hh) h=hh;
    }
    return h;
}

unsigned char StTriggerData2009::eemcHighestTowerADC(int prepost) const {
    // Unpacking of Eemc trigger data (level 0 DSM input, trigger patches)
    const int m_max_patch=90;
    unsigned char h=0;
    for (int i=1; i<m_max_patch; i++){
        unsigned char hh=eemcHighTower(i,prepost);
        if (h>hh) h=hh;
    }
    return h;
}

char* StTriggerData2009::getTriggerStructure()
{
    return (char*) mData;
}

TriggerDataBlk2009* StTriggerData2009::getTriggerStructure2009()
{
    return mData;
}

int StTriggerData2009::getRawSize() const
{
    return  mData->totalTriggerLength;
}

unsigned char* StTriggerData2009::getDsm0_BEMCE(int prepost) const {  
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBCE[buffer]) return mBCE[buffer]->BEMCEast;
    return 0;
}

unsigned char* StTriggerData2009::getDsm0_BEMCW(int prepost) const {  
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBCW[buffer]) return mBCW[buffer]->BEMCWest;
    return 0;
}

unsigned short* StTriggerData2009::getDsm1_BEMC(int prepost) const {  
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBC1[buffer]) return mBC1[buffer]->BEMClayer1;
    return 0;
}

unsigned char* StTriggerData2009::getDsm0_EEMC(int prepost) const {  
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBC1[buffer]) return mBC1[buffer]->EEMC;
    return 0;
}

unsigned short int* StTriggerData2009::getDsm1_EEMC(int prepost) const{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if (mBC1[buffer]) return mBC1[buffer]->EEMClayer1;
    return 0;
}

unsigned short int  * StTriggerData2009::getDsm2_EMC() const{
    return   L1_DSM->EMC;
}

unsigned short int  * StTriggerData2009::getDsm3() const{
    return   L1_DSM->lastDSM;
}

int StTriggerData2009::L2ResultsOffset(StL2AlgorithmId id) const
{
    switch(id) {
    default: return -999999999;
    }
}

bool StTriggerData2009::isL2Triggered(StL2TriggerResultType id) const
{
    return false;
}    

unsigned int StTriggerData2009::l2ResultLength() const
{
    return sizeof(TrgSum->L2Result)/sizeof(unsigned int);
}

const unsigned int* StTriggerData2009::l2Result() const
{
    return TrgSum->L2Result;
}

unsigned short StTriggerData2009::vpdADC(StBeamDirection eastwest, int pmt, int prepost) const
{
    static const int map[16] = {0, 1, 2, 3, 8, 9,10,11,16,17,18,19,24,25,26,27};
    if (pmt<1 || pmt>16) return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return bbq[buffer][10+(int)eastwest*2][map[pmt-1]];
    return 0;
}

unsigned short StTriggerData2009::vpdTDC(StBeamDirection eastwest, int pmt, int prepost) const
{
    static const int map[16] = {0, 1, 2, 3, 8, 9,10,11,16,17,18,19,24,25,26,27};
    if (pmt<1 || pmt>16) return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return bbq[buffer][10+(int)eastwest*2][map[pmt-1]+4];
    return 0;
}

unsigned short StTriggerData2009::vpdADCHighThr(StBeamDirection eastwest, int pmt, int prepost) const
{
    static const int map[16] = {0, 1, 2, 3, 8, 9,10,11,16,17,18,19,24,25,26,27};
    if (pmt<1 || pmt>16) return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return bbq[buffer][6+(int)eastwest*2][map[pmt-1]];
    return 0;
}

unsigned short StTriggerData2009::vpdTDCHighThr(StBeamDirection eastwest, int pmt, int prepost) const
{
    static const int map[16] = {0, 1, 2, 3, 8, 9,10,11,16,17,18,19,24,25,26,27};
    if (pmt<1 || pmt>16) return 0;
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) return bbq[buffer][6+(int)eastwest*2][map[pmt-1]+4];
    return 0;
}

unsigned short StTriggerData2009::vpdEarliestTDC(StBeamDirection eastwest) const
{
  int buffer = 0;
  if(mBBC[buffer]){
    if(eastwest==east) {return mBBC[buffer]->VPD[6]%4096;}
    else               {return mBBC[buffer]->VPD[4]%4096;}
  }
  return 0;
}

unsigned short StTriggerData2009::vpdTimeDifference() const
{
  return L1_DSM->VTX[6]%8192;
}

unsigned short StTriggerData2009::nQTdata(int prepost) const
{
    return 0;
}

unsigned int* StTriggerData2009::QTdata(int prepost) const
{
    return 0;
}

unsigned char* StTriggerData2009::getDsm_FMS(int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if(mFMS[buffer]) return mFMS[buffer]->FMS;
    return 0;  
}

unsigned short int* StTriggerData2009::getDsm1_FMS(int prepost) const
{
    int buffer = prepostAddress(prepost);
    if (buffer >= 0) if(mMIX[buffer]) return mMIX[buffer]->FPDEastNSLayer1;
    return 0;
}

unsigned short int* StTriggerData2009::getDsm2_FMS() const {return L1_DSM->FPD;}

unsigned short StTriggerData2009::mtdAtAddress(int address, int prepost) const
{
    return 0;
}

unsigned short StTriggerData2009::mtdAdc(StBeamDirection eastwest, int pmt, int prepost) const
{
    return 0;
}

unsigned short StTriggerData2009::mtdTdc(StBeamDirection eastwest, int pmt, int prepost) const
{
    return 0;
}

unsigned short StTriggerData2009::tofAtAddress(int address, int prepost) const 
{
    return 0;
}

unsigned short StTriggerData2009::tofMultiplicity(int prepost) const 
{
    return 0;
}

void StTriggerData2009::dump() const
{
    printf("***** StTriggerData Dump *****\n");
    printf(" Year=%d  Version=%x\n",year(),version());
    printf(" %d pre and %d post crossing data available\n",numberOfPreXing(),numberOfPostXing());
    printf(" Token=%d  TriggerWord=%x  ActionWord=%x  BusyStatus=%x\n",
	 token(), triggerWord(), actionWord(), busyStatus());    
    printf(" TUC Bits=%d  : ",tcuBits());
    for (int i=0; i<16; i++) {printf(" %d",(tcuBits()>>(15-i))%2);}; printf("\n");
    printf(" BunchId 7bit=%d  48bit=%d\n",bunchId7Bit(), bunchId48Bit());  
    printf(" Spin Bits=%d  : ",spinBit());
    for (int i=0; i<8; i++) {printf(" %d",(spinBit()>>(7-i))%2);}; printf("\n");
    //    printf(" CTB ADC : ");       for (int i=0; i<240;i++){ printf("%d ",ctb(i,0));      }; printf("\n");
    printf(" BBC East ADC : ");  for (int i=1; i<=24;i++){ printf("%d ",bbcADC(east,i,0)); }; printf("\n");
    printf(" BBC West ADC : ");  for (int i=1; i<=24;i++){ printf("%d ",bbcADC(west,i,0)); }; printf("\n");
    printf(" BBC East TAC : ");  for (int i=1; i<=16;i++){ printf("%d ",bbcTDC(east,i,0)); }; printf("\n");
    printf(" BBC West TAC : ");  for (int i=1; i<=16;i++){ printf("%d ",bbcTDC(west,i,0)); }; printf("\n");
    for (int i=-numberOfPreXing(); i<=static_cast<int>(numberOfPostXing()); i++){
        printf(" BBC Sums %d xing : ",i);  
        printf("East=%d  West=%d   Large tile East=%d  West=%d\n",
    	     bbcADCSum(east,i),bbcADCSum(west,i),
    	     bbcADCSumLargeTile(east,i),bbcADCSumLargeTile(west,i));
    }
    printf(" BBC Earilest : ");  printf("East=%d  West=%d  Difference+256=%d\n",
    				bbcEarliestTDC(east,0),bbcEarliestTDC(west,0),bbcTimeDifference());
    //printf(" FPD East North   : ");for (int i=1; i<=49;i++){ printf("%d ",fpd(east,0,i,0));  }; printf("\n");
    //printf(" FPD East South   : ");for (int i=1; i<=49;i++){ printf("%d ",fpd(east,1,i,0));  }; printf("\n");
    //printf(" FPD East Top     : ");for (int i=1; i<=25;i++){ printf("%d ",fpd(east,2,i,0));  }; printf("\n");
    //printf(" FPD East Bottom  : ");for (int i=1; i<=25;i++){ printf("%d ",fpd(east,3,i,0));  }; printf("\n");
    //printf(" FPD East North PS: ");for (int i=1; i<= 7;i++){ printf("%d ",fpd(east,4,i,0));  }; printf("\n");
    //printf(" FPD East South PS: ");for (int i=1; i<= 7;i++){ printf("%d ",fpd(east,5,i,0));  }; printf("\n");
    //printf(" FPD West South   : ");for (int i=1; i<=49;i++){ printf("%d ",fpd(west,1,i,0));  }; printf("\n");
    //printf(" FPD West Bottom  : ");for (int i=1; i<=25;i++){ printf("%d ",fpd(west,3,i,0));  }; printf("\n");
    //printf(" FPD West South PS: ");for (int i=1; i<= 7;i++){ printf("%d ",fpd(west,5,i,0));  }; printf("\n");
    //printf(" FPD Sums East    : ");for (int j=0; j<4 ;j++) printf("%d ",fpdSum(east,j));        printf("\n");
    //printf(" FPD Sums West    : ");for (int j=0; j<4 ;j++) printf("%d ",fpdSum(west,j));        printf("\n");
    printf(" ZDC Sum(A) East  : ");printf("%d ",zdcAttenuated(east));        printf("\n");
    printf(" ZDC Sum(A) West  : ");printf("%d ",zdcAttenuated(west));        printf("\n");
    printf(" ZDC Sum(UA) East : ");printf("%d ",zdcUnAttenuated(east));      printf("\n");
    printf(" ZDC Sum(UA) West : ");printf("%d ",zdcUnAttenuated(west));      printf("\n");
    printf(" VPD E Earliest TAC : %d\n", vpdEarliestTDC(east));
    printf(" VPD W Earliest TAC : %d\n", vpdEarliestTDC(west));
    printf(" VPD TimeDifference : %d\n", vpdTimeDifference());
    printf(" L2 result : \n"); 
    for (int j=0; j<4 ;j++) { for (int k=0; k<16; k++) {printf("%u ",*(l2Result()+j*16+k)); } printf("\n");}
    printf("\n");
    printf("***** StTriggerData Dump *****\n");
}
