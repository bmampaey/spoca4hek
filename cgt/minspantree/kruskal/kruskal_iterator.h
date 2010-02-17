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
 * \file cgt/minspantree/kruskal/kruskal_iterator.h
 * \brief Contains definition of an iterator that implements Kruskal algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_ITERATOR_H_
#define __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_ITERATOR_H_

#include "kruskal_edge_heap.h"
#include "../../base/iterator/iterator_ptr.h"
#include "../../base/hash.h"


namespace cgt
{
  template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
    class _VertexIterator;

  /*!
   * \namespace cgt::minspantree
   * \brief Where are defined structures related to minimum spanning tree algoriths.
   * \author Leandro Costa
   * \date 2009
   */

  namespace minspantree
  {
    /*!
     * \namespace cgt::minspantree::kruskal
     * \brief Where are defined the Kruskal iterator's related structures
     * \author Leandro Costa
     * \date 2009
     */

    namespace kruskal
    {
      /*!
       * \class _KruskalIterator
       * \brief An iterator that executes kruskal algorithm
       * \author Leandro Costa
       * \date 2009
       *
       * The kruskal iterator returns edges in sequence according to the
       * Kruskal Algorithm.
       */

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator = cgt::base::iterator::_TpCommon>
        class _KruskalIterator : public cgt::base::iterator::_IteratorPtr<_GraphEdge<_TpVertex, _TpEdge>, _TpIterator>
        {
          private:
            class _SetInfo;

          private:
            typedef _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>  _Self;
            typedef _KruskalIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpCommon>    _SelfCommon;

          private:
            typedef _GraphNode<_TpVertex, _TpEdge>      _Node;
            typedef _GraphEdge<_TpVertex, _TpEdge>      _Edge;
            typedef _GraphVertex<_TpVertex>             _Vertex;
            typedef cgt::base::iterator::_IteratorPtr<_Edge, _TpIterator>    _Base;

            typedef _VertexIterator<_TpVertex, _TpEdge>       _viterator;
            typedef typename cgt::base::list<_Edge>::iterator _eiterator;

            typedef cgt::base::list<_Node>        _NodeList;
            typedef typename _NodeList::iterator  _NodeIterator;

          private:
            typedef cgt::base::heap<_Edge*, _KruskalEdgeHeapInvariant>  _EdgeHeap;
            typedef cgt::base::vector<_SetInfo>                         _SetVector;
            typedef cgt::base::hash<_Vertex*, size_t>                   _VertexSetHash;

          private:
            using _Base::_ptr;

          private:
            /*!
             * \struct _SetInfo
             * \brief A structure to represent the sets used by Kruskal Algorithm.
             * \author Leandro Costa
             * \date 2009
             *
             * Implementation found in book <b>Algorithms, by S. Dasgupta, C.H. Papadimitriou, and U.V. Vazirani</b>.
             */

            struct _SetInfo
            {
              _SetInfo (const size_t& _pos) { _set = _pos; _rank = 0; }
              const _SetInfo* operator()() { return this; }
              size_t  _set;
              size_t  _rank;
            };

          public:
            _KruskalIterator () { }
            _KruskalIterator (_Node* const _ptr_n) { }
            _KruskalIterator (_Node* const _ptr_n, _viterator _vit, _viterator _vitEnd, _eiterator _eit, _eiterator _eitEnd)
            {
              if (_ptr_n)
                _init (*_ptr_n, _vit, _vitEnd, _eit, _eitEnd);
            }
            _KruskalIterator (const _NodeIterator& _it, _viterator _vit, _viterator _vitEnd, _eiterator _eit, _eiterator _eitEnd) { _init (*_it, _vit, _vitEnd, _eit, _eitEnd); }
            _KruskalIterator (const _SelfCommon& _it) { }
            virtual ~_KruskalIterator () { }

          private:
            void _init (_Node& _n, _viterator _vit, _viterator _vitEnd, _eiterator _eit, _eiterator _eitEnd);
            void _incr ();
            const bool _set_same (size_t _pos1, size_t _pos2) const
            {
              bool _ret = false;

              if (_pos1 < _setVector.size () && _pos2 < _setVector.size ())
              {
                while (_setVector [_pos1]._set != _pos1)
                  _pos1 = _setVector [_pos1]._set;

                while (_setVector [_pos2]._set != _pos2)
                  _pos2 = _setVector [_pos2]._set;

                _ret = (_pos1 == _pos2);
              }

              return _ret;
            }

            void _set_union (size_t _pos1, size_t _pos2)
            {
              if (_pos1 < _setVector.size () && _pos2 < _setVector.size ())
              {
                while (_setVector [_pos1]._set != _pos1)
                  _pos1 = _setVector [_pos1]._set;

                while (_setVector [_pos2]._set != _pos2)
                  _pos2 = _setVector [_pos2]._set;

                if (_pos1 != _pos2)
                {
                  if (_setVector [_pos1]._rank > _setVector [_pos2]._rank)
                  {
                    _setVector [_pos2]._set = _pos1;
                  }
                  else
                  {
                    _setVector [_pos1]._set = _pos2;

                    if (_setVector [_pos1]._rank == _setVector [_pos2]._rank)
                      _setVector [_pos2]._rank++;
                  }
                }
              }
            }

          public:
            _Edge& operator*() const { return *_ptr; }
            _Edge* operator->() const { return _ptr; }
            _Self& operator++();
            const _Self operator++(int);

          private:
            _EdgeHeap       _edgeHeap;
            _SetVector      _setVector;
            _VertexSetHash  _vertexSetHash;
        };


      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        void _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>::_init (_Node& _n, _viterator _vit, _viterator _vitEnd, _eiterator _eit, _eiterator _eitEnd)
        {
          while (_vit != _vitEnd)
          {
            _vertexSetHash.insert (&(*_vit), _setVector.size ());
            _setVector.push_back (_SetInfo (_setVector.size ()));
            ++_vit;
          }

          while (_eit != _eitEnd)
          {
            _edgeHeap.push (&(*_eit));
            ++_eit;
          }

          _Edge** _p = _edgeHeap.pop ();
          _ptr = *_p;
          delete _p;

          _set_union (*(_vertexSetHash [&(_ptr->v1 ())]), *(_vertexSetHash [&(_ptr->v2 ())]));
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        void _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>::_incr ()
        {
          _ptr = NULL;

          while (! _edgeHeap.empty ())
          {
            _Edge** _p = _edgeHeap.pop ();
            _ptr = *_p;
            delete _p;

            if (! _set_same (*(_vertexSetHash [&(_ptr->v1 ())]), *(_vertexSetHash [&(_ptr->v2 ())])))
            {
              _set_union (*(_vertexSetHash [&(_ptr->v1 ())]), *(_vertexSetHash [&(_ptr->v2 ())]));
              break;
            }
            else
              _ptr = NULL;
          }
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>& _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>::operator++()
        {
          _incr ();
          return *this;
        }

      template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
        const _KruskalIterator<_TpVertex, _TpEdge, _TpIterator> _KruskalIterator<_TpVertex, _TpEdge, _TpIterator>::operator++(int)
        {
          _Self _it = *this;
          _incr ();
          return _it;
        }
    }
  }
}

#endif // __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_ITERATOR_H_
