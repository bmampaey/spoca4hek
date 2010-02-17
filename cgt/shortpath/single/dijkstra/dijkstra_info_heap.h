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
 * \file cgt/shortpath/single/dijkstra/dijkstra_info_heap.h
 * \brief Contains definition of the heap used by Dijkstra algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_HEAP_H_
#define __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_HEAP_H_

#include "dijkstra_info.h"
#include "../../../graph_node.h"
#include "../../../base/heap.h"


namespace cgt
{
  namespace shortpath
  {
    namespace single
    {
      namespace dijkstra
      {
        /*!
         * \class _DijkstraInfoHeap
         * \brief A heap of dijkstra info structures used by dijkstra iterator.
         * \author Leandro Costa
         * \date 2009
         *
         * The heap used by Dijkstra Algorithm to get the closest node
         * on each iteration.
         */

        template<typename _TpVertex, typename _TpEdge>
          class _DijkstraInfoHeap : public cgt::base::heap<_DijkstraInfo<_TpVertex, _TpEdge> >
        {
          private:
            typedef _DijkstraInfo<_TpVertex, _TpEdge>     _Info;
            typedef cgt::base::heap<_Info>                _Base;
            typedef _DijkstraInfoHeap<_TpVertex, _TpEdge> _Self;

          private:
            typedef _GraphNode<_TpVertex, _TpEdge>  _Node;
            typedef _GraphEdge<_TpVertex, _TpEdge>  _Edge;
            typedef typename _Base::const_iterator  _ConstIterator;

          public:
            static void _set_previous (_Info& _info, const _Node* const _previous) { _info._set_previous (_previous); }
            static void _set_distance (_Info& _info, const _TpEdge& _distance) { _info._set_distance (_distance); }

          private:
            static const bool _has_node (const _Info& _info, const _Node& _node) { return (_info.node ().vertex () == _node.vertex ()); }

          public:
            _ConstIterator get_by_node (const _Node* const _ptr_node);
            _ConstIterator get_closest ();
            void relax (const _Node* const _ptr_node, const _TpEdge& _distance, const _Edge& _edge, _Node* const _ptr_prev);
        };

        template<typename _TpVertex, typename _TpEdge>
          typename _DijkstraInfoHeap<_TpVertex, _TpEdge>::_ConstIterator _DijkstraInfoHeap<_TpVertex, _TpEdge>::get_by_node (const _Node* const _ptr_node)
          {
            return find_if (_Base::begin (), _Base::end (), _has_node, *_ptr_node);
          }

        template<typename _TpVertex, typename _TpEdge>
          typename _DijkstraInfoHeap<_TpVertex, _TpEdge>::_ConstIterator _DijkstraInfoHeap<_TpVertex, _TpEdge>::get_closest ()
          {
            return _Base::begin ();
          }

        template<typename _TpVertex, typename _TpEdge>
          void _DijkstraInfoHeap<_TpVertex, _TpEdge>::relax (const _Node* const _ptr_node, const _TpEdge& _distance, const _Edge& _edge, _Node* const _ptr_prev)
          {
            _ConstIterator it = get_by_node (_ptr_node);

            if (it != static_cast<const _Self*> (this)->_Base::end ())
            {
              _TpEdge _new_distance = _distance + _edge.value ();

              if (it->inf_distance () || it->distance () > _new_distance)
              {
                _Base::modify_by (it, _set_distance, _new_distance);
                _Base::modify_by (it, _set_previous, _ptr_prev);
              }
            }
          }
      }
    }
  }
}


#endif // __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_HEAP_H_
