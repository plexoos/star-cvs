/***************************************************************************
 *
 * $Id: StMuArrays.h,v 1.13 2004/11/29 15:53:22 mvl Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 ***************************************************************************/
/** 
    @class StMuArrays 
    Class holding the definitions of the TClonesArrays as static data members.
    There are two sets of TClonesArrays. The 'arrays' for tracks and event information,
    as well as the 'strangeArrays' holding the information that has been copied from the 
    StStrangeMuDst (i.e. V0s, Kinks, etc.)
*/
#ifndef StMuArrays_hh
#define StMuArrays_hh

/// @enum emcTypes enumeration to to index the emcArrays
//enum emcTypes {muEmc=0};
enum emcTypes {muEmcTow=0, muEmcPrs, muEmcSmde, muEmcSmdp, muEEmcPrs, muEEmcSmdu, muEEmcSmdv};

/// @enum strangeTypes enumeration to to index the strangeArrays
enum strangeTypes {smuEv=0, smuEvMc, smuV0, smuV0Mc, smuV0Assoc, smuXi, smuXiMc, smuXiAssoc, smuKink, smuKinkMc, smuKinkAssoc, smuCut};

/// @enum enumeration to to index the arrays
enum muDstTypes {muEvent=0, muPrimary, muGlobal, muOther, muL3, muRich, muState, muAccept, muReject}; 

/// @enum pmdTypes enumeration to to index the pmdArrays
enum pmdTypes {muPmdHit=0, muCpvHit, muPmdCluster, muCpvCluster}; 

/// @enum Tofr enumeration
enum tofTypes {muTofHit=0, muTofData};

/// @enum eztTypes enumeration to to index the eztArrays (IUCF-ezTree)
enum eztTypes {muEztHead=0, muEztTrig, muEztETow, muEztESmd,muEztFpd};

enum NARRAYS {
__NARRAYS__        =9,	///< size of the 'regular stuff' arrays, i.e. number of TClonesArrays  
__NSTRANGEARRAYS__ =12,	///< size of the strangeness arrays, i.e. number of TClonesArrays  
__NEMCARRAYS__     =7 ,	///< size of the emc arrays, i.e. number of TClonesArrays  
__NPMDARRAYS__     =4 ,	///< size of the pmd arrays, i.e. number of TClonesArrays  
__NTOFARRAYS__     =2 ,  ///< size of the tof arrays >
__NEZTARRAYS__     =5 ,  ///< size of the ez arrays >
__NALLARRAYS__     =  __NARRAYS__+__NSTRANGEARRAYS__+__NEMCARRAYS__+__NPMDARRAYS__+__NTOFARRAYS__+__NEZTARRAYS__
};
class StMuArrays {
 public:
 StMuArrays();
///< names of the TBranches in the TTree/File 
    static const char*         arrayNames    [__NALLARRAYS__    ];
    static const char** strangeArrayNames; //[__NSTRANGEARRAYS__]
    static const char**      emcArrayNames;//[__NEMCARRAYS__    ]
    static const char**      pmdArrayNames;//[__NPMDARRAYS__    ]
    static const char**      tofArrayNames;//[__NTOFARRAYS__    ]
    static const char**      eztArrayNames;//[__NEZARRAYS__    ]

///< names of the classes, the TClonesArrays are arrays of this type
    static const char*   arrayTypes          [__NALLARRAYS__    ];
    static const char**  strangeArrayTypes;//[__NSTRANGEARRAYS__]
    static const char**  emcArrayTypes;//    [__NEMCARRAYS__    ]
    static const char**  pmdArrayTypes;//    [__NPMDARRAYS__    ]
    static const char**  tofArrayTypes;//    [__NTOFARRAYS__    ]
    static const char**  eztArrayTypes;//    [__NEZARRAYS__    ]

///< maximum sizes of the TClonesArrays
    static int           arraySizes    [__NALLARRAYS__    ];
    static int*   strangeArraySizes;// [__NSTRANGEARRAYS__]
    static int*       emcArraySizes;// [__NEMCARRAYS__    ]
    static int*       pmdArraySizes;// [__NPMDARRAYS__    ]
    static int*       tofArraySizes;// [__NTOFARRAYS__    ]
    static int*       eztArraySizes;// [__NEZARRAYS__    ]

///< number of entries in current event, currently not used
    static int        arrayCounters    [__NALLARRAYS__    ];
    static int*strangeArrayCounters;// [__NSTRANGEARRAYS__]
    static int*    emcArrayCounters;// [__NEMCARRAYS__    ]
    static int*    pmdArrayCounters;// [__NPMDARRAYS__    ]
    static int*    tofArrayCounters;// [__NTOFARRAYS__    ]
    static int*    eztArrayCounters;// [__NEZARRAYS__    ]
};

#endif

/***************************************************************************
 *
 * $Log: StMuArrays.h,v $
 * Revision 1.13  2004/11/29 15:53:22  mvl
 * Additions by Jan for Fpd ezTree
 *
 * Revision 1.12  2004/10/28 00:11:33  mvl
 * Added stuff to support ezTree mode of MuDstMaker.
 * This is a special mode for fast-online processing of fast-detector data.
 *
 * Revision 1.11  2004/10/19 01:43:05  mvl
 * Changes for splitting Emc and Pmd collections
 *
 * Revision 1.10  2004/07/27 02:35:23  mvl
 * Added access methods for Strangeness Monte-Carlo arrays
 *
 * Revision 1.9  2004/05/04 00:10:28  perev
 * Cleanup
 *
 * Revision 1.8  2004/04/26 00:13:28  perev
 * Cleanup+simplification
 *
 * Revision 1.7  2004/04/09 22:06:35  subhasis
 * after tof createevent fix by Xin
 *
 * Revision 1.6  2004/04/09 03:36:14  jeromel
 * Removed TOF support entirely for now as we need a working version ... Will
 * revisit later.
 *
 * Revision 1.5  2004/04/02 03:24:53  jeromel
 * Changes implements PMD and TOF.  TOF is clearly incomplete.
 *
 * Revision 1.4  2003/01/09 18:59:45  laue
 * initial check in of new EMC classes and the changes required
 *
 * Revision 1.3  2002/05/20 17:23:31  laue
 * StStrangeCuts added
 *
 * Revision 1.2  2002/05/04 23:56:29  laue
 * some documentation added
 *
 * Revision 1.1  2002/03/08 17:04:17  laue
 * initial revision
 *
 *
 **************************************************************************/
