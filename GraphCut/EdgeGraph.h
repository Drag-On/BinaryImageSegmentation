//
// Created by jan on 11.12.15.
//

#ifndef EDGE_GRAPH_H
#define EDGE_GRAPH_H

#include <map>

/**
 * @brief A very simple graph implementation, intended to store adjacency information
 */
class EdgeGraph
{
public:
    /**
     * @brief Index of a vertex
     */
    using Vertex = unsigned int;

    /**
     * @brief Inserts a new edge into the graph
     * @param v1 First vertex
     * @param v2 Second vertex
     */
    void insertEdge(Vertex v1, Vertex v2);

    /**
     * @brief Checks if a certain connection between vertices is in the graph
     * @param v1 First vertex
     * @param v2 Second vertex
     * @returns true in case the edge is in the graph, otherwise false
     */
    bool hasEdge(Vertex v1, Vertex v2) const;

    /**
     * @returns Provides an iterator over all the edges
     */
    std::multimap<Vertex, Vertex>::const_iterator begin() const;

    /**
     * @returns Provides an iterator past the last edge
     */
    std::multimap<Vertex, Vertex>::const_iterator end() const;

    /**
     * @returns the number of vertices in the graph
     */
    unsigned int nVertices() const;

    /**
     * @returns the number of edges in the graph
     */
    unsigned int nEdges() const;

    /**
     * @returns true in case the graph is empty
     */
    bool empty() const;

private:
    Vertex m_maxVertex = 0;
    std::multimap<Vertex, Vertex> m_edges;
};

#endif // EDGE_GRAPH_H