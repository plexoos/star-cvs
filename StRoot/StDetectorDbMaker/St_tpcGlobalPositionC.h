#ifndef St_tpcGlobalPositionC_h
#define St_tpcGlobalPositionC_h

#include "TChair.h"
#include "tables/St_tpcGlobalPosition_Table.h"

class St_tpcGlobalPositionC : public TChair {
 public:
  static St_tpcGlobalPositionC* 	instance();
  tpcGlobalPosition_st 	*Struct(Int_t i = 0) 	{return ((St_tpcGlobalPosition*) Table())->GetTable()+i;}
  UInt_t     	getNumRows()                	{return GetNRows();}
  Float_t 	LocalxShift(Int_t i = 0) 	{return Struct(i)->LocalxShift;}
  Float_t 	LocalyShift(Int_t i = 0) 	{return Struct(i)->LocalyShift;}
  Float_t 	LocalzShift(Int_t i = 0) 	{return Struct(i)->LocalzShift;}
  Float_t 	PhiXY(Int_t i = 0) 	 	{return Struct(i)->PhiXY;}
  Float_t 	PhiXZ(Int_t i = 0) 	 	{return Struct(i)->PhiXZ;}
  Float_t 	PhiYZ(Int_t i = 0) 	 	{return Struct(i)->PhiYZ;}
  Float_t 	XX(Int_t i = 0) 	 	{return Struct(i)->XX;}
  Float_t 	YY(Int_t i = 0) 	 	{return Struct(i)->YY;}
  Float_t 	ZZ(Int_t i = 0) 	 	{return Struct(i)->ZZ;}
  Float_t 	PhiXY_geom(Int_t i = 0) 	{return Struct(i)->PhiXY_geom;}
  Float_t 	PhiXZ_geom(Int_t i = 0) 	{return Struct(i)->PhiXZ_geom;}
  Float_t 	PhiYZ_geom(Int_t i = 0) 	{return Struct(i)->PhiYZ_geom;}
  Float_t 	XX_geom(Int_t i = 0) 	 	{return Struct(i)->XX_geom;}
  Float_t 	YY_geom(Int_t i = 0) 	 	{return Struct(i)->YY_geom;}
  Float_t 	ZZ_geom(Int_t i = 0) 	 	{return Struct(i)->ZZ_geom;}
 protected:
  St_tpcGlobalPositionC(St_tpcGlobalPosition *table=0) : TChair(table) {}
  virtual ~St_tpcGlobalPositionC() {SafeDelete(fgInstance);}
 private:
  static St_tpcGlobalPositionC* fgInstance;
  ClassDefChair(St_tpcGlobalPosition, tpcGlobalPosition_st )
  ClassDef(St_tpcGlobalPositionC,1) //C++ TChair for tpcGlobalPosition table class
};
#endif
