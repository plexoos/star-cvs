c	JS	calculates d2F/dWdY as per Eq. 6 in STAR Note 243.
C--- This portion of the program has been modified from a program
c  written by K Hencken.   Unfortunately, MeV are the units of energy
c  here --------------

C Calculate the Luminosity-function d^2L/dMdY where M is the invariant
C mass and Y the rapidity C of the photon-photon-system
C
C Calculations are based on the formulae of
C C. Cahn, J. D. Jackson, Nucl. Phys.
C Kai Hencken, 29. 9. 1994
C
C
       REAL FUNCTION  D2LDMDY(M,Y)
       IMPLICIT NONE
       REAL M,Y

C M : invariant mass (MeV)
C Y : rapidity

C D2LDMDY : diff. Lum. (MeV**-1)

C the Collider-Parameter are set using this Common-Block
C all Parameter are IN Parameter

	include 'const.inc'
	include 'D2LParam.inc'
	double precision RNuc, gamma_em,hbarc,pi,mp,mpi,mK,alpha
	double precision hbarcmev

C Z : charge of the heavy ions
C A : IF .GT. 0 : calculate the radii of the ions
C                 as R = 1.2 fm * A**(1/3)
C R : radii of the ions (in fm) (if provided, set A to 0)
C Gamma  : Lorentz factor of each ion in the center of velocity frame
C          (= c.m. frame for identical ions)



C internally used variables (for integration)

       double precision RM,W1,W2
       COMMON/D2LIParam/RM,W1,W2

       REAL NClass,DeltaL
       EXTERNAL NClass,DeltaL

       REAL LClass,DL

C physical constant hbar*c (convert to MeV)
	hbarcmev = hbarc*1000.


C calculate photon frequencies from M,Y

       W1 = M/2.0 * EXP (Y)
       W2 = M/2.0 * EXP (-Y)

C calculate Rho in MeV^-1 instead of fm
C use A or R, if A<= 0

       RM = RNuc / hbarcmev
C calculate the "classical" value first.

       LClass = NClass (W1,RM,gamma_em) * NClass (W2,RM,gamma_em)


C subtract from this the Delta-value

       DL = DeltaL ()

       D2LDMDY = 2.0 / M * Z**4 * alpha**2 * (LClass - DL)

       RETURN
       END


C the classical photon-number without impact parameter cutoff:

       REAL FUNCTION NClass ( W, R, gamma)

       include 'const.inc'
       include 'D2LParam.inc'
       double precision W,R,gamma
       REAL Xi

       Xi = W*R/gamma
       NClass = 2.0/PI * (Xi*besK0 (Xi)*besk1 (Xi) -
     &          Xi**2/2.0 *(besK1(Xi)**2 - besk0(Xi)**2))

       RETURN
       END


C the difference to the classical value

       REAL FUNCTION DeltaL ()
       IMPLICIT NONE

       include 'const.inc'
       include 'D2LParam.inc'
       double precision RNuc, gamma_em, pi,hbarc,alpha,mp,mpi,mK
       double precision RM,W1,W2
       COMMON/D2LIParam/RM,W1,W2	


       DOUBLE PRECISION XGAUSS(126),WGAUSS(126)
       COMMON /GAUSSD/XGAUSS,WGAUSS

       INTEGER N,I
       REAL b1,t,tmin,tmax
       REAL Sum,Int,Int2

       REAL Itgb1
       EXTERNAL Itgb1

       REAL Accur
       PARAMETER (Accur = 1E -2)


C integrate first over b1



C Loop incrementing the boundary

       tmin = 0.0
       tmax = 0.25
       Sum = 0.0

100    CONTINUE


C Loop for the Gauss integration

       Int=0.0
       DO N=1,6
       Int2 = Int
       Int=0.0
       DO I=2**N-1,2**(N+1)-2
       t = (tmax-tmin)/2.0*XGAUSS(I)+(tmax+tmin)/2.0
       b1 = RM * EXP (t)
       Int=Int+WGAUSS(I) * Itgb1 (b1) * b1**2
       ENDDO
       Int = (tmax-tmin)/2.0*Int
       IF (ABS ((Int2-Int)/Int) .LT. Accur) GOTO 200
       ENDDO

