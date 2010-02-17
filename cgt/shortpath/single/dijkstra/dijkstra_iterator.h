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
 * \file cgt/shortpath/single/dijkstra/dijkstra_iterator.h
 * \brief Contains definition of an iterator that implements Dijkstra algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_ITERATOR_H_
#define __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_ITERATOR_H_


#include "dijkstra_info_list.h"
#include "dijkstra_info_heap.h"

namespace cgt
{
    /*!
     * \namespace cgt::shortpath
     * \brief Where are defined structures related to shortest-path algorithms.
     * \author Leandro Costa
     * \date 2009
     */

  namespace shortpath
  {
    /*!
     * \namespace cgt::shortpath::single
     * \brief Where are defined structures related to single-source shortest-path algorithms.
     * \author Leandro Costa
     * \date 2009
     */

    namespace single
    {
    /*!
     * \namespace cgt::shortpath::single::dijkstra
     * \brief Where are defined structures related to Dijkstra iterator.
     * \author Leandro Costa
     * \date 2009
     */

      namespace dijkstra
      {
        /*!
         * \class _DijkstraIterator
         * \brief An iterator that implements dijkstra algorithm.
         * \author Leandro Costa
         * \date 2009
         *
         * This iterator executes Dijkstra Algorithm and returns nodes
         * in the order found by the algorithm.
         */

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator = cgt::base::iterator::_TpCommon>
          class _DijkstraIterator
          {
            private:
              friend class _DijkstraIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpConst>;

            private:
              typedef _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>  _Self;
              typedef _DijkstraIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpCommon>    _SelfCommon;

            public:
              typedef _DijkstraInfo<_TpVertex, _TpEdge>     _Info;

            private:
              typedef _DijkstraInfoList<_TpVertex, _TpEdge> _InfoList;
              typedef _DijkstraInfoHeap<_TpVertex, _TpEdge> _InfoHeap;
              typedef _GraphNode<_TpVertex, _TpEdge>        _Node;
              typedef _GraphAdjList<_TpVertex, _TpEdge>     _AdjList;
              typedef typename cgt::base::list<_Node>::iterator        _NodeIterator;
              typedef typename cgt::base::list<_Info>::iterator        _InfoIterator;
              typedef typename _AdjList::const_iterator     _AdjListIterator;

            public:
              _DijkstraIterator () { }
              /*
               * The constructors need to insert all nodes (except _ptr_n) in _notVisitedInfoHeap with distance == infinite,
               * and after that, calculate distance for the nodes from adjacency list of _ptr_n.
               */
              _DijkstraIterator (_Node* const _ptr_n) : _ptr_node (_ptr_n) { }
              _DijkstraIterator (_Node* const _ptr_n, const _NodeIterator& _it_begin, const _NodeIterator& _it_end) : _ptr_node (_ptr_n), _it_node (_it_begin), _it_node_end (_it_end)
            {
              if (_ptr_node)
                _init ();
            }
              _DijkstraIterator (const _NodeIterator& _it, const _NodeIterator& _it_begin, const _NodeIterator& _it_end) : _ptr_node (&(*_it)), _it_node (_it_begin), _it_node_end (_it_end)
            {
              if (_ptr_node)
                _init ();
            }
              _DijkstraIterator (const _SelfCommon& _it) : _ptr_node ( _it._ptr_node), _it_node (_it._it_node), _it_node_end (_it._it_node_end), _infoList (_it._infoList), _notVisitedInfoHeap (_it._notVisitedInfoHeap) { }

            private:
              void _init ();
              const _Info* const _get_info_by_node (const _Node* const _ptr_node);

            public:
              _Node& operator*() const;
              _Node* operator->() const;
              const bool operator==(const _Self& _other) const;
              const bool operator!=(const _Self& _other) const;
              _Self& operator++();

            public:
              const _Info* const info (const _Node* const _ptr_node) { return _get_info_by_node (_ptr_node); }
              const _Info* const info (const _Node& _node) { return _get_info_by_node (&_node); }

              _InfoIterator info_begin () { return _infoList.begin (); }
              _InfoIterator info_end () { return _infoList.end (); }

            private:
              _Node*        _ptr_node;
              _NodeIterator _it_node;
              _NodeIterator _it_node_end;
              _InfoList     _infoList;
              _InfoHeap     _notVisitedInfoHeap;
          };

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          void _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::_init ()
          {
            /*
             * Insert all nodes (except _ptr_node) in _notVisitedInfoHeap with distance == infinite,
             * calculate distance for the nodes from adjacency list of _ptr_n.
             */

            for (_NodeIterator it = _it_node; it != _it_node_end; ++it)
            {
              if (&(*it) == _ptr_node)
                _infoList.insert (_Info (*it));
              else
                _notVisitedInfoHeap.push (_Info (*it));
            }

            const _AdjList &adjList = _ptr_node->adjlist ();
            _AdjListIterator itA = adjList.begin ();
            _AdjListIterator itAEnd = adjList.end ();

            for (itA = adjList.begin (); itA != itAEnd; ++itA)
              _notVisitedInfoHeap.relax (&(itA->node ()), _TpEdge (), itA->edge (), &(_infoList.back ()->node ()));
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          const _DijkstraInfo<_TpVertex, _TpEdge>* const _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::_get_info_by_node (const _Node* const _ptr_node)
          {
            const _Info* _ptr = NULL;

            typename _InfoList::const_iterator _it = _infoList.get_by_node (_ptr_node);

            if (_it != _infoList.end ())
              _ptr = &(*_it);
            else
            {
              typename _InfoHeap::const_iterator _it = _notVisitedInfoHeap.get_by_node (_ptr_node);

              if (_it != _notVisitedInfoHeap.end ())
                _ptr = &(*_it);
            }

            return _ptr;
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          _GraphNode<_TpVertex, _TpEdge>& _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::operator*() const
          {
            return *_ptr_node;
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          _GraphNode<_TpVertex, _TpEdge>* _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::operator->() const
          {
            return _ptr_node;
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>& _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::operator++()
          {
            /*
             * Remove the node with lowest distance from _notVisitedInfoHeap
             * and recalculate distances for the adjacency list of the chosen node.
             */

            _ptr_node = NULL;

            if (! _notVisitedInfoHeap.empty () && ! _notVisitedInfoHeap.get_closest ()->inf_distance ())
            {
              _Info* _ptr = _notVisitedInfoHeap.pop ();

              _ptr_node = &(_ptr->node ());
              _infoList.insert (*_ptr);

              const _AdjList &adjList = _ptr_node->adjlist ();
              _AdjListIterator itA = adjList.begin ();
              _AdjListIterator itAEnd = adjList.end ();

              for (itA = adjList.begin (); itA != itAEnd; ++itA)
                _notVisitedInfoHeap.relax (&(itA->node ()), _ptr->distance (), itA->edge(), &(_infoList.back ()->node ()));
            }

            return *this;
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          const bool _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::operator==(const _Self& _other) const
          {
            return (_ptr_node == _other._ptr_node);
          }

        template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
          const bool _DijkstraIterator<_TpVertex, _TpEdge, _TpIterator>::operator!=(const _Self& _other) const
          {
            return !(*this == _other);
          }
      }
    }
  }
}

#endif // __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_ITERATOR_H_
