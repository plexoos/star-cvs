/***************************************************************************
 *
 * $Id $
 *
 * Author: Aleksei Pavlinov, May 2000
 ***************************************************************************
 *
 * Description: Monte Carlo Emc Hit Collection class
 *
 ***************************************************************************
 *
 * $Log: StMcEmcHitCollection.cc,v $
 * Revision 2.3  2001/05/13 21:12:10  calderon
 * Modifications by Aleksei to the Emc Hit Collections on indexing of
 * module numbers
 *
 * Revision 2.2  2000/08/30 14:52:02  calderon
 * New changes made by Aleksei.
 *
 * Revision 2.1  2000/06/06 23:01:09  calderon
 * Inital revision
 *
 *
 **************************************************************************/
#include "StMcEmcHitCollection.hh"
#include "StMcEmcModuleHitCollection.hh"
#include "StMcCalorimeterHit.hh"
#include "TObjectSet.h"

static const char rcsid[] = "$Id ";

#ifdef PERSISTENT
ClassImp(StMcEmcHitCollection)
#endif

StMcEmcHitCollection::StMcEmcHitCollection():TDataSet("emcHits") {/* Nothing */}

StMcEmcHitCollection::StMcEmcHitCollection(char* name):TDataSet(name) {/* Nothing */}
StMcEmcHitCollection::StMcEmcHitCollection(const char* name):TDataSet(name) {/* Nothing */}

StMcEmcHitCollection::~StMcEmcHitCollection() 
{ /* noop */ }
    
StMcEmcHitCollection::EAddHit 
StMcEmcHitCollection::addHit(StMcCalorimeterHit* hit)
{
  unsigned int m ,i; // i - array index, m - module nuumber;
    if (hit && (m = hit->module())<=mNumberOfModules && m>=1) {
      i = m - 1;
      if(mModules[i].numberOfHits() == 0) {

        mModules[i](m); // Set name
        Add((TDataSet*)(&mModules[i]));

        mModules[i].hits().push_back(hit); // New hit(first)
        return kNew;
      }
      else {
        for(unsigned int ih=0; ih<mModules[i].numberOfHits(); ih++){
          if((*mModules[i].hits()[ih]) == (*hit)) { // Hits from the same particle
            (*mModules[i].hits()[ih])  += (*hit);
            return kAdd;
          }
        }
        mModules[i].hits().push_back(hit); // New hit
        return kNew;
      }
    }
    else return kErr; // Bad number of module
}

unsigned int
StMcEmcHitCollection::numberOfModules() const { return mNumberOfModules; }

unsigned long
StMcEmcHitCollection::numberOfHits() const
{
    unsigned long sum = 0;
    for (int i=0; i<mNumberOfModules; i++) {
	sum += mModules[i].hits().size();
    }
    return sum;
}

float 
StMcEmcHitCollection::sum() const
{
    float s = 0.0;
    unsigned int i, m;
    for(i=0; i < mNumberOfModules; i++){
      m  = i + 1;
      s += module(m)->sum();
    }
    return s;
}

StMcEmcModuleHitCollection*
StMcEmcHitCollection::module(unsigned int m)
{
// m - module number; i-index number;  i = m - 1; // 10-may-2001 
    unsigned int i = m - 1;
    if (i>=0 && i < mNumberOfModules)
        return &(mModules[i]);
    else
        return 0;
}

const StMcEmcModuleHitCollection*
StMcEmcHitCollection::module(unsigned int m) const
{
    unsigned int i = m - 1;
    if (i>=0 && i < mNumberOfModules)
        return &(mModules[i]);
    else
        return 0;
}

void
StMcEmcHitCollection::Browse(TBrowser *b)
{
  TDataSet::Browse(b);
  print();
}

void
StMcEmcHitCollection::print()
{
  //  cout<<"\n  "<<numberOfHits()<<" hits for "<<GetName()<<" sum Energy "<<sum()<<"\n";
  cout<<"-----------\n"<<GetName()<<" has "<<numberOfHits()<<" hits in "
      <<GetListSize()<<" modules. Deposit Energy "<<sum()<<" GeV \n-----------\n";
  unsigned int i, m;
  for (i=0; i<mNumberOfModules; i++) {
    m = i + 1;
    StSPtrVecMcCalorimeterHit& hits=mModules[i].hits();
    int nh = hits.size();
    if (nh > 0) {
      cout<<" " <<module(m)->GetName()<<" #hits "<<nh<<" Dep.Energy " << 
      module(m)->sum()<<"\n";
    }
  }
}
