/*
 * CGTL - A graph template library for C++
 * ---------------------------------------
 * Copyright (C) 2009 Leandro Costa
 *
 * This file is part of CGTL.
 *
 * CGTL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * CGTL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with CGTL. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \file cgt/stconncomp/graph_scc_node.h
 * \brief Contains definition of a thin node used by SCC iterator.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_STCONNCOMP_GRAPH_SCC_NODE_H_
#define __CGTL__CGT_STCONNCOMP_GRAPH_SCC_NODE_H_

#include "../graph_node.h"
#include "../graph_adjlist.h"


namespace cgt
{
  namespace stconncomp
  {
    /*!
     * \class _GraphSCCNode
     * \brief The node used by _GraphSCCComponent. It has a reference to the real node.
     * \author Leandro Costa
     * \date 2009
     *
     * This is the node used by strongly connected component structure. It is a
     * special node with a reference to the real node and a list of adjacencies.
     */

    template<typename _TpVertex, typename _TpEdge>
      class _GraphSCCNode
      {
        private:
          typedef _GraphVertex<_TpVertex>           _Vertex;
          typedef _GraphNode<_TpVertex, _TpEdge>    _Node;
          typedef _GraphEdge<_TpVertex, _TpEdge>    _Edge;
          typedef _GraphAdjList<_TpVertex, _TpEdge> _AdjList;

        public:
          _GraphSCCNode (_Node& _n) : _node (_n) { }

        public:
          void _insert (_Edge& _e, _Node& _n) { _adjList._insert (_e, _n); }

        public:
          _Node& node () const { return _node; }
          const _Vertex& vertex () const { return _node.vertex (); }
          const _AdjList& adjlist () const { return _adjList; }

        private:
          _Node&    _node;
          _AdjList  _adjList;
      };
  }
}

#endif // __CGTL__CGT_STCONNCOMP_GRAPH_SCC_NODE_H_
