//StJetEtCell.h
//M.L. Miller (Yale Software) (adapted from Akio Ogawa's work)
//07/02

#ifndef StJetEtCell_HH
#define StJetEtCell_HH

#include <list>
using std::list;

#include "Stiostream.h"

#include "StProtoJet.h"

double deltaphi(double p1, double p2);

class StJetEtCell
{
public:
    typedef list<StProtoJet> JetList;
    typedef StProtoJet::FourVecList FourList;
    typedef list<StJetEtCell*> CellList;

    StJetEtCell(); 
    StJetEtCell(double etaMin, double etaMax, double phiMin, double phiMax);
    virtual ~StJetEtCell();

    //simple access
    double eta() const;
    double phi() const;
    virtual double eT() const;
    double etaMin() const;
    double etaMax() const;
    double phiMin() const;
    double phiMax() const;
    int nTimesUsed() const;

    //Allow jet-finder power to over-ride eT
    void setEt(double v) {mEt=v;}

    //operators (sort by Et)
    virtual bool operator>(const StJetEtCell& rhs) const {
	return eT() > rhs.eT();
    }
    virtual bool operator<(const StJetEtCell& rhs) const {
	return eT() < rhs.eT();
    }

    //equality via cell geometry
    bool operator==(const StJetEtCell* rhs) const {
	return ( (*this)==(*rhs) );
    };
    bool operator==(const StJetEtCell& rhs) const {
	return (mEtaMin==rhs.mEtaMin && mEtaMax==rhs.mEtaMax
		&& mPhiMin==rhs.mPhiMin && mPhiMax==rhs.mPhiMax);
    };

    //access to the centroid.  If mUpToDate==true, just return.  Else calculate, store, return
    const StFourVec& centroid();

    //action
    virtual void add(const StProtoJet&);
    virtual void add(StJetEtCell* cell);

    void setNtimesUsed(int); ///count how many jets ref this cell
    virtual void clear(); ///internal reset for next pass at jet-finding

    //distance measures (to be moved to polymorphic behavior)
    double deltaPhi(const StJetEtCell& rhs) const;
    double deltaEta(const StJetEtCell& rhs) const;
    double distance(const StJetEtCell& rhs) const;

    //write access to the contents of the cell
    StProtoJet& protoJet() {return mProtoJet;}

    //daugter cells of this one
    CellList& cellList();
    const CellList& cellList() const;

    //does this cell itself have any energy
    bool empty() const {return mProtoJet.size()==0;}

    //number of particles in protojet
    unsigned int size() const; 

protected:
    friend ostream& operator<<(ostream& os, const StJetEtCell& cell);
    friend struct PreJetUpdater;
    friend struct PreJetInitializer;
    friend struct PostMergeUpdater;

    double mEtaMin;
    double mEtaMax;
    double mPhiMin;
    double mPhiMax;
    double mEt;
    int mNtimesUsed;

    //lazy cache of centroid
    bool mUpToDate;
    StFourVec mCentroid;

    //JetList mProtoJets; //remember the particles that make up cell!
    CellList mCells;    //remember the cells cluster w/ this one
    StProtoJet mProtoJet;
};

//inlines
inline void StJetEtCell::clear()
{
    mEt=0.;
    mNtimesUsed=0;
    //mProtoJets.clear();
    mCells.clear();
    mProtoJet.clear();
    mUpToDate=false;
}

inline double StJetEtCell::eta() const
{
    return (mEtaMax+mEtaMin)/2.;
}

inline double StJetEtCell::phi() const
{
    return (mPhiMax+mPhiMin)/2.;
}

inline double StJetEtCell::eT() const
{
    return mEt;
}

//private access
inline double StJetEtCell::etaMin() const
{
    return mEtaMin;
}

inline double StJetEtCell::etaMax() const
{
    return mEtaMax;
}

inline double StJetEtCell::phiMin() const
{
    return mPhiMin;
}

inline double StJetEtCell::phiMax() const
{
    return mPhiMax;
}

inline int StJetEtCell::nTimesUsed() const
{
    return mNtimesUsed;
}

inline void StJetEtCell::setNtimesUsed(int v)
{
    mNtimesUsed=v;
}

inline const StJetEtCell::CellList& StJetEtCell::cellList() const
{
    return mCells;
}

inline StJetEtCell::CellList& StJetEtCell::cellList()
{
    return mCells;
}

inline unsigned int StJetEtCell::size() const
{
    return mProtoJet.size();
}

inline double StJetEtCell::deltaPhi(const StJetEtCell& rhs) const
{
    return deltaphi( phi(), rhs.phi() );
}

inline double StJetEtCell::deltaEta(const StJetEtCell& rhs) const
{
    return eta()-rhs.eta();
}

inline ostream& operator<<(ostream& os, const StJetEtCell& cell)
{
    os <<"eta: "<<cell.eta()<<"\tphi: "<<cell.phi()<<"\tet: "<<cell.eT()
	//<<"\tsize: "<<cell.size()<<"\tnUsed: "<<cell.nTimesUsed();
       <<"\tcells:"<<endl;
    const StJetEtCell::CellList& l = cell.cellList();
    for (StJetEtCell::CellList::const_iterator it=l.begin(); it!=l.end(); ++it) {
	cout <<**it;
    }

    return os;

}
#endif

