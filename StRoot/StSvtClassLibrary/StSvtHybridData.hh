/***************************************************************************
 *
 * $Id: StSvtHybridData.hh,v 1.3 2001/04/30 22:20:42 caines Exp $
 *
 * Author: Marcelo Munhoz
 ***************************************************************************
 *
 * Description: SVT Hybrid Data BASE class
 *
 ***************************************************************************
 *
 * $Log: StSvtHybridData.hh,v $
 * Revision 1.3  2001/04/30 22:20:42  caines
 * Add Anode to setList fn so works with ZSP data
 *
 * Revision 1.2  2000/11/30 20:39:12  caines
 * Changed to allow us of database
 *
 * Revision 1.1.1.1  2000/03/10 14:26:21  munhoz
 * SVT Class Library
 *
 **************************************************************************/

#ifndef STSVTHYBRIDDATA_HH
#define STSVTHYBRIDDATA_HH

#include "StSvtHybridObject.hh"

class StSequence;

class StSvtHybridData: public StSvtHybridObject
{
public:
  StSvtHybridData();
  StSvtHybridData(int barrel, int ladder, int wafer, int hybrid);
  virtual ~StSvtHybridData();

  StSvtHybridData(const StSvtHybridData&);
  StSvtHybridData& operator = (const StSvtHybridData&);

  int getAnodeList(int*& list); // Returns the number of anodes that have at least one sequence. The list of anodes that have sequences is passed through reference to the argument of this method
  int getSequences(int anodeID, int& nSequences, StSequence*& sequence); // Pass through reference the sequences of a particular anode
  int getListSequences(int listID, int& nSequence,StSequence*& sequence); // Pass through reference the sequences of a particular list item of the anode list (used mainly in the cluster finder) 

  int setListSequences(int listID, int Anode, int& nSequence, StSequence* tempSeq); // resets the sequence list
  int setAnodeList(); // Allows to reset the anodelist

protected:

  int nAnodes;       // Number of anodes with one or more sequences
  int* anodeList;    //! List of anodes with one or more sequences
  int* nSeq;         //! Number of sequences for each anode
  StSequence** seq;  //! List of sequences for each anode

  ClassDef(StSvtHybridData,1)
};

#endif
