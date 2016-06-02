//
// Created by jan on 02.06.16.
//

#ifndef SHEET_4_GRAPHCUTOPTIMIZER_H
#define SHEET_4_GRAPHCUTOPTIMIZER_H

#include <functional>
#include "EdgeGraph.h"
#include "graph.h"

/**
 * This class can optimize binary energy functions based on graph cuts. The energy function must have the form
 * sum_i=1^n E_i (y_i) + sum_i,j;i<j E_ij(y_i,y_j), where E_ij must be regular.
 */
class BinaryGraphCutOptimizer
{
public:
    using Energy = double;
    using Site = unsigned int;
    using Label = bool;

    using Unary = std::function<Energy(Site s, Label l)>;
    using Pairwise = std::function<Energy(Site s1, Site s2, Label l1, Label l2)>;

    /**
     * Constructs a new optimizer
     * @param graph Graph to optimize on
     * @param unary Unary energy function
     * @param pairwise Pairwise energy function
     */
    BinaryGraphCutOptimizer(EdgeGraph const& graph, Unary const& unary, Pairwise const& pairwise);

    /**
     * Copy constructor
     * @param other Object to copy
     */
    BinaryGraphCutOptimizer(BinaryGraphCutOptimizer const& other);

    /**
     * Assignment operator
     * @param other Object to copy
     */
    void operator=(BinaryGraphCutOptimizer const& other);

    /**
     * Destructor
     */
    ~BinaryGraphCutOptimizer();

    /**
     * Optimizes the given graph
     */
    void optimize() const;

    /**
     * Retrieves which segment a site belongs to
     * @param s Site to check
     * @return True in case \p s belongs to the source set, false if it belongs to the sink.
     */
    bool whatSegment(Site s) const;

private:
    Unary m_unary;
    Pairwise m_pairwise;
    EdgeGraph m_graph;
    Graph<Energy, Energy, Energy>* m_pMaxFlow = nullptr;

    void setupInternalGraph();
};


#endif //SHEET_4_GRAPHCUTOPTIMIZER_H
