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
 * \file cgt/shortpath/single/dijkstra/dijkstra_info_list.h
 * \brief Containss definition of a list of structures used by Dijkstra iterator.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_LIST_H_
#define __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_LIST_H_

#include "../../../graph_node.h"
#include "dijkstra_info.h"
#include "../../../base/list.h"


namespace cgt
{
  namespace shortpath
  {
    namespace single
    {
      namespace dijkstra
      {
        /*!
         * \class _DijkstraInfoList
         * \brief A list of dijkstra info structures used by dijkstra iterator.
         * \author Leandro Costa
         * \date 2009
         *
         * The list used by Dijkstra Algorithm to keep the nodes already taken.
         * This is also the list of dijkstra infos that can be obtained after
         * the algorithm finish.
         */

        template<typename _TpVertex, typename _TpEdge>
          class _DijkstraInfoList : public cgt::base::list<_DijkstraInfo<_TpVertex, _TpEdge> >
        {
          private:
            typedef _DijkstraInfo<_TpVertex, _TpEdge>     _Info;
            typedef _DijkstraInfoList<_TpVertex, _TpEdge> _Self;
            typedef cgt::base::list<_Info>                           _Base;

          private:
            typedef _GraphNode<_TpVertex, _TpEdge>    _Node;
            typedef _GraphEdge<_TpVertex, _TpEdge>    _Edge;
            typedef typename _Base::iterator          _Iterator;

          private:
            static const bool _has_node (const _Info& _info, const _Node& _node) { return (_info.node ().vertex () == _node.vertex ()); }

          public:
            _Iterator get_by_node (const _Node* const _ptr_node);
            _Iterator get_closest ();
            void relax (const _Node* const _ptr_node, const _TpEdge& _distance, const _Edge& _edge, _Node* const _ptr_prev);
        };

        template<typename _TpVertex, typename _TpEdge>
          typename _DijkstraInfoList<_TpVertex, _TpEdge>::_Iterator _DijkstraInfoList<_TpVertex, _TpEdge>::get_by_node (const _Node* const _ptr_node)
          {
            return find_if (_Base::begin (), _Base::end (), _has_node, *_ptr_node);
          }

        template<typename _TpVertex, typename _TpEdge>
          typename _DijkstraInfoList<_TpVertex, _TpEdge>::_Iterator _DijkstraInfoList<_TpVertex, _TpEdge>::get_closest ()
          {
            _Iterator it_ret = _Base::begin ();

            if (! _Base::empty ())
            {
              _Iterator it = it_ret;
              _Iterator itEnd = _Base::end ();

              for (++it; it != itEnd; ++it)
              {
                if (! it->inf_distance () && *it < *it_ret)
                  it_ret = it;
              }
            }

            return it_ret;
          }

        template<typename _TpVertex, typename _TpEdge>
          void _DijkstraInfoList<_TpVertex, _TpEdge>::relax (const _Node* const _ptr_node, const _TpEdge& _distance, const _Edge& _edge, _Node* const _ptr_prev)
          {
            _Iterator it = get_by_node (_ptr_node);

            if (it != _Base::end ())
            {
              _TpEdge new_distance = _distance + _edge.value ();

              if (it->inf_distance () || it->distance () > new_distance)
              {
                it->_set_distance (new_distance);
                it->_set_previous (_ptr_prev);
              }
            }
          }
      }
    }
  }
}


#endif // __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_INFO_LIST_H_
