#ifndef MATH_CONSTANTS
#define MATH_CONSTANTS
#if 0
/* $Id: math_constants.h,v 1.1 2000/08/08 23:40:25 laue Exp $ 
 * $Log: math_constants.h,v $
 * Revision 1.1  2000/08/08 23:40:25  laue
 * New check-in. Needed only for standalone version
 *
 * Revision 1.4  2000/01/07 20:15:31  fisyak
 * fix linux warnigns
 *
 * Revision 1.3  1999/12/23 23:21:22  fisyak
 * remove egcs warning
 *
 * Revision 1.2  1998/11/30 16:06:10  fisyak
 * Protection versus usage in FORTRAN
 *
 * Revision 1.1  1998/02/19 15:56:21  fisyak
 * Math. Phys. constants from H.Ward
 *   
 * 
 * Most of these were copied from math.h.  The AIX math.h was
 * different than the other three, so I am simply going to have
 * our own version.
 *
 * Underscores here often mean 'divided by'.  E.g., PI_2 means pi/2.
*/
#endif
#define C_DEG_PER_RAD                 57.295779513082320876798155
#define C_RAD_PER_DEG                 0.017453292519943295769237
#define C_2PI                         6.28318530717958648
#define C_EULER                       0.577215664901532860606512
#define C_1_PI                        0.31830988618379067154
#define C_2_PI                        0.63661977236758134308
#define C_2_SQRTPI                    1.12837916709551257390
#define C_E                           2.7182818284590452354
#define C_LN10                        2.30258509299404568402
#define C_LN2                         0.69314718055994530942
#define C_LOG10E                      0.43429448190325182765
#define C_LOG2E                       1.4426950408889634074
#define C_PI                          3.14159265358979323846
#define C_PI_2                        1.57079632679489661923
#define C_PI_4                        0.78539816339744830962
#define C_SQRT1_2                     0.70710678118654752440
#define C_SQRT2                       1.41421356237309504880

#endif
