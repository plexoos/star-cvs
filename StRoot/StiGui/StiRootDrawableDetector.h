//StiRootDrawableDetector.h
//M.L. Miller (Yale Software)
//04/01

#ifndef StiRootDrawableDetector_HH
#define StiRootDrawableDetector_HH

#include "Sti/StiDetector.h"
#include "Sti/StiObjectFactory.h"

#include "StiRootDrawable.h"

class StiRootDrawableDetector : public StiDetector , public StiRootDrawable
{
public:

    StiRootDrawableDetector();
    virtual ~StiRootDrawableDetector();
    
protected:

    //Implement StiRootDrawable interface
    virtual void draw();
    virtual void update();
    virtual const char* name() const;

    //Overide StiDetector methods
    virtual void build();
    
protected:
    virtual void makeShape();
};

typedef StiObjectFactory<StiRootDrawableDetector> detector_factory;

#endif
