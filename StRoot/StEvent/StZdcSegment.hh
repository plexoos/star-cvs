/***************************************************************************
 *
 * $Id: StZdcSegment.hh,v 1.1 1999/01/15 20:40:31 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StZdcSegment.hh,v $
 * Revision 1.1  1999/01/15 20:40:31  wenaus
 * Commit Thomas' original code
 *
 **************************************************************************/
#ifndef StZdcSegment_hh
#define StZdcSegment_hh

class StZdcSegment {
public:
    StZdcSegment();
    StZdcSegment(short id, float a, float t);
    ~StZdcSegment();
    // StZdcSegment(const StZdcSegment &right);
    // const StZdcSegment & operator=(const StZdcSegment &right);

    short id() const;
    float adc() const;
    float tdc() const;

    void setId(short);
    void setAdc(float);
    void setTdc(float);
    
protected:
    short mId;
    float mAdc;
    float mTdc;
};

inline short StZdcSegment::id() const { return mId; }

inline float StZdcSegment::adc() const { return mAdc; }

inline float StZdcSegment::tdc() const { return mTdc; }

#endif
