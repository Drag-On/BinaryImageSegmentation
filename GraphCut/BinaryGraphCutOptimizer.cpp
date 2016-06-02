//
// Created by jan on 02.06.16.
//

#include "BinaryGraphCutOptimizer.h"

BinaryGraphCutOptimizer::BinaryGraphCutOptimizer(EdgeGraph const& graph, BinaryGraphCutOptimizer::Unary const& unary,
                                                 BinaryGraphCutOptimizer::Pairwise const& pairwise)
        : m_graph(graph),
          m_unary(unary),
          m_pairwise(pairwise),
          m_pMaxFlow(new Graph<Energy, Energy, Energy>(graph.nVertices(), graph.nEdges()))
{
    setupInternalGraph();
}

BinaryGraphCutOptimizer::BinaryGraphCutOptimizer(BinaryGraphCutOptimizer const& other)
        : m_graph(other.m_graph),
          m_unary(other.m_unary),
          m_pairwise(other.m_pairwise),
          m_pMaxFlow(new Graph<Energy, Energy, Energy>(other.m_graph.nVertices(), other.m_graph.nEdges()))
{
    setupInternalGraph();
}

void BinaryGraphCutOptimizer::operator=(BinaryGraphCutOptimizer const& other)
{
    if (this != &other)
    {
        delete m_pMaxFlow;
        m_graph = other.m_graph;
        m_unary = other.m_unary;
        m_pairwise = other.m_pairwise;
        m_pMaxFlow = new Graph<Energy, Energy, Energy>(other.m_graph.nVertices(), other.m_graph.nEdges());
        setupInternalGraph();
    }
}

BinaryGraphCutOptimizer::~BinaryGraphCutOptimizer()
{
    delete m_pMaxFlow;
}

void BinaryGraphCutOptimizer::optimize() const
{
    m_pMaxFlow->maxflow();
}

bool BinaryGraphCutOptimizer::whatSegment(BinaryGraphCutOptimizer::Site s) const
{
    return m_pMaxFlow->what_segment(s) == Graph<Energy, Energy, Energy>::SOURCE;
}

void BinaryGraphCutOptimizer::setupInternalGraph()
{
    m_pMaxFlow->add_node(m_graph.nVertices());

    // Setup unary weights between each node i and the terminals as:
    // source -> i: E_i(1)
    // i -> sink: E_i(0)
    if (m_unary)
    {
        for (Site s = 0; s < m_graph.nVertices(); s++)
            m_pMaxFlow->add_tweights(s, m_unary(s, 1), m_unary(s, 0));
    }

    // Setup pairwise weights for node i with all neighbors j as:
    // source -> i: E_ij(1,0)
    // i -> sink: E_ij(0,0)
    // i -> j: E_ij(0,1) + E_ij(1,0) - E_ij(0,0) - E_ij(1,1)
    if (m_pairwise)
    {
        for (auto const& e : m_graph)
        {
            m_pMaxFlow->add_tweights(e.first, m_pairwise(e.first, e.second, 1, 0), m_pairwise(e.first, e.second, 0, 0));
            auto energy = m_pairwise(e.first, e.second, 0, 1) + m_pairwise(e.first, e.second, 1, 0)
                          - m_pairwise(e.first, e.second, 0, 0) - m_pairwise(e.first, e.second, 1, 1);
            m_pMaxFlow->add_edge(e.first, e.second, energy, 0);
        }
    }
}
