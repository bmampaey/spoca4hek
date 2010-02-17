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
 * \file cgt/shortpath/single/dijkstra/dijkstra_distance.h
 * \brief Contains definition of a structure that represents distance for Dijkstra algorithm.
 * \author Leandro Costa
 * \date 2009
 *
 * $LastChangedDate: 2009-12-01 01:01:04 +0100 (Tue, 01 Dec 2009) $
 * $LastChangedBy: leandro.costa $
 * $Revision: 90 $
 */

#ifndef __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_DISTANCE_H_
#define __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_DISTANCE_H_

#include <string>


namespace cgt
{
  namespace shortpath
  {
    namespace single
    {
      namespace dijkstra
      {
        /*!
         * \class _DijkstraDistance
         * \brief A class template used by Dijkstra algorithms that represents distance between vertices.
         * \author Leandro Costa
         * \date 2009
         * \todo Define constructors for all primitive types.
         *
         * The structure uses the edge value to represent distance, but since any type can
         * be used as an edge (numerical, strings, and even classes or structures), we need
         * a way to represent the infinite value. So, this class template encapsulates an
         * edge value and has a flag to indicate if the distance is equal to infinite. If
         * a user-defined class is used as edge, it is expected from this class to have a
         * constructor that receives no value.
         */

        template<typename _TpEdge>
          class _DijkstraDistance
          {
            public:
              _DijkstraDistance () : _inf (true) { }

            public:
              void _set_distance (const _TpEdge &_d) { _distance = _d; _inf = false; }
              const _TpEdge& distance () const { return _distance; }
              const bool &inf_distance () const { return _inf; }

            private:
              _TpEdge _distance;
              bool    _inf;
          };

        template<> _DijkstraDistance<char>::_DijkstraDistance ()            : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<unsigned char>::_DijkstraDistance ()   : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<short>::_DijkstraDistance ()           : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<unsigned short>::_DijkstraDistance ()  : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<int>::_DijkstraDistance ()             : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<unsigned int>::_DijkstraDistance ()    : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<long>::_DijkstraDistance ()            : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<unsigned long>::_DijkstraDistance ()   : _distance (0), _inf (true) { }
        template<> _DijkstraDistance<std::string>::_DijkstraDistance ()     : _distance (""),_inf (true) { }
      }
    }
  }
}

#endif // __CGTL__CGT_SHORTPATH_SINGLE_DIJKSTRA_DIJKSTRA_DISTANCE_H_
