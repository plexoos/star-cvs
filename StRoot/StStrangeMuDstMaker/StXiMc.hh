/***********************************************************************
 *
 * $Id: StXiMc.hh,v 3.3 2001/08/23 13:21:00 genevb Exp $
 * $Log: StXiMc.hh,v $
 * Revision 3.3  2001/08/23 13:21:00  genevb
 * Many bug workarounds...
 *
 * Revision 3.2  2001/05/04 20:15:15  genevb
 * Common interfaces and reorganization of components, add MC event info
 *
 * Revision 3.1  2000/07/14 21:28:34  genevb
 * Added V0Mc index for XiMc, fixed bug with entries for XiMc, cleaned up controllers
 *
 * Revision 3.0  2000/07/14 12:56:50  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/05 05:19:47  genevb
 * New version of Strangeness micro DST package
 *
 *
 ***********************************************************************
 *
 * Description: Monte Carlo Xi micro dst class
 *
 ***********************************************************************/
#ifndef  STAR_StXiMc
#define  STAR_StXiMc
#include "StKinkMc.hh"
#include "StXiI.hh"
//#include "StStrangeMuDst.hh"   // Now inherited through StKinkMc
#include "StDecayMode.hh"


class StXiMc : public virtual StXiI, public StKinkMc {
public:
  StXiMc();
  StXiMc(StMcVertex*, StMcTrack*, StStrangeEvMuDst*);
  ~StXiMc();

  Int_t V0Index();
  void SetV0Index(Int_t index);

  Int_t   charge() const;              // Particle charge
  Float_t decayVertexXiX() const;      // Coordinate of decay vertex
  Float_t decayVertexXiY() const;
  Float_t decayVertexXiZ() const;
  Float_t momBachelorX() const;        // Momentum components of bachelor
  Float_t momBachelorY() const;
  Float_t momBachelorZ() const;
  Float_t momXiX() const;              // Momentum components of Xi/Omega
  Float_t momXiY() const;
  Float_t momXiZ() const;

  Float_t decayVertexV0X() const {return 999.;} // Coordinates of decay vertex
  Float_t decayVertexV0Y() const {return 999.;}
  Float_t decayVertexV0Z() const {return 999.;}
  Float_t momPosX() const {return 999;} // Momentum components of pos. daughter
  Float_t momPosY() const {return 999;}
  Float_t momPosZ() const {return 999;}
  Float_t momNegX() const {return 999;} // Momentum components of neg. daughter
  Float_t momNegY() const {return 999;}
  Float_t momNegZ() const {return 999;}
  Float_t momV0X()  const {return 999;} // Momentum components of V0
  Float_t momV0Y()  const {return 999;}
  Float_t momV0Z()  const {return 999;}

  Int_t decayMode() const;
  Int_t geantIdParent() const;

protected:
  Int_t v0;
  ClassDef(StXiMc,4)
};

inline Int_t   StXiMc::charge() const {return parentCharge();}
inline Float_t StXiMc::decayVertexXiX() const {return positionX();}
inline Float_t StXiMc::decayVertexXiY() const {return positionY();}
inline Float_t StXiMc::decayVertexXiZ() const {return positionZ();}
inline Float_t StXiMc::momBachelorX() const {return daughterMomentumX();}
inline Float_t StXiMc::momBachelorY() const {return daughterMomentumY();}
inline Float_t StXiMc::momBachelorZ() const {return daughterMomentumZ();}
inline Float_t StXiMc::momXiX() const {return parentMomentumX();}
inline Float_t StXiMc::momXiY() const {return parentMomentumY();}
inline Float_t StXiMc::momXiZ() const {return parentMomentumZ();}
inline Int_t   StXiMc::V0Index() {return v0;}
inline void    StXiMc::SetV0Index(Int_t index) {v0=index;}
inline Int_t   StXiMc::geantIdParent() const {return StKinkMc::geantIdParent();}
inline Int_t   StXiMc::decayMode() const {return StKinkMc::decayMode();}
#endif
