#ifndef SMALLCHANGE_AXISKIT_H
#define SMALLCHANGE_AXISKIT_H

/**************************************************************************\
 *
 *  This file is part of the SmallChange extension library for Coin.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using SmallChange with software that can not be combined with the
 *  GNU GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFVec2f.h>
#include "SmallChange.h"
// #include <SmallChange/basic.h>


class SMALLCHANGE_DLL_API SmAxisKit : public SoBaseKit {
  
  typedef SoBaseKit inherited;
  SO_KIT_HEADER(SmAxisKit);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);

public:
  SmAxisKit(void);
  static void initClass(void);
  virtual SbBool affectsState(void) const;

  SoSFVec2f axisRange;
  SoSFFloat markerInterval;
  SoSFFloat markerHeight;
  SoSFFloat textInterval;
  SoSFInt32 digits;
  SoSFString axisName;

protected:
  virtual ~SmAxisKit();

private:
  friend class SmAxisKitP;
  class SmAxisKitP * pimpl;
};

#endif // !SMALLCHANGE_AXISKIT_H
