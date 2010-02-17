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
 * \file cgt/stconncomp/scc_iterator.h
 * \brief Contains definition of an iterator that implements SCC algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-02 00:05:33 +0100 (Wed, 02 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 96 $
 */

#ifndef __CGTL__CGT_STCONNCOMP_SCC_ITERATOR_H_
#define __CGTL__CGT_STCONNCOMP_SCC_ITERATOR_H_

#include "graph_scc_component.h"
#include "../base/heap.h"
#include "../base/iterator/iterator_type.h"
#include "../search/depth/depth_iterator.h"


/*!
 * invert graph;
 * run DFS (1);
 * invert graph again;
 * create heap with DFS (1);
 * while (! end)
 *  - pop N from heap, paint with GRAY and push in the stack;
 *  - run DFS (2) until N is poped from stack.
 */

namespace cgt
{
  /*!
   * \namespace cgt::stconncomp
   * \brief Where are defined structures related to strongly connected components iterator.
   * \author Leandro Costa
   * \date 2009
   */

  namespace stconncomp
  {
    /*!
     * \class _SCCIterator
     * \brief An iterator that returns strongly connected components.
     * \author Leandro Costa
     * \date 2009
     *
     * This iterator returns all strongly connected components that exist in the graph.
     */

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator = cgt::base::iterator::_TpCommon>
      class _SCCIterator
      {
        private:
          class _SCC_DFSInfo;

        private:
          typedef _SCCIterator<_TpVertex, _TpEdge, _TpIterator> _Self;
          typedef _SCCIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpCommon>   _SelfCommon;

        private:
          typedef _GraphNode<_TpVertex, _TpEdge>          _Node;
          typedef typename cgt::base::list<_Node>   _NodeList;
          typedef typename _NodeList::iterator            _NodeIterator;

          typedef _GraphSCCComponent<_TpVertex, _TpEdge>  _Component;
          typedef cgt::base::list<_Component>       _ComponentList;
          typedef typename _ComponentList::const_iterator _ComponentIterator;

          typedef _GraphSCCNode<_TpVertex, _TpEdge>       _SCCNode;

          typedef cgt::search::depth::_DepthIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpConst>  _DFSIterator;
          typedef typename _DFSIterator::_DepthInfo             _DFSInfo;
          typedef cgt::base::list<_DFSInfo>               _DFSInfoList;
          typedef typename _DFSInfoList::iterator               _DFSInfoIterator;
          typedef typename _DFSInfoList::const_iterator         _DFSInfoCIterator;
          typedef typename _DFSInfo::_color_t                   _DFSColor;
          typedef typename _DFSIterator::_DepthState            _DFSState;
          typedef cgt::base::stack<_DFSState>                   _DFSStateStack;

          typedef cgt::base::heap<_SCC_DFSInfo>       _SCC_DFSInfoHeap;
          typedef typename _SCC_DFSInfoHeap::iterator _SCC_DFSInfoHeapIterator;

          /*!
           * \class _SCC_DFSInfo
           * \brief A structure used to keep informations about SCC iterator.
           * \author Leandro Costa
           * \date 2009
           *
           * This structure is used to keep informations about strongly connected
           * components algorithm. It is a wrapper for a depth-first search info structure
           * with an operator les than (<) used to create a heap. \b S1 < \b S2 means the
           * finish time of \b S1 os greater or equals to the finish time of \b S2.
           */

          class _SCC_DFSInfo
          {
            public:
              _SCC_DFSInfo (_DFSInfo& _i) : _info (_i) { }

            public:
              const _DFSInfo& info () const { return _info; }
              _Node& node () { return _info.node (); }

            public:
              const bool operator<(const _SCC_DFSInfo& _b) const { return _info.finish () >= _b._info.finish (); }

            private:
              _DFSInfo& _info;
          };

        public:
          _SCCIterator () : _ptr_component (NULL) { }
          _SCCIterator (const _NodeIterator& _it_begin, const _NodeIterator& _it_end) : _ptr_component (NULL), _it_node_begin (_it_begin), _it_node_end (_it_end) { _init (); }
          _SCCIterator (const _SelfCommon& _it) { *this = _it; }

        _Self& operator=(const _Self& _s)
        {
          _it_node_begin    = _s._it_node_begin;
          _it_node_end      = _s._it_node_end;
          _it_rdfs          = _s._it_rdfs;
          _component_list   = _s._component_list;
          _rdfs_heap        = _s._rdfs_heap;
          _dfs_list         = _s._dfs_list;
          _dfs_state_stack  = _s._dfs_state_stack;

          /*!
           * \todo We need a better way to initialize _ptr_component.
           * It should point to the component pointed by _s._ptr_component:
           * 
           * \code
           *   _ptr_component = NULL;
           *
           *   if (_s._ptr_component)
           *   {
           *     _ComponentIterator _it = _component_list.find (*_s._ptr_component);
           *
           *     if (_it != _component_list.end ())
           *      _ptr_component  = &(*_it);
           *   }
           * \endcode
           */

          if (_s._ptr_component)
            _ptr_component  = _component_list.back ();

          return *this;
        }

        private:
          void _init ();

        private:
          void _run_reverse_dfs ();
          void _discover_node (const _Node& _node);
          void _finish_node (const _Node& _node);
          const bool _has_color (const _Node& _node, const _DFSColor &_color) const;
          _DFSInfo* _get_depth_info_by_node (const _Node& _node);
          _SCC_DFSInfo* _get_node_for_new_scc ();
          void _make_new_scc (_Node& _node);

        public:
          _Component& operator*() const { return *_ptr_component; }
          _Component* operator->() const { return _ptr_component; }
          const bool operator==(const _Self& _other) const { return (_ptr_component == _other._ptr_component); }
          const bool operator!=(const _Self& _other) const { return !(*this == _other); }
          _Self& operator++();

        private:
          _Component*       _ptr_component;
          _ComponentList    _component_list;

          _DFSIterator      _it_rdfs;
          _NodeIterator     _it_node_begin;
          _NodeIterator     _it_node_end;
          
          _DFSInfoList      _dfs_list;
          _DFSStateStack    _dfs_state_stack;
          _SCC_DFSInfoHeap  _rdfs_heap;
      };


    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      void _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_init ()
      {
        /*
         * Execute DFS in reverse graph and initialize heap
         * used to get nodes in decreasing finish time.
         */

        _run_reverse_dfs ();

        /*
         * Paint all nodes with WHITE and set finish time to 0.
         * These are the only important informations of DFS for
         * this SCC algorithm step.
         */

        _SCC_DFSInfoHeapIterator _itEnd = _rdfs_heap.end ();
        for (_SCC_DFSInfoHeapIterator _it = _rdfs_heap.begin (); _it != _itEnd; ++_it)
          _dfs_list.insert (_DFSInfo (_it->node ()));

        /*
         * Get the node with greater finish time
         * and try to make the first SCC.
         */

        _SCC_DFSInfo* _ptr_rdfs_info = _rdfs_heap.pop ();

        if (_ptr_rdfs_info)
        {
          _make_new_scc (_ptr_rdfs_info->node ());
          delete _ptr_rdfs_info;
        }
      }

    /*
     * Execute DFS in reverse graph and initialize heap
     * used to get nodes in decreasing finish time.
     */

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      void _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_run_reverse_dfs ()
      {
        for (_NodeIterator _it = _it_node_begin; _it != _it_node_end; ++_it)
          _it->_invert_edges ();

        _it_rdfs = _DFSIterator (_it_node_begin, _it_node_begin, _it_node_end);
        _DFSIterator  _it_rdfs_end (NULL, _it_node_begin, _it_node_end);

        while (_it_rdfs != _it_rdfs_end)
          ++_it_rdfs;

        for (_NodeIterator _it = _it_node_begin; _it != _it_node_end; ++_it)
          _it->_invert_edges ();

        _DFSInfoIterator _it_end (_it_rdfs.info_end ());
        for (_DFSInfoIterator _it (_it_rdfs.info_begin ()); _it != _it_end; ++_it)
          _rdfs_heap.push (_SCC_DFSInfo (*_it));
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      void _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_discover_node (const _Node& _node)
      {
        _DFSInfo *_ptr = _get_depth_info_by_node (_node);

        if (_ptr)
          _ptr->set_color (_DFSInfo::GRAY);
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      void _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_finish_node (const _Node& _node)
      {
        _DFSInfo *_ptr = _get_depth_info_by_node (_node);

        if (_ptr)
          _ptr->set_color (_DFSInfo::BLACK);
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      const bool _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_has_color (const _Node& _node, const _DFSColor &_color) const
      {
        bool bRet = false;

        _DFSInfoCIterator itEnd = _dfs_list.end ();

        for (_DFSInfoCIterator it = _dfs_list.begin (); it != itEnd; ++it)
        {
          if (it->node ().vertex () == _node.vertex ())
          {
            bRet = (it->color () == _color);
            break;
          }
        }

        return bRet;
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      typename cgt::search::depth::_DepthIterator<_TpVertex, _TpEdge, cgt::base::iterator::_TpConst>::_DepthInfo* _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_get_depth_info_by_node (const _Node& _node)
      {
        _DFSInfo *_ptr = NULL;

        _DFSInfoIterator itEnd = _dfs_list.end ();

        for (_DFSInfoIterator it = _dfs_list.begin (); it != itEnd; ++it)
        {
          if (it->node ().vertex () == _node.vertex ())
          {
            _ptr = &(*it);
            break;
          }
        }

        return _ptr;
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      typename _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_SCC_DFSInfo* _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_get_node_for_new_scc ()
      {
        _SCC_DFSInfo* _ptr_rdfs_info = _rdfs_heap.pop ();

        while (_ptr_rdfs_info && ! _has_color (_ptr_rdfs_info->node (), _DFSInfo::WHITE))
        {
          delete _ptr_rdfs_info;
          _ptr_rdfs_info = _rdfs_heap.pop ();
        }

        return _ptr_rdfs_info;
      }

    /*
     * Start of a new cycle of DFS. Put _node into stack,
     * paint it with GRAY, and visit all nodes reachable
     * from _node, i.e., execute DFS until _node is poped
     * from the stack. Add all visited nodes (including
     * _node) to the new SCC.
     */

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      void _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::_make_new_scc (_Node& _node)
      {
         // Put _node into the stack
        _dfs_state_stack.insert (_DFSState (_node));

        // Paint it with GRAY
        _discover_node (_node);

         // Add _node to the SCC
        _component_list.insert (_Component (_SCCNode (_node)));

        // Add _node to the new SCC
        _ptr_component = _component_list.back ();

        /*
         * Execute DFS until _node is poped from the stack.
         * Add all nodes reachable from _node to the new SCC
         */

        _Node* _ptr_node = NULL;

        while (! _dfs_state_stack.empty ())
        {
          _DFSState *_ptr_state  = _dfs_state_stack.top ();

          while (! _ptr_state->adj_finished ())
          {
            if (_has_color (_ptr_state->_adj_node (), _DFSInfo::WHITE))
            {
              _ptr_node = &(_ptr_state->_adj_node ());
              _ptr_state->adj_incr ();
              _dfs_state_stack.push (_DFSState (*_ptr_node));
              _discover_node (*_ptr_node);
              break;
            }
            else
              _ptr_state->adj_incr ();
          }

          if (_ptr_node)
          {
            _ptr_component->insert (_SCCNode (*_ptr_node));
            _ptr_node = NULL;
          }
          else
          {
            _DFSState *_ptr = _dfs_state_stack.pop ();
            _finish_node (_ptr->node ());
            delete _ptr;
          }
        }
      }

    template<typename _TpVertex, typename _TpEdge, template<typename> class _TpIterator>
      _SCCIterator<_TpVertex, _TpEdge, _TpIterator>& _SCCIterator<_TpVertex, _TpEdge, _TpIterator>::operator++()
      {
        _ptr_component = NULL;

        /*
         * Get the next node with greater finish time
         * and try to make the next SCC.
         */

        _SCC_DFSInfo* _ptr_rdfs_info = _get_node_for_new_scc ();

        if (_ptr_rdfs_info)
        {
          _make_new_scc (_ptr_rdfs_info->node ());
          delete _ptr_rdfs_info;
        }

        return *this;
      }
  }
}

#endif // __CGTL__CGT_STCONNCOMP_SCC_ITERATOR_H_