C comment this line to suppress error messages

       WRITE(*,*) ' (b1) GAUSS MAY BE INACCURATE'

200       CONTINUE

       Sum = Sum + Int

       IF (ABS (Int2/Sum) .LT. Accur) GOTO 300

       tmin = tmax
       tmax = tmax + 0.5

       GOTO 100

300    CONTINUE

       DeltaL = 4.0*Pi * Sum
       RETURN
       END


C integration then over b2:

       REAL FUNCTION Itgb1(b1)

       IMPLICIT NONE

       include 'D2LParam.inc'
       REAL b1
       REAL bmin,bmax
       double precision RNuc, gamma_em
       INTEGER N,I
       REAL b2,Int,Int2
       REAL Itgb2
       EXTERNAL Itgb2

       double precision RM,W1,W2
       COMMON/D2LIParam/RM,W1,W2

       DOUBLE PRECISION XGAUSS(126),WGAUSS(126)
       COMMON /GAUSSD/XGAUSS,WGAUSS


       REAL Accur
       PARAMETER (Accur = 1E-2)

       bmin = b1 - 2.0*RM
       IF (RM .GT. bmin) THEN
       bmin = RM
       ENDIF
       bmax = b1 + 2.0 * RM

       Int = 0.0
       DO N=1,6
       Int2 = Int
       Int = 0.0
       DO I=2**N-1,2**(N+1)-2
       b2 = (bmax-bmin)/2.0*XGAUSS(I)+(bmax+bmin)/2.0
       Int = Int +WGAUSS(I) * Itgb2(b1,b2) * b2
       ENDDO
       Int = (bmax-bmin)/2.0*Int
       IF (ABS((Int2 - Int)/Int) .LT. Accur) GOTO 100
       ENDDO

C comment this line to suppress error messages

       WRITE(*,*) ' (b2) GAUSS MAY BE INACCURATE'

100    CONTINUE

       Itgb1 = Int
       RETURN
       END

C the function to be integrated over

       REAL FUNCTION Itgb2 (b1,b2)

       IMPLICIT NONE

       include 'D2LParam.inc'
       REAL b1,b2
       double precision RNuc, gamma_em
       double precision RM,W1,W2
       COMMON/D2LIParam/RM,W1,W2

       REAL Nphoton
       EXTERNAL Nphoton

       Itgb2 = Nphoton(W1,b1,gamma_em) *
     &         Nphoton(W2,b2,gamma_em) *
     &         ACOS (( b1**2 + b2**2 - 4.0 * RM**2)/(2.0*b1*b2))

       RETURN
       END

C the differential photonnumber for a nucleus
C (without form factor)

       REAL FUNCTION Nphoton (W,Rho,gamma)

       include 'const.inc'
       REAL Rho
       double precision W,gamma,pi,hbarc,alpha,mp,mpi,mK
       REAL Wphib,WGamma

          WGamma = W/gamma

C without form factor

       Wphib = WGamma*besk1 (WGamma*Rho)
       Nphoton = 1.0/PI**2 * Wphib**2
       RETURN
       END


C Data for the Gauss integration, x-values and weight for a
C N=2,4,8,16,32,64 point Gauss integration.

C based on program GAUSS64 of D. Trautmann
C data from Abramowitz & Stegun

