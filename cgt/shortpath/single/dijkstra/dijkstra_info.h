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
 * \file cgt/shortpath/single/dijkstra/dijkstra_info.h
 * \brief Contains definition of a structure where we put node's informations about Dijkstra algorithm execution.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_H_
#define __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_H_

#include "dijkstra_distance.h"
#include "../../../graph_node.h"


namespace cgt
{
  namespace shortpath
  {
    namespace single
    {
      namespace dijkstra
      {
        /*!
         * \class _DijkstraInfo
         * \brief A structure used to keep information about each node by dijkstra iterator.
         * \author Leandro Costa
         * \date 2009
         *
         * This structure keeps informations about nodes while executing
         * Dijkstra Algorithm, like distance.
         */

        template<typename _TpVertex, typename _TpEdge>
          class _DijkstraInfo
          {
            private:
              typedef _DijkstraInfo<_TpVertex, _TpEdge> _Self;
              typedef _GraphNode<_TpVertex, _TpEdge>    _Node;
              typedef _DijkstraDistance<_TpEdge>        _Distance;

            public:
              _DijkstraInfo (_Node& _n) : _node (_n), _ptr_previous (NULL) { }
              _DijkstraInfo (const _DijkstraInfo& _d) : _node (_d._node), _distance (_d._distance), _ptr_previous (_d._ptr_previous) { }

            public:
              const bool operator==(const _DijkstraInfo& _d) const { return (_node.vertex () == _d._node.vertex ()); }
              const bool operator!=(const _DijkstraInfo& _d) const { return ! (*this == _d); }
              const bool operator<(const _DijkstraInfo& _d) const { return (! inf_distance () && (_d.inf_distance () || distance () < _d.distance ())); }
              const bool operator<=(const _DijkstraInfo& _d) const { return (! inf_distance () && (_d.inf_distance () || distance () <= _d.distance ())); }

            public:
              _Node& node () const { return _node; }
              void _set_distance (const _TpEdge &_d) { _distance._set_distance (_d); }
              const _TpEdge& distance () const { return _distance.distance (); }
              const bool &inf_distance () const { return _distance.inf_distance (); }
              void _set_previous (const _Node* const _ptr) { _ptr_previous = _ptr; }
              const _Node* const previous () const { return _ptr_previous; }

            private:
              _Node&        _node;
              _Distance     _distance;
              const _Node*  _ptr_previous;
          };
      }
    }
  }
}

#endif // __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_H_
