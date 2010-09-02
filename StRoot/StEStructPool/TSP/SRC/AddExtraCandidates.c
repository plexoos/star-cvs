#include "LKH.h"

/*
 * The AddExtraCandidates function extends the candidate set with the
 * K extra neighbors from a given canddate set type (QUADRANT, NN, 
 * or REINELT).
 *
 * A non-zero value of of the parameter Symmetric specifies that the 
 * candidate set is to be complemented such that every candidate edge 
 * is associated with both its 
 * two end nodes.
 *   
 * The function is called from CreateCandidateSet.  
 */

void AddExtraCandidates(int K, int CandidateSetType, int Symmetric)
{
    Candidate *Nt, *ExtraCandidateSet, **SavedCandidateSet;
    Node *t;

    assert(SavedCandidateSet =
           (Candidate **) malloc((1 + DimensionSaved) *
                                 sizeof(Candidate *)));
    t = FirstNode;
    do {
        SavedCandidateSet[t->Id] = t->CandidateSet;
        t->CandidateSet = 0;
    } while ((t = t->Suc) != FirstNode);
    if (CandidateSetType == NN)
        CreateNearestNeighborCandidateSet(K);
    else if (CandidateSetType == QUADRANT)
        CreateQuadrantCandidateSet(K);
    t = FirstNode;
    do {
        ExtraCandidateSet = t->CandidateSet;
        t->CandidateSet = SavedCandidateSet[t->Id];
        for (Nt = ExtraCandidateSet; Nt && Nt->To; Nt++) {
            AddCandidate(t, Nt->To, Nt->Cost, Nt->Alpha);
            if (Symmetric)
                AddCandidate(Nt->To, t, Nt->Cost, Nt->Alpha);
        }
        free(ExtraCandidateSet);
    } while ((t = t->Suc) != FirstNode);
    free(SavedCandidateSet);
}
