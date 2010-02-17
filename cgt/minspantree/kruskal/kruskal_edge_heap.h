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
 * \file cgt/minspantree/kruskal/kruskal_edge_heap.h
 * \brief Contains definition of the heap used by Kruskal algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-01 01:53:45 +0100 (Tue, 01 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 95 $
 */

#ifndef __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_EDGE_HEAP_H_
#define __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_EDGE_HEAP_H_

#include "../../base/heap.h"
#include "../../graph_edge.h"


namespace cgt
{
  namespace minspantree
  {
    namespace kruskal
    {
      /*!
       * \class _KruskalEdgeHeapInvariant
       * \brief The invariant used to construct a heap used by kruskal iterator.
       * \author Leandro Costa
       * \date 2009
       *
       *  The invariant to be used while constructing the heap used by kruskal.
       *  The difference from the default invariant is that in this case we have
       *  a vector of pointers to structures, and not a vector of structures. Because
       *  that we need to access the value of the object pointed to each vector's item.
       */

      template<typename _TpItem>
        class _KruskalEdgeHeapInvariant
        {
          public:
            virtual const bool operator() (const _TpItem& _child, const _TpItem& _parent) const
            {
              return (_child->value () < _parent->value ());
            }
        };

      /*
      template<typename _TpItem>
        class _KruskalEdgeHeap : public cgt::base::heap<_TpItem, _KruskalEdgeHeapInvariant>
        {
          protected:
            virtual const bool _less_than (const _TpItem& _child, const _TpItem& _parent) const
            {
              return (_child->value () < _parent->value ());
            }
        };
        */
    }
  }
}



#endif // __CGTL__CGT_MINSPANTREE_KRUSKAL_KRUSKAL_EDGE_HEAP_H_
