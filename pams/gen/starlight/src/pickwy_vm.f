c     This subroutine picks a w and y value for the vector-meson
c     calculation.
      subroutine pickwy_vm(W,Y)

      IMPLICIT NONE

      include 'D2LParam.inc'
      include 'global.inc'
      include 'Ftable.inc'
      include 'inputp.inc'
      include 'const.inc'
      real W,Y,xw,xy,xtest,ran,dW,dY      
      integer ISEED,IW,IY

      dW = (Wmax-Wmin)/DFLOAT(numw)
      dY = (Ymax-Ymin)/DFLOAT(numy)

C       >> DRAW xw,xy
 201    xw = ran(ISEED)
        W = Wmin + xw*(Wmax-Wmin)

C  protect against sub-threshold events

        IF (W .lt. 2*mpi) goto 201

        IW = INT((W-Wmin)/dW) + 1
        xy = ran(ISEED)
        Y = Ymin + xy*(Ymax-Ymin)
        IY = INT((Y-Ymin)/dY) + 1

C       >> Check if this matches the map
        xtest = ran(ISEED)
        IF(xtest.gt.farray(IW,IY))GOTO 201

C       >> W,Y should now have the right distribution

 	return
        END

