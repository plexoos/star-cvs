//TpcMapUtilities.h
//M.L. Miller, Yale Software, 4/13

#ifndef TpcMapUtilities_h
#define TpcMapUtilities_h

class StHit;
class StiHit;

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

//Structures for detector map key
struct DetectorMapKey {
  bool operator==(const DetectorMapKey&) const;
  bool operator<(const DetectorMapKey&) const;
  double position;
  double refangle;
  double z;
};

// Structure for material map key
struct MaterialMapKey {
    MaterialMapKey::MaterialMapKey(const char *str){ strncpy(name, str, 127); }
    MaterialMapKey::MaterialMapKey(){}
    bool operator==(const MaterialMapKey&) const;
    bool operator<(const MaterialMapKey&) const;
    char name[128];
};

struct ShapeMapKey {
    ShapeMapKey::ShapeMapKey(const char *str){ strncpy(name, str, 127); }
    ShapeMapKey::ShapeMapKey(){}
    bool operator==(const ShapeMapKey&) const;
    bool operator<(const ShapeMapKey&) const;
    char name[128];
};

//Functors for ordering hits
struct StHitRadiusLessThan
{
    bool operator() (const StHit*, const StHit*) const;
};

struct StiHitLessThan
{
    bool operator() (const StiHit*, const StiHit*) const;
};

struct StidHitLessThan
{
    bool operator() (const StiHit*, const StiHit*) const;
};

struct StizHitLessThan
{
    bool operator() (const StiHit*, const StiHit*) const;
};


#endif


