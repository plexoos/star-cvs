#ifndef TPC_WIREPLANES_H
#define TPC_WIREPLANES_H
//:Description: location & dimension of Wire-planes
//:Synonyms::::
//:Source:
//:Update:
//:Update frequncy:
//:Reminder:
//:Recall frequency:
//:Size of Data:
//:Pointer to data:  Geometry_tpc/tpcWirePlanes
struct tpcWirePlanes {

  //  type varnam;    //Units : Comments
    double anodeWireRadius;
    double frischGridWireRadius;
    double gatingGridWireRadius;
    
    double anodeWirePitch;
    double frischGridWirePitch;
    double gatingGridWirePitch;
    
    double innerSectorAnodeWirePadPlaneSeparation;
    double innerSectorFrischGridPadPlaneSeparation;
    double innerSectorGatingGridPadPlaneSeparation;

    double outerSectorAnodeWirePadPlaneSeparation;
    double outerSectorFrischGridPadPlaneSeparation;
    double outerSectorGatingGridPadPlaneSeparation;

    int    numberOfInnerSectorAnodeWires;
    int    numberOfInnerSectorFrischGridWires;
    int    numberOfInnerSectorGatingGridWires;
    double firstInnerSectorAnodeWire;
    double firstInnerSectorFrischGridWire;
    double firstInnerSectorGatingGridWire;
    double lastInnerSectorAnodeWire;

    int    numberOfOuterSectorAnodeWires;
    int    numberOfOuterSectorFrischGridWires;
    int    numberOfOuterSectorGatingGridWires;
    double firstOuterSectorAnodeWire;
    double firstOuterSectorFrischGridWire;
    double firstOuterSectorGatingGridWire;
    double lastOuterSectorAnodeWire;


};
#endif
