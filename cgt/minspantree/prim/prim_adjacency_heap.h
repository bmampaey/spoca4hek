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
 * \file cgt/minspantree/prim/prim_adjacency_heap.h
 * \brief Contains definition of the heap used by Prim algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ADJACENCY_HEAP_H_
#define __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ADJACENCY_HEAP_H_

#include "../../base/heap.h"
#include "../../graph_adjacency.h"


namespace cgt
{
  namespace minspantree
  {
    namespace prim
    {
      /*!
       * \class _PrimAdjacencyHeapInvariant
       * \brief Invariant used to contruct a heap for Prim Algorithm.
       * \author Leandro Costa
       * \date 2009
       *
       * The new invariant is necessary because we need an indirect heap,
       * based on a pointers's array.
       */

      template<typename _TpItem>
        class _PrimAdjacencyHeapInvariant
        {
          public:
            virtual const bool operator() (const _TpItem& _child, const _TpItem& _parent) const
            {
              return (_child->edge ().value () < _parent->edge ().value ());
            }
      };
    }
  }
}



#endif // __CGTL__CGT_MINSPANTREE_PRIM_PRIM_ADJACENCY_HEAP_H_