C      KAI HENCKEN, FEBRUARY 1993

       BLOCKDATA GAUSSDATA
       IMPLICIT NONE
       DOUBLE PRECISION XGAUSS(126),WGAUSS(126)
       COMMON /GAUSSD/XGAUSS,WGAUSS

       DATA XGAUSS(1)/ .57735026918962576D0/
       DATA XGAUSS(2)/-.57735026918962576D0/
       DATA WGAUSS(1)/ 1.00000000000000000D0/
       DATA WGAUSS(2)/ 1.00000000000000000D0/

       DATA XGAUSS(3)/ .33998104358485627D0/
       DATA XGAUSS(4)/ .86113631159405258D0/
       DATA XGAUSS(5)/-.33998104358485627D0/
       DATA XGAUSS(6)/-.86113631159405258D0/
       DATA WGAUSS(3)/ .65214515486254613D0/
       DATA WGAUSS(4)/ .34785484513745385D0/
       DATA WGAUSS(5)/ .65214515486254613D0/
       DATA WGAUSS(6)/ .34785484513745385D0/

       DATA XGAUSS(7)/ .18343464249564981D0/
       DATA XGAUSS(8)/ .52553240991632899D0/
       DATA XGAUSS(9)/ .79666647741362674D0/
       DATA XGAUSS(10)/ .96028985649753623D0/
       DATA XGAUSS(11)/-.18343464249564981D0/
       DATA XGAUSS(12)/-.52553240991632899D0/
       DATA XGAUSS(13)/-.79666647741362674D0/
       DATA XGAUSS(14)/-.96028985649753623D0/
       DATA WGAUSS(7)/ .36268378337836198D0/
       DATA WGAUSS(8)/ .31370664587788727D0/
       DATA WGAUSS(9)/ .22238103445337448D0/
       DATA WGAUSS(10)/ .10122853629037627D0/
       DATA WGAUSS(11)/ .36268378337836198D0/
       DATA WGAUSS(12)/ .31370664587788727D0/
       DATA WGAUSS(13)/ .22238103445337448D0/
       DATA WGAUSS(14)/ .10122853629037627D0/

       DATA XGAUSS(15)/ .0950125098376374402D0/
       DATA XGAUSS(16)/ .281603550779258913D0/
       DATA XGAUSS(17)/ .458016777657227386D0/
       DATA XGAUSS(18)/ .617876244402643748D0/
       DATA XGAUSS(19)/ .755404408355003034D0/
       DATA XGAUSS(20)/ .865631202387831744D0/
       DATA XGAUSS(21)/ .944575023073232576D0/
       DATA XGAUSS(22)/ .989400934991649933D0/
       DATA XGAUSS(23)/-.0950125098376374402D0/
       DATA XGAUSS(24)/-.281603550779258913D0/
       DATA XGAUSS(25)/-.458016777657227386D0/
       DATA XGAUSS(26)/-.617876244402643748D0/
       DATA XGAUSS(27)/-.755404408355003034D0/
       DATA XGAUSS(28)/-.865631202387831744D0/
       DATA XGAUSS(29)/-.944575023073232576D0/
       DATA XGAUSS(30)/-.989400934991649933D0/
       DATA WGAUSS(15)/ .189450610455068496D0/
       DATA WGAUSS(16)/ .182603415044923589D0/
       DATA WGAUSS(17)/ .169156519395002538D0/
       DATA WGAUSS(18)/ .149595988816576732D0/
       DATA WGAUSS(19)/ .124628971255533872D0/
       DATA WGAUSS(20)/ .0951585116824927848D0/
       DATA WGAUSS(21)/ .0622535239386478929D0/
       DATA WGAUSS(22)/ .0271524594117540949D0/
       DATA WGAUSS(23)/ .189450610455068496D0/
       DATA WGAUSS(24)/ .182603415044923589D0/
       DATA WGAUSS(25)/ .169156519395002538D0/
       DATA WGAUSS(26)/ .149595988816576732D0/
       DATA WGAUSS(27)/ .124628971255533872D0/
       DATA WGAUSS(28)/ .0951585116824927848D0/
       DATA WGAUSS(29)/ .0622535239386478929D0/
       DATA WGAUSS(30)/ .0271524594117540949D0/

       DATA XGAUSS(31)/ .0483076656877383162D0/
       DATA XGAUSS(32)/ .144471961582796493D0/
       DATA XGAUSS(33)/ .239287362252137075D0/
       DATA XGAUSS(34)/ .331868602282127650D0/
       DATA XGAUSS(35)/ .421351276130635345D0/
       DATA XGAUSS(36)/ .506899908932229390D0/
       DATA XGAUSS(37)/ .587715757240762329D0/
       DATA XGAUSS(38)/ .663044266930215201D0/
       DATA XGAUSS(39)/ .732182118740289680D0/
       DATA XGAUSS(40)/ .794483795967942407D0/
       DATA XGAUSS(41)/ .849367613732569970D0/
       DATA XGAUSS(42)/ .896321155766052124D0/
       DATA XGAUSS(43)/ .934906075937739689D0/
       DATA XGAUSS(44)/ .964762255587506430D0/
       DATA XGAUSS(45)/ .985611511545268335D0/
       DATA XGAUSS(46)/ .997263861849481564D0/
       DATA XGAUSS(47)/-.0483076656877383162D0/
       DATA XGAUSS(48)/-.144471961582796493D0/
       DATA XGAUSS(49)/-.239287362252137075D0/
       DATA XGAUSS(50)/-.331868602282127650D0/
       DATA XGAUSS(51)/-.421351276130635345D0/
       DATA XGAUSS(52)/-.506899908932229390D0/
       DATA XGAUSS(53)/-.587715757240762329D0/
       DATA XGAUSS(54)/-.663044266930215201D0/
       DATA XGAUSS(55)/-.732182118740289680D0/
       DATA XGAUSS(56)/-.794483795967942407D0/
       DATA XGAUSS(57)/-.849367613732569970D0/
       DATA XGAUSS(58)/-.896321155766052124D0/
       DATA XGAUSS(59)/-.934906075937739689D0/
       DATA XGAUSS(60)/-.964762255587506430D0/
       DATA XGAUSS(61)/-.985611511545268335D0/
       DATA XGAUSS(62)/-.997263861849481564D0/
       DATA WGAUSS(31)/ .0965400885147278006D0/
       DATA WGAUSS(32)/ .0956387200792748594D0/
       DATA WGAUSS(33)/ .0938443990808045654D0/
       DATA WGAUSS(34)/ .0911738786957638847D0/
       DATA WGAUSS(35)/ .0876520930044038111D0/
       DATA WGAUSS(36)/ .0833119242269467552D0/
       DATA WGAUSS(37)/ .0781938957870703065D0/
       DATA WGAUSS(38)/ .0723457941088485062D0/
       DATA WGAUSS(39)/ .0658222227763618468D0/
       DATA WGAUSS(40)/ .0586840934785355471D0/
       DATA WGAUSS(41)/ .0509980592623761762D0/
       DATA WGAUSS(42)/ .0428358980222266807D0/
       DATA WGAUSS(43)/ .0342738629130214331D0/
       DATA WGAUSS(44)/ .0253920653092620595D0/
       DATA WGAUSS(45)/ .0162743947309056706D0/
       DATA WGAUSS(46)/ .00701861000947009660D0/
       DATA WGAUSS(47)/ .0965400885147278006D0/
       DATA WGAUSS(48)/ .0956387200792748594D0/
       DATA WGAUSS(49)/ .0938443990808045654D0/
       DATA WGAUSS(50)/ .0911738786957638847D0/
       DATA WGAUSS(51)/ .0876520930044038111D0/
       DATA WGAUSS(52)/ .0833119242269467552D0/
       DATA WGAUSS(53)/ .0781938957870703065D0/
       DATA WGAUSS(54)/ .0723457941088485062D0/
       DATA WGAUSS(55)/ .0658222227763618468D0/
       DATA WGAUSS(56)/ .0586840934785355471D0/
       DATA WGAUSS(57)/ .0509980592623761762D0/
       DATA WGAUSS(58)/ .0428358980222266807D0/
       DATA WGAUSS(59)/ .0342738629130214331D0/
       DATA WGAUSS(60)/ .0253920653092620595D0/
       DATA WGAUSS(61)/ .0162743947309056706D0/
       DATA WGAUSS(62)/ .00701861000947009660D0/

       DATA XGAUSS(63)/ .02435029266342443250D0/
       DATA XGAUSS(64)/ .0729931217877990394D0/
       DATA XGAUSS(65)/ .121462819296120554D0/
       DATA XGAUSS(66)/ .169644420423992818D0/
       DATA XGAUSS(67)/ .217423643740007084D0/
       DATA XGAUSS(68)/ .264687162208767416D0/
       DATA XGAUSS(69)/ .311322871990210956D0/
       DATA XGAUSS(70)/ .357220158337668116D0/
       DATA XGAUSS(71)/ .402270157963991604D0/
       DATA XGAUSS(72)/ .446366017253464088D0/
       DATA XGAUSS(73)/ .489403145707052957D0/
       DATA XGAUSS(74)/ .531279464019894546D0/
       DATA XGAUSS(75)/ .571895646202634034D0/
       DATA XGAUSS(76)/ .611155355172393250D0/
       DATA XGAUSS(77)/ .648965471254657340D0/
       DATA XGAUSS(78)/ .685236313054233243D0/
       DATA XGAUSS(79)/ .719881850171610827D0/
       DATA XGAUSS(80)/ .752819907260531897D0/
       DATA XGAUSS(81)/ .783972358943341408D0/
       DATA XGAUSS(82)/ .813265315122797560D0/
       DATA XGAUSS(83)/ .840629296252580363D0/
       DATA XGAUSS(84)/ .865999398154092820D0/
       DATA XGAUSS(85)/ .889315445995114106D0/
       DATA XGAUSS(86)/ .910522137078502806D0/
       DATA XGAUSS(87)/ .929569172131939576D0/
       DATA XGAUSS(88)/ .946411374858402816D0/
       DATA XGAUSS(89)/ .961008799652053719D0/
       DATA XGAUSS(90)/ .973326827789910964D0/
       DATA XGAUSS(91)/ .983336253884625957D0/
       DATA XGAUSS(92)/ .991013371476744321D0/
       DATA XGAUSS(93)/ .996340116771955279D0/
       DATA XGAUSS(94)/ .999305041735772139D0/
       DATA XGAUSS(95)/-.02435029266342443250D0/
       DATA XGAUSS(96)/-.0729931217877990394D0/
       DATA XGAUSS(97)/-.121462819296120554D0/
       DATA XGAUSS(98)/-.169644420423992818D0/
       DATA XGAUSS(99)/-.217423643740007084D0/
       DATA XGAUSS(100)/-.264687162208767416D0/
       DATA XGAUSS(101)/-.311322871990210956D0/
       DATA XGAUSS(102)/-.357220158337668116D0/
       DATA XGAUSS(103)/-.402270157963991604D0/
       DATA XGAUSS(104)/-.446366017253464088D0/
       DATA XGAUSS(105)/-.489403145707052957D0/
       DATA XGAUSS(106)/-.531279464019894546D0/
       DATA XGAUSS(107)/-.571895646202634034D0/
       DATA XGAUSS(108)/-.611155355172393250D0/
       DATA XGAUSS(109)/-.648965471254657340D0/
       DATA XGAUSS(110)/-.685236313054233243D0/
       DATA XGAUSS(111)/-.719881850171610827D0/
       DATA XGAUSS(112)/-.752819907260531897D0/
       DATA XGAUSS(113)/-.783972358943341408D0/
       DATA XGAUSS(114)/-.813265315122797560D0/
       DATA XGAUSS(115)/-.840629296252580363D0/
       DATA XGAUSS(116)/-.865999398154092820D0/
       DATA XGAUSS(117)/-.889315445995114106D0/
       DATA XGAUSS(118)/-.910522137078502806D0/
       DATA XGAUSS(119)/-.929569172131939576D0/
       DATA XGAUSS(120)/-.946411374858402816D0/
       DATA XGAUSS(121)/-.961008799652053719D0/
       DATA XGAUSS(122)/-.973326827789910964D0/
       DATA XGAUSS(123)/-.983336253884625957D0/
       DATA XGAUSS(124)/-.991013371476744321D0/
       DATA XGAUSS(125)/-.996340116771955279D0/
       DATA XGAUSS(126)/-.999305041735772139D0/
       DATA WGAUSS(63)/ .0486909570091397204D0/
       DATA WGAUSS(64)/ .0485754674415034269D0/
       DATA WGAUSS(65)/ .0483447622348029572D0/
       DATA WGAUSS(66)/ .0479993885964583077D0/
       DATA WGAUSS(67)/ .0475401657148303087D0/
       DATA WGAUSS(68)/ .0469681828162100173D0/
       DATA WGAUSS(69)/ .0462847965813144172D0/
       DATA WGAUSS(70)/ .0454916279274181445D0/
       DATA WGAUSS(71)/ .0445905581637565631D0/
       DATA WGAUSS(72)/ .0435837245293234534D0/
       DATA WGAUSS(73)/ .0424735151236535890D0/
       DATA WGAUSS(74)/ .0412625632426235286D0/
       DATA WGAUSS(75)/ .0399537411327203414D0/
       DATA WGAUSS(76)/ .0385501531786156291D0/
       DATA WGAUSS(77)/ .0370551285402400460D0/
       DATA WGAUSS(78)/ .0354722132568823838D0/
       DATA WGAUSS(79)/ .0338051618371416094D0/
       DATA WGAUSS(80)/ .0320579283548515535D0/
       DATA WGAUSS(81)/ .0302346570724024789D0/
       DATA WGAUSS(82)/ .0283396726142594832D0/
       DATA WGAUSS(83)/ .0263774697150546587D0/
       DATA WGAUSS(84)/ .0243527025687108733D0/
       DATA WGAUSS(85)/ .0222701738083832542D0/
       DATA WGAUSS(86)/ .0201348231535302094D0/
       DATA WGAUSS(87)/ .0179517157756973431D0/
       DATA WGAUSS(88)/ .0157260304760247193D0/
       DATA WGAUSS(89)/ .0134630478967186426D0/
       DATA WGAUSS(90)/ .0111681394601311288D0/
       DATA WGAUSS(91)/ .00884675982636394772D0/
       DATA WGAUSS(92)/ .00650445796897836286D0/
       DATA WGAUSS(93)/ .00414703326056246764D0/
       DATA WGAUSS(94)/ .00178328072169643295D0/
       DATA WGAUSS(95)/ .0486909570091397204D0/
       DATA WGAUSS(96)/ .0485754674415034269D0/
       DATA WGAUSS(97)/ .0483447622348029572D0/
       DATA WGAUSS(98)/ .0479993885964583077D0/
       DATA WGAUSS(99)/ .0475401657148303087D0/
       DATA WGAUSS(100)/ .0469681828162100173D0/
       DATA WGAUSS(101)/ .0462847965813144172D0/
       DATA WGAUSS(102)/ .0454916279274181445D0/
       DATA WGAUSS(103)/ .0445905581637565631D0/
       DATA WGAUSS(104)/ .0435837245293234534D0/
       DATA WGAUSS(105)/ .0424735151236535890D0/
       DATA WGAUSS(106)/ .0412625632426235286D0/
       DATA WGAUSS(107)/ .0399537411327203414D0/
       DATA WGAUSS(108)/ .0385501531786156291D0/
       DATA WGAUSS(109)/ .0370551285402400460D0/
       DATA WGAUSS(110)/ .0354722132568823838D0/
       DATA WGAUSS(111)/ .0338051618371416094D0/
       DATA WGAUSS(112)/ .0320579283548515535D0/
       DATA WGAUSS(113)/ .0302346570724024789D0/
       DATA WGAUSS(114)/ .0283396726142594832D0/
       DATA WGAUSS(115)/ .0263774697150546587D0/
       DATA WGAUSS(116)/ .0243527025687108733D0/
       DATA WGAUSS(117)/ .0222701738083832542D0/
       DATA WGAUSS(118)/ .0201348231535302094D0/
       DATA WGAUSS(119)/ .0179517157756973431D0/
       DATA WGAUSS(120)/ .0157260304760247193D0/
       DATA WGAUSS(121)/ .0134630478967186426D0/
       DATA WGAUSS(122)/ .0111681394601311288D0/
       DATA WGAUSS(123)/ .00884675982636394772D0/
       DATA WGAUSS(124)/ .00650445796897836286D0/
       DATA WGAUSS(125)/ .00414703326056246764D0/
       DATA WGAUSS(126)/ .00178328072169643295D0/

       END
     
c---- Here ends program modified from K. Henken ------------c
                                                          
