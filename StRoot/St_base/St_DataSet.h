//*CMZ :          13/08/98  18.27.27  by  Valery Fine(fine@bnl.gov)
//*-- Author :    Valery Fine(fine@mail.cern.ch)   13/08/98 

#ifndef ROOT_St_DataSet
#define ROOT_St_DataSet
 
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_DataSet                                                           //
//                                                                      //
// St_DataSet class is a base class to implement the directory-like     //
// data structures and maintain it via St_DataSetIter class iterator    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
//*KEEP,TList.
#include "TList.h"
//*KEEP,TNamed.
#include "TNamed.h"
#include "TNode.h"
//*KEND.
 
class St_DataSetIter;
class TBrowser;

//----- dataset flags
enum ESetBits {
     kMark        = BIT(22)   // if object is marked
};

enum EBitOpt { 
               kSet   = kTRUE,
               kReset = kFALSE
             };

// The control codes to navigate the St_DataSet structure via St_DataSet::Pass method

typedef enum {
      kContinue,  // continue passing 
      kPrune,     // stop passing of the current branch but continue with the next one if any
      kStop,      // break passing
      kUp,        // break passing, return to the previous level, then continue
      kStruct,    // work with structural links only
      kAll,       // work with all links 
      kRefs,      // work with refs links only
      kMarked,    // work with marked links only
      kLock = BIT(21) // DataSet is locked
     } EDataSetPass;

class St_DataSet : public TNamed
{
 friend class St_DataSetIter;
 friend class St_DataSetTree;
 protected: 
    St_DataSet  *fParent; // pointer to mother of the directory
    TList       *fList;   // List of the the the objects included into this dataset
    virtual void SetMother(TObject *mother) {SetParent((St_DataSet*)mother);}
    St_DataSet(const Char_t *name,const Char_t *title):
    TNamed(name,title),fParent(0),fList(0){} // to support TDictionary
    static EDataSetPass SortIt(St_DataSet *ds);

    static EDataSetPass SortIt(St_DataSet *ds,void *user);
     
 public:
 
    St_DataSet(const Char_t *name="", St_DataSet *parent=0);
    St_DataSet(const St_DataSet &src,EDataSetPass iopt=kAll);
    St_DataSet(TNode &src); 
    virtual ~St_DataSet();
    virtual void         Add(St_DataSet *dataset);
    virtual void         AddFirst(St_DataSet *dataset);
    virtual void         AddLast(St_DataSet *dataset);
    virtual Int_t        Audit(Option_t *opt=0){if(opt){/*touch*/};return 0;};
    virtual void         Browse(TBrowser *b);
    virtual TObject     *Clone();
    virtual void         Delete(Option_t *opt="");   
    virtual St_DataSet  *Find(const Char_t *path) const;
    virtual St_DataSet  *FindObject(const Char_t *name,const Char_t *path="",Option_t *opt="") const;
    virtual St_DataSet  *First() const;
            TList       *GetList()   const { return fList; }
    virtual Int_t        GetListSize() const;
            TObject     *GetMother() const { return (TObject*)GetParent();}
    virtual TObject     *GetObject(){printf("***DUMMY GetObject***\n");return 0;}
    virtual St_DataSet  *GetParent() const { return fParent;}
    virtual Long_t       HasData() const {return 0;} 	// Check whether this dataset has extra "data-members"
    virtual TString      Path() const;                  // return the "full" path of this dataset
    virtual EDataSetPass Pass(EDataSetPass ( *callback)(St_DataSet *),Int_t depth=0);
    virtual EDataSetPass Pass(EDataSetPass ( *callback)(St_DataSet *,void*),void *user,Int_t depth=0);
    virtual Int_t        Purge(Option_t *opt="");   
    virtual void         Remove(St_DataSet *set);
    virtual void         SetLock(Int_t lock);
    virtual void         SetMother(St_DataSet *parent=0){SetParent(parent);};
    virtual void         SetObject(TObject *obj){printf("***DUMMY PutObject***%p\n",obj);}
    virtual void         SetParent(St_DataSet *parent=0);
    virtual void         SetWrite();
    virtual void         Shunt(St_DataSet *dataset=0);
    virtual void         Sort();			//Sort objects in lexical order
    virtual Bool_t       IsFolder() {return kTRUE;}
    virtual Bool_t       IsLocked() const ;
    virtual Bool_t       IsThisDir(const Char_t *dirname,int len=-1,int ignorecase=0) const ;
    virtual St_DataSet  *Last() const;
    virtual void         ls(Option_t *option="")  const;      // Option "*" means print all levels
    virtual void         ls(Int_t depth)  const;              // Print the "depth" levels of this datatset
    virtual void         Mark();                              // *MENU*
    virtual void         Mark(UInt_t flag,EBitOpt reset=kSet);
    virtual void         Update();                            // Update dataset
    virtual void         Update(St_DataSet *set,UInt_t opt=0);// Update this dataset with the new one
    ClassDef(St_DataSet,1)
};
 
inline void      St_DataSet::Add(St_DataSet *dataset){ AddLast(dataset); }
inline Int_t     St_DataSet::GetListSize() const {TList *tl=GetList(); return (tl) ? tl->GetSize():0;}

#endif
