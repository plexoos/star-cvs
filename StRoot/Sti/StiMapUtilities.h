//TpcMapUtilities.h
//M.L. Miller, Yale Software, 4/13

#ifndef TpcMapUtilities_h
#define TpcMapUtilities_h

#include <string>
using std::string;

#include "StiFactoryTypes.h" //for typedef.  Can't forward declare!

class StHit;
class StiHit;
class StTpcHit;
class StiIOBroker;

//Structure for hit map key
struct HitMapKey {
    bool operator==(const HitMapKey&) const;
    double refangle;
    double position;
};

//Functor for ordering hit map key
struct MapKeyLessThan{
    MapKeyLessThan() : reftolerance(.01), postolerance(.01) {};
    bool operator() (const HitMapKey&, const HitMapKey&) const;
    double reftolerance;
    double postolerance;
};

// Structure for material, shape, or detector name map key
struct NameMapKey {
    NameMapKey::NameMapKey(const string& str){ name = str; }
    NameMapKey::NameMapKey(){}
    bool operator==(const NameMapKey&) const;
    bool operator<(const NameMapKey&) const;
    string name;
};

//Functors for ordering hits
struct StHitRadiusLessThan
{
    bool operator() (const StHit*, const StHit*) const;
};

struct StHitRadiusGreaterThan
{
    bool operator() (const StHit*, const StHit*) const;
};

struct StidHitLessThan
{
    bool operator() (const StiHit*, const StiHit*) const;
};

struct StizHitLessThan
{
    bool operator() (const StiHit*, const StiHit*) const;
};

struct StiDetectorNodePositionLessThan
{
    bool operator() (const StiDetectorNode*, const StiDetectorNode*) const;
};

struct SameStHit
{
    bool operator() (const StiHit*) const;
    StHit* stHit;
};

//unary predicate
class StTpcHitFilter
{
public:
    virtual ~StTpcHitFilter() {};
    virtual bool operator()(const StTpcHit&) const = 0;
    virtual void getNewState()=0;
};

class StTpcPadrowHitFilter : public StTpcHitFilter
{
public:
    StTpcPadrowHitFilter();
    virtual ~StTpcPadrowHitFilter() {};

    virtual bool operator()(const StTpcHit&) const;
    virtual void getNewState();
    
private:
    unsigned int mMinPadrow;
    unsigned int mMaxPadrow;
};

#endif


