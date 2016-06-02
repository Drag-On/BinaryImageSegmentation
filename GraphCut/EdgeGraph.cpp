//
// Created by jan on 11.12.15.
//

#include "EdgeGraph.h"

void EdgeGraph::insertEdge(Vertex v1, Vertex v2)
{
    if (m_edges.count(v1) > 0)
    {
        auto range = m_edges.equal_range(v1);
        for (auto it = range.first; it != range.second; ++it)
            if (it->second == v2)
                return;
        m_edges.insert(std::pair<Vertex, Vertex>(v1, v2));
    }
    else
    {
        auto range = m_edges.equal_range(v2);
        for (auto it = range.first; it != range.second; ++it)
            if (it->second == v1)
                return;
        m_edges.insert(std::pair<Vertex, Vertex>(v2, v1));
    }
    m_maxVertex = std::max(m_maxVertex, std::max(v1, v2));
}

bool EdgeGraph::hasEdge(Vertex v1, Vertex v2) const
{
    auto range = m_edges.equal_range(v1);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == v2)
            return true;
    }
    range = m_edges.equal_range(v2);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == v1)
            return true;
    }
    return false;
}

std::multimap<EdgeGraph::Vertex, EdgeGraph::Vertex>::const_iterator EdgeGraph::begin() const
{
    return m_edges.begin();
}

std::multimap<EdgeGraph::Vertex, EdgeGraph::Vertex>::const_iterator EdgeGraph::end() const
{
    return m_edges.end();
}

unsigned int EdgeGraph::nVertices() const
{
    return m_maxVertex + 1;
}

unsigned int EdgeGraph::nEdges() const
{
    return m_edges.size();
}

bool EdgeGraph::empty() const
{
    return m_edges.empty();
}
