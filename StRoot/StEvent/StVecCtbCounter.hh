/***************************************************************************
 *
 * $Id: StVecCtbCounter.hh,v 1.3 1999/03/04 15:57:06 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVecCtbCounter.hh,v $
 * Revision 1.3  1999/03/04 15:57:06  wenaus
 * add std namespace for Sun CC5 compatibility
 *
 * Revision 1.4  1999/03/04 18:17:31  ullrich
 * Namespace std not used if ST_NO_NAMESPACES defined
 *
 * Revision 1.3  1999/03/04 15:57:06  wenaus
 * add std namespace for Sun CC5 compatibility
 *
 * Revision 1.2  1999/01/15 22:54:16  wenaus
 * version with constructors for table-based loading
 *
#define StVecCtbCounter_hh
#include <vector>
using namespace std;
#endif

class StCtbCounter;
#ifdef ST_NO_TEMPLATE_DEF_ARGS
typedef vector<StCtbCounter, allocator<StCtbCounter> > StVecCtbCounter;
#else
typedef vector<StCtbCounter> StVecCtbCounter;
#endif

#endif
