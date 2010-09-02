#include "LKH.h"

/* 
 * The Ascent function computes a lower bound on the optimal tour length 
 * using subgradient optimization. The function also transforms the original 
 * problem into a problem in which the Alpha-values reflect the likelihood 
 * of edges being optimal.
 *
 * The function attempts to find penalties (Pi-values) that maximizes the 
 * lower bound L(T(Pi)) - 2*PiSum, where L(T(Pi)) denotes the length of the 
 * minimum spanning 1-tree computed from the transformed distances, and PiSum 
 * denotes the sum of Pi-values. If C(i,j) denotes the length of an edge 
 * (i,j), then the transformed distance D(i,j) of an edge is 
 * C(i,j) + Pi(i) + Pi(j).
 *
 * The Minimum1TreeCost function is used to compute the cost of a minimum 
 * 1-tree.The Generatecandidates function is called in order to generate 
 * candidate sets. Minimum 1-trees are then computed in the corresponding 
 * sparse graph.         
 */

GainType Ascent()
{
    Node *t;
    GainType BestW, W, W0, Alpha, MaxAlpha = INT_MAX;
    int T, Period, P, InitialPhase;

  Start:
    /* Initialize Pi and BestPi */
    t = FirstNode;
    do
        t->Pi = t->BestPi = 0;
    while ((t = t->Suc) != FirstNode);
    if (CandidateSetType == DELAUNAY)
        CreateDelaunayCandidateSet();
    else
        AddTourCandidates();

    /* Compute the cost of a minimum 1-tree */
    W = Minimum1TreeCost(CandidateSetType == DELAUNAY
                         || MaxCandidates == 0);

    /* Return this cost 
       if either
       (1) subgradient optimization is not wanted,
       (2) the norm of the tree (its deviation from a tour) is zero
           (in that case the true optimum has been found), or
       (3) the cost equals the specified value for optimum.
     */
    if (!Subgradient || !Norm || W / Precision == Optimum)
        return W;

    if (Optimum != MINUS_INFINITY && (Alpha = Optimum * Precision - W) > 0)
        MaxAlpha = Alpha;
    if (MaxCandidates > 0) {
        /* Generate symmetric candididate sets for all nodes */
        if (CandidateSetType != DELAUNAY)
            GenerateCandidates(AscentCandidates, MaxAlpha, 1);
        else {
            OrderCandidateSet(AscentCandidates, MaxAlpha, 1);
            W = Minimum1TreeCost(1);
            if (!Norm || W / Precision == Optimum)
                return W;
        }
    }
    if (ExtraCandidates > 0)
        AddExtraCandidates(ExtraCandidates, ExtraCandidateSetType,
                           ExtraCandidateSetSymmetric);
    if (TraceLevel >= 2) {
        CandidateReport();
        printff("Subgradient optimization ...\n");
    }

    /* Set LastV of every node to V (the node's degree in the 1-tree) */
    t = FirstNode;
    do
        t->LastV = t->V;
    while ((t = t->Suc) != FirstNode);

    BestW = W0 = W;
    InitialPhase = 1;
    /* Perform subradient optimization with decreasing period length 
       and decreasing step size */
    for (Period = InitialPeriod, T = InitialStepSize * Precision;
         Period > 0 && T > 0 && Norm != 0; Period /= 2, T /= 2) {
        /* Period and step size are halved at each iteration */
        if (TraceLevel >= 2)
            printff
                ("  T = %d, Period = %d, BestW = %0.1f, Norm = %d\n",
                 T, Period, (double) BestW / Precision, Norm);
        for (P = 1; T && P <= Period && Norm != 0; P++) {
            /* Adjust the Pi-values */
            t = FirstNode;
            do {
                if (t->V != 0)
                    t->Pi += T * (7 * t->V + 3 * t->LastV) / 10;
                t->LastV = t->V;
            }
            while ((t = t->Suc) != FirstNode);
            /* Compute a minimum 1-tree in the sparse graph */
            W = Minimum1TreeCost(1);
            /* Test if an improvement has been found */
            if (W > BestW) {
                /* If the lower bound becomes greater than twice its
                   initial value it is taken as a sign that the graph is
                   too sparse */
                if (W > 2 * W0 && AscentCandidates > 0
                    && AscentCandidates < Dimension) {
                    W = Minimum1TreeCost(CandidateSetType == DELAUNAY
                                         || MaxCandidates == 0);
                    if (W < W0) {
                        /* Double the number of candidate edges 
                           and start all over again */
                        if (TraceLevel >= 2)
                            printff("Warning: AscentCandidates doubled\n");
                        if ((AscentCandidates *= 2) > Dimension)
                            AscentCandidates = Dimension;
                        goto Start;
                    }
                    W0 = W;
                }
                BestW = W;
                /* Update the BestPi-values */
                t = FirstNode;
                do
                    t->BestPi = t->Pi;
                while ((t = t->Suc) != FirstNode);
                if (TraceLevel >= 2)
                    printff
                        ("* T = %d, Period = %d, P = %d, BestW = %0.1f, Norm = %d\n",
                         T, Period, P, (double) BestW / Precision, Norm);
                /* If in the initial phase, the step size is doubled */
                if (InitialPhase && T * sqrt((double) Norm) > 0)
                    T *= 2;
                /* If the improvement was found at the last iteration of the 
                   current period, then double the period */
                if (CandidateSetType != DELAUNAY && P == Period
                    && (Period *= 2) > InitialPeriod)
                    Period = InitialPeriod;
            } else {
                if (TraceLevel >= 3)
                    printff
                        ("  T = %d, Period = %d, P = %d, W = %0.1f, Norm = %d\n",
                         T, Period, P, (double) W / Precision, Norm);
                if (InitialPhase && P > Period / 2) {
                    /* Conclude the initial phase */
                    InitialPhase = 0;
                    P = 0;
                    T = 3 * T / 4;
                }
            }
        }
    }

    t = FirstNode;
    do
        t->Pi = t->BestPi;
    while ((t = t->Suc) != FirstNode);

    /* Compute a minimum 1-tree */
    W = BestW = Minimum1TreeCost(CandidateSetType == DELAUNAY
                                 || MaxCandidates == 0);

    if (MaxCandidates > 0) {
        FreeCandidateSets();
        if (CandidateSetType == DELAUNAY)
            CreateDelaunayCandidateSet();
    } else {
        Candidate *Nt;
        t = FirstNode;
        do {
            for (Nt = t->CandidateSet; Nt && Nt->To; Nt++)
                Nt->Cost += t->Pi + Nt->To->Pi;
        }
        while ((t = t->Suc) != FirstNode);
    }
    if (TraceLevel >= 2)
        printff("Ascent: BestW = %0.1f, Norm = %d\n",
                (double) BestW / Precision, Norm);
    return W;
}
