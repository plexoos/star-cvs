/***************************************************************************
 *
 * $Id: StVpdCounter.hh,v 1.1 1999/01/15 20:40:29 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVpdCounter.hh,v $
 * Revision 1.1  1999/01/15 20:40:29  wenaus
 * Commit Thomas' original code
 *
 **************************************************************************/
#ifndef StVpdCounter_hh
#define StVpdCounter_hh

class StVpdCounter {
public:
    StVpdCounter();
    StVpdCounter(short id, float a, float t);
    ~StVpdCounter();
    // StVpdCounter(const StVpdCounter &right);
    // const StVpdCounter & operator=(const StVpdCounter &right);

    short id() const;
    float adc() const;
    float time() const;

    void setId(short);
    void setAdc(float);
    void setTime(float);
    
protected:
    short mId;
    float mAdc;
    float mTime;
};

inline short StVpdCounter::id() const { return mId; }

inline float StVpdCounter::adc() const { return mAdc; }

inline float StVpdCounter::time() const { return mTime; }

#endif
