// @(#) $Id: AliHLTTPCCANeighboursCleaner.cxx,v 1.8 2010/08/27 20:01:33 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************


#include "AliHLTTPCCANeighboursCleaner.h"
#include "AliHLTTPCCAMath.h"
#include "AliHLTTPCCATracker.h"

#include <valgrind/memcheck.h>

// *
// * kill link to the neighbour if the neighbour is not pointed to the hit
// *

void AliHLTTPCCANeighboursCleaner::run( const int numberOfRows, SliceData &data, const AliHLTTPCCAParam &param )
{
  sfloat_v X,Y,Z,Xup,Yup,Zup,Xdown,Ydown,Zdown, X4,Y4,Z4;
  sfloat_v Yx1, Yx2, Yxx1, Yxx2, Yxxx, Zx1, Zx2, Zxx1, Zxx2, Zxxx;

  const int rowStep = AliHLTTPCCAParameters::RowStep;
  const int beginRowIndex = rowStep;
  const int endRowIndex = numberOfRows - rowStep;
  for ( int rowIndex = beginRowIndex; rowIndex < endRowIndex; ++rowIndex ) {
    const AliHLTTPCCARow &row = data.Row( rowIndex );
    const AliHLTTPCCARow &rowUp = data.Row( rowIndex + rowStep );
    const AliHLTTPCCARow &rowDown = data.Row( rowIndex - rowStep );
    const int numberOfHits = row.NHits();

    // - look at up link, if it's valid but the down link in the row above doesn't link to us remove
    //   the link
    // - look at down link, if it's valid but the up link in the row below doesn't link to us remove
    //   the link
    for ( int hitIndex = 0; hitIndex < numberOfHits; hitIndex += short_v::Size ) {

      const ushort_v hitIndexes = ushort_v( Vc::IndexesFromZero ) + hitIndex;
      short_m validHitsMask = hitIndexes < numberOfHits;
      assert( ( validHitsMask && ((hitIndexes   >= 0 ) && (hitIndexes   < row.NHits()   )) ) == validHitsMask );
      validHitsMask &= ( short_v(data.HitDataIsUsed( row ), hitIndexes, validHitsMask ) == short_v( Vc::Zero ) ); // not-used hits can be connected only with not-used, so only one check is needed

        // collect information
        // up part
      assert( ( validHitsMask && ((hitIndexes   >= 0 ) && (hitIndexes   < row.NHits()   )) ) == validHitsMask );
      short_v up = short_v(data.HitLinkUpData( row ), hitIndexes, validHitsMask );
      VALGRIND_CHECK_VALUE_IS_DEFINED( up );
      ushort_v upIndexes = up.staticCast<ushort_v>();
      // assert ( (validHitsMask && (up >= short_v( -1 )) ) == validHitsMask ); // TODO uncomment - now have a problem with optimized(NODEBUG) mode with gcc451
      short_m upMask = validHitsMask && up >= short_v( Vc::Zero );
      assert( ( upMask && ((upIndexes   >= 0 ) && (upIndexes   < rowUp.NHits()   )) ) == upMask );
      short_v downFromUp = short_v(data.HitLinkDownData( rowUp ), upIndexes, upMask );      
        // down part
      short_v dn = short_v(data.HitLinkDownData( row ), hitIndexes, validHitsMask );
      // assert ( ( validHitsMask && (dn >= short_v(-1)) ) == validHitsMask ); // TODO uncomment - now have a problem with optimized(NODEBUG) mode with gcc451
      VALGRIND_CHECK_VALUE_IS_DEFINED( dn );
      ushort_v downIndexes = dn.staticCast<ushort_v>();
      short_m dnMask = validHitsMask && dn >= short_v( Vc::Zero );
      assert( ( dnMask && ((downIndexes   >= 0 ) && (downIndexes   < rowDown.NHits()   )) ) == dnMask );
      short_v upFromDown = short_v(data.HitLinkUpData( rowDown ), downIndexes, dnMask );
      
        // -- make clean --
           // check if some one-way links can be good

//       std::cout << "downFromUp " << downFromUp   << " upFromDown " << upFromDown << " validHitsMask " << validHitsMask << " dnMask " << dnMask << " upMask " << upMask << std::endl; // IKu debug
      upMask &= (downFromUp == -1) && (upFromDown == static_cast<short_v>(hitIndexes));  // have mutual link only downwards. is   up-link good?
      dnMask &= (upFromDown == -1) && (downFromUp == static_cast<short_v>(hitIndexes)); // have mutual link only upwards.   is down-link good?
      
      dnMask &= short_m(rowIndex + 2*rowStep < numberOfRows);  // will use up & upup hits for check. If no one there then have to delete link
      upMask &= short_m(rowIndex - 2*rowStep >= 0);
      upMask &= dn >= short_v( Vc::Zero );
      dnMask &= up >= short_v( Vc::Zero );
      
      short_m upOrDnMask = upMask || dnMask;
///mvz start 14.07.2010
      if(!upOrDnMask.isEmpty())
      {
        X = data.RowX( rowIndex );
        assert( ( upOrDnMask && ((hitIndexes   >= 0 ) && (hitIndexes   < row.NHits()   )) ) == upOrDnMask );
        Y.gather( data.HitDataY( row ), hitIndexes, upOrDnMask );//HitDataY( row, hitIndex );
        Z.gather( data.HitDataZ( row ), hitIndexes, upOrDnMask );// data.HitDataZ( row, hitIndex );

        if(!dnMask.isEmpty())
        {
          Xdown = data.RowX( rowIndex - rowStep );
          assert( ( dnMask && ((downIndexes   >= 0 ) && (downIndexes   < rowDown.NHits()   )) ) == dnMask );
          Ydown.gather( data.HitDataY( rowDown ), downIndexes, dnMask );
          Zdown.gather( data.HitDataZ( rowDown ), downIndexes, dnMask );
          
          const AliHLTTPCCARow &rowUpUp = data.Row( rowIndex + 2*rowStep );
          assert( ( dnMask && ((upIndexes   >= 0 ) && (upIndexes   < rowUp.NHits()   )) ) == dnMask );
          short_v upup = short_v(data.HitLinkUpData( rowUp ), upIndexes, dnMask );           

          dnMask &= upup >= short_v( Vc::Zero ); // can't check, so can't save any other link
//          short_v downFromUpUp = short_v(data.HitLinkDownData( rowUpUp ), static_cast<ushort_v>(upupIndexes), dnMask ); 
//          dnMask &= downFromUpUp != -1; 

          X4 = data.RowX( rowIndex + 2*rowStep );
          assert( ( dnMask && ((upup   >= 0 ) && (upup   < rowUpUp.NHits()   )) ) == dnMask );
          ushort_v upupIndexes = upup.staticCast<ushort_v>();
          Y4.gather( data.HitDataY( rowUpUp ), upupIndexes, dnMask );
          Z4.gather( data.HitDataZ( rowUpUp ), upupIndexes, dnMask );

          Yx1 = (Y - Y4)/(X - X4);
          Yx2 = Y - X*Yx1;
          Yxx2 = Yx2 + Yx1*Xdown;
          Zx1 = (Z - Z4)/(X - X4);
          Zx2 = Z - X*Zx1;
          Zxx2 = Zx2 + Zx1*Xdown;

          sfloat_v err2Y, err2Z;
          param.GetClusterErrors2(upIndexes,Xdown,Ydown,Zdown,err2Y, err2Z);

          sfloat_v ch = CAMath::Abs((Yxx2 - Ydown)/CAMath::Sqrt(err2Y))+CAMath::Abs((Zxx2 - Zdown)/CAMath::Sqrt(err2Z));
 //         upMask &= static_cast<short_m>(ch < 50.f);
          dnMask &= static_cast<short_m>(CAMath::Abs((Yxx2 - Ydown)/CAMath::Sqrt(err2Y)) < 50.f);
//          dnMask &= static_cast<short_m>(CAMath::Abs((Zxx2 - Zdown)/CAMath::Sqrt(err2Z)) < 80.f);
        }
        if(!upMask.isEmpty())
        {
          Xup = data.RowX( rowIndex + rowStep );
          assert( ( upMask && ((upIndexes   >= 0 ) && (upIndexes   < rowUp.NHits()   )) ) == upMask );
          Yup.gather( data.HitDataY( rowUp ), upIndexes, upMask );
          Zup.gather( data.HitDataZ( rowUp ), upIndexes, upMask );
          
          const AliHLTTPCCARow &rowDownDown = data.Row( rowIndex - 2*rowStep );
          assert( ( upMask && ((downIndexes   >= 0 ) && (downIndexes   < rowDown.NHits()   )) ) == upMask );
          short_v downdown = short_v(data.HitLinkDownData( rowDown ), downIndexes, upMask );
          upMask &= downdown >= short_v( Vc::Zero ); // can't check, so can't save any other link
//          short_v upFromDownDown = short_v(data.HitLinkUpData( rowDownDown ), static_cast<ushort_v>(downdownIndexes), upMask ); 
//          upMask &= upFromDownDown != -1;
          
          ASSERT( ( upMask && (downdown   >= 0 ) && (downdown   < rowDownDown.NHits()) ) == upMask,
                   " upMask= " << upMask << " dn= " << dn <<  " downdown= "  << downdown << " row= " << rowIndex << " rowDownDown.NHits= " << rowDownDown.NHits());
          X4 = data.RowX( rowIndex - 2*rowStep );
          ushort_v downdownIndexes = downdown.staticCast<ushort_v>();
          Y4.gather( data.HitDataY( rowDownDown ), downdownIndexes, upMask );
          Z4.gather( data.HitDataZ( rowDownDown ), downdownIndexes, upMask );

          Yx1 = (Y - Y4)/(X - X4);
          Yx2 = Y - X*Yx1;
          Yxx2 = Yx2 + Yx1*Xup;
          Zx1 = (Z - Z4)/(X - X4);
          Zx2 = Z - X*Zx1;
          Zxx2 = Zx2 + Zx1*Xup;

          sfloat_v err2Y, err2Z;
          param.GetClusterErrors2(upIndexes,Xup,Yup,Zup,err2Y, err2Z);

          //std::cout << upMask << "     "<<CAMath::Abs((Yxx2 - Yup)/CAMath::Sqrt(err2Y))<< "   " << (CAMath::Abs((Yxx2 - Yup)/CAMath::Sqrt(err2Y))+CAMath::Abs((Zxx2 - Zup)/CAMath::Sqrt(err2Z)))<<std::endl;
          sfloat_v ch = CAMath::Abs((Yxx2 - Yup)/CAMath::Sqrt(err2Y));
          upMask &= static_cast<short_m>(ch < 50.f);
//          upMask &= static_cast<short_m>(CAMath::Abs((Yxx2 - Yup)/CAMath::Sqrt(err2Y)) < 50.f);
//          upMask &= static_cast<short_m>(CAMath::Abs((Zxx2 - Zup)/CAMath::Sqrt(err2Z)) < 80.f);
//          std::cout << upMask << "     "<<std::endl;

          Zx1 = (Zdown - Z4)/(Xdown - X4);
          Zx2 = (Z - Zdown)/(X - Xdown);
          Zxx2 = (Zx1 - Zx2)*2/(X - X4);
        }
      } // if(!dnMask.isEmpty() || !upMask.isEmpty()) 

///mvz end 14.07.2010

      assert( ( upMask && ((upIndexes   >= 0 ) && (upIndexes   < rowUp.NHits()   )) ) == upMask );
      assert( ( dnMask && ((downIndexes >= 0 ) && (downIndexes < rowDown.NHits() )) ) == dnMask );      
        // make from good one-way links mutual links
      downFromUp( upMask ) = static_cast<short_v>(hitIndexes);
      upFromDown( dnMask ) = static_cast<short_v>(hitIndexes);
      data.SetHitLinkDownData( rowUp, upIndexes, downFromUp, upMask );
      data.SetHitLinkUpData( rowDown, downIndexes, upFromDown, dnMask );
     //  std::cout << "validHitsMask= " << validHitsMask << " hitIndexes=" << hitIndexes << std::endl;
     //  std::cout << "up >= short_v( Vc::Zero )= " << short_m(up >= short_v( Vc::Zero ))  << "dn >= short_v( Vc::Zero )= " << short_m(dn >= short_v( Vc::Zero ))  <<  std::endl;
     // std::cout << " upIndexes " <<  upIndexes << " downIndexes " <<  downIndexes << std::endl;
     // std::cout << "downFromUp " << downFromUp   << " upFromDown " << upFromDown << std::endl; // IKu debug
      // short_v downFromUp1 =  downFromUp; // dbg
      // short_v upFromDown1 =  upFromDown;
      

      downFromUp = short_v(data.HitLinkDownData( rowUp ), upIndexes,   validHitsMask && (up >= short_v( Vc::Zero )));   // TODO rid of it - now have a problem with optimized(NODEBUG) mode with gcc451
      upFromDown = short_v(data.HitLinkUpData( rowDown ), downIndexes, validHitsMask && (dn >= short_v( Vc::Zero )));   // TODO rid of it
            
      // downFromUp = short_v(data.HitLinkDownData( rowUp ), upIndexes, upMask );      
      // upFromDown = short_v(data.HitLinkUpData( rowDown ), downIndexes, dnMask );
     // std::cout << " upIndexes " <<  upIndexes << " downIndexes " <<  downIndexes << std::endl;
     // std::cout << "downFromUp " << downFromUp   << " upFromDown " << upFromDown << std::endl; // IKu debug
     // assert( ( ( ((downFromUp1 == downFromUp) && (downFromUp != hitIndexes)) || (downFromUp == hitIndexes) ) && validHitsMask) == validHitsMask );
     // assert( ( ( ((upFromDown1 == upFromDown) && (upFromDown != hitIndexes)) || (upFromDown == hitIndexes) ) && validHitsMask) == validHitsMask );

//       std::cout << " downFromUp " << downFromUp
//           << " upFromDown " << upFromDown
//          << " dnMask " << dnMask << " upMask " << upMask  << std::endl; // IKu debug

        // delete one-way links (all other one-way links)
      const short_v minusOne = -1;
      short_m badUpMask = validHitsMask && (downFromUp != static_cast<short_v>(hitIndexes));
      VALGRIND_CHECK_VALUE_IS_DEFINED( up );
      assert( ( badUpMask && ((hitIndexes   >= 0 ) && (hitIndexes   < row.NHits()   )) )  == badUpMask );
      data.SetHitLinkUpData( row, hitIndexes, minusOne, badUpMask );

      short_m badDnMask = validHitsMask && (upFromDown != static_cast<short_v>(hitIndexes));
      VALGRIND_CHECK_VALUE_IS_DEFINED( dn );
      assert( ( badDnMask && ((hitIndexes   >= 0 ) && (hitIndexes   < row.NHits()   )) )  == badDnMask );
      data.SetHitLinkDownData( row, hitIndexes, minusOne, badDnMask );
    } // for iHit
  } // for iRow
}
