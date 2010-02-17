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
 * \file cgt/minspantree/prim/prim_iterator.h
 * \brief Contains definition of an iterator that implements Prim algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ITERATOR_H_
#define __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ITERATOR_H_

#include "prim_adjacency_heap.h"
#include "../../base/iterator/iterator_ptr.h"
#include "../../base/hash.h"


namespace cgt
{
  namespace base
  {
    /*
    template<typename _TpVertex, typename _TpEdge>
      class _HeapInvariant<_GraphAdjacency<_TpVertex, _TpEdge>*>
      {
        private:
          typedef _GraphAdjacency<_TpVertex, _TpEdge> _Adjacency;
          typedef _Adjacency*                         _AdjacencyPtr;

        public:
          static const bool _less_than (const _AdjacencyPtr& _child, const _AdjacencyPtr& _parent)
          {
            return (_child->edge ().value () < _parent->edge ().value ());
          }
      };
      */
  }

  namespace minspantree
  {
    /*!
     * \namespace cgt::minspantree::prim
     * \brief Where are defined the Prim iterator's related structures
     * \author Leandro Costa
     * \date 2009
     */

    namespace prim
    {
      /*!
       * \class _PrimIterator
       * \brief An iterator that executes Prim Algorithm
       * \author Leandro Costa
       * \date 2009
       *
       * The prim iterator returns edges in sequence according to the
       * Prim Algorithm.
       */

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator = cgt::base::iterator::_TpCommon>
        class _PrimIterator : public cgt::base::iterator::_IteratorPtr<_GraphAdjacency<_TpVertex, _TpEdge>, _TpIterator>
        {
          private:
            typedef _PrimIterator<_TpVertex, _TpEdge, _TpIterator>  _Self;
            typedef _PrimIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpCommon>    _SelfCommon;

          private:
            typedef _GraphVertex<_TpVertex>             _Vertex;
            typedef _GraphEdge<_TpVertex, _TpEdge>      _Edge;

            typedef _GraphNode<_TpVertex, _TpEdge>      _Node;
            typedef cgt::base::list<_Node>        _NodeList;
            typedef typename _NodeList::iterator        _NodeIterator;

            typedef _GraphAdjacency<_TpVertex, _TpEdge> _Adjacency;
            typedef cgt::base::list<_Adjacency>   _AdjacencyList;
            typedef typename _AdjacencyList::iterator   _AdjacencyIterator;

            typedef cgt::base::iterator::_IteratorPtr<_Adjacency, _TpIterator>    _Base;

          private:
            typedef cgt::base::heap<_Adjacency*, _PrimAdjacencyHeapInvariant>  _AdjacencyHeap;
            typedef cgt::base::hash<_Vertex*, bool> _VertexHash;

          private:
            using _Base::_ptr;

          public:
            _PrimIterator () { }
            _PrimIterator (_Node* const _ptr_n)
            {
              if (_ptr_n)
                _init (*_ptr_n);
            }
            _PrimIterator (const _NodeIterator& _it) { _init (*_it); }
            _PrimIterator (const _SelfCommon& _it) { }
            virtual ~_PrimIterator () { }

          private:
            void _init (_Node& _n);
            void _incr ();

          public:
            _Edge& operator*() const { return _ptr->edge (); }
            _Edge* operator->() const { return &(_ptr->edge ()); }
            _Self& operator++();
            const _Self operator++(int);

          private:
            _AdjacencyHeap  _adjHeap;
            _VertexHash     _vertexHash;
        };


      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        void _PrimIterator<_TpVertex, _TpEdge, _TpIterator>::_init (_Node& _n)
        {
          _AdjacencyIterator _itEnd = _n.adjlist ().end ();
          for (_AdjacencyIterator _it = _n.adjlist ().begin (); _it != _itEnd; ++_it)
            _adjHeap.push (&(*_it));

          _Adjacency** _p = _adjHeap.pop ();
          _ptr = *_p;
          delete _p;

          _vertexHash.insert (&(_ptr->edge ().v1 ()), true);
          _vertexHash.insert (&(_ptr->edge ().v2 ()), true);

          _itEnd = _ptr->node ().adjlist ().end ();
          for (_AdjacencyIterator _it = _ptr->node ().adjlist ().begin (); _it != _itEnd; ++_it)
          {
            if (! _vertexHash [&(_it->node ().vertex ())])
              _adjHeap.push (&(*_it));
          }
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        void _PrimIterator<_TpVertex, _TpEdge, _TpIterator>::_incr ()
        {
          _ptr = NULL;

          while (! _adjHeap.empty ())
          {
            _Adjacency** _p = _adjHeap.pop ();
            _ptr = *_p;
            delete _p;

            if (! _vertexHash [&(_ptr->edge ().v1 ())] || ! _vertexHash [&(_ptr->edge ().v2 ())])
              break;
            else
              _ptr = NULL;
          }

          if (_ptr)
          {
            _AdjacencyIterator _itEnd = _ptr->node ().adjlist ().end ();
            for (_AdjacencyIterator _it = _ptr->node ().adjlist ().begin (); _it != _itEnd; ++_it)
            {
              if (! _vertexHash [&(_it->node ().vertex ())])
                _adjHeap.push (&(*_it));
            }

            if (! _vertexHash [&(_ptr->edge ().v1 ())])
              _vertexHash.insert (&(_ptr->edge ().v1 ()), true);
            if (! _vertexHash [&(_ptr->edge ().v2 ())])
              _vertexHash.insert (&(_ptr->edge ().v2 ()), true);
          }
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        _PrimIterator<_TpVertex, _TpEdge, _TpIterator>& _PrimIterator<_TpVertex, _TpEdge, _TpIterator>::operator++()
        {
          _incr ();
          return *this;
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        const _PrimIterator<_TpVertex, _TpEdge, _TpIterator> _PrimIterator<_TpVertex, _TpEdge, _TpIterator>::operator++(int)
        {
          _Self _it = *this;
          _incr ();
          return _it;
        }
    }
  }
}

#endif // __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ITERATOR_H_
