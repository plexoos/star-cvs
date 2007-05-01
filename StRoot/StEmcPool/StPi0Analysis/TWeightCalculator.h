#ifndef StPi0Analysis_TWeightCalculator_H
#define StPi0Analysis_TWeightCalculator_H

#include <TNamed.h>
#include <TF1.h>
#include <TH1.h>

#include <list>
using namespace std;

#include "StPi0AnalysisVersion.h"
#include "TBinStatistics.h"
#include "StPi0AnalysisUtil.h"

class TWeightCalculator : public TNamed {
	public:
		typedef TWeightCalculator this_type;
		typedef TNamed inherited;
		typedef TBinStatistics bin_type;
		typedef list<bin_type> bin_list_type;

		TWeightCalculator(const Char_t *name = 0, const Char_t *title = 0);
		TWeightCalculator(const this_type &w);
		virtual ~TWeightCalculator();

		this_type &operator=(const this_type &w);
		Bool_t operator==(const this_type &w) const;
		Bool_t operator!=(const this_type &w) const;

		Int_t debug;

		virtual void Print(Option_t* option) const;

		Float_t mult;
		Float_t multDrift;
		Float_t pt0;
		Float_t power;

		Bool_t useHistogram;
		
		TH1F *histogram;

		Float_t rangeLow;
		Float_t rangeHigh;

		Float_t getWeight(Float_t value) const;

		TF1 *createFunc(Bool_t forDrawing) const;

		void Fit(const bin_list_type &points);
		void Fit(const TH1 *hist);
		void DrawCopy(Option_t* option = "") const;

		ClassDef(TWeightCalculator, STPI0ANALYSIS_VERSION);
};

#endif
