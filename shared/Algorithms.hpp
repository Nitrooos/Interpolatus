/*
 * Algorithms.hpp
 *
 * Copyright 2014 Bartosz Kostaniak <bartosz@inspiron>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 *
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <map>
#include <vector>

#include "../src/Types.hpp"

using namespace std;

template<typename T>
T lagrange(mymap const &data, T xx, int &st);

template<typename T>
T neville(mymap const &data, T xx, int &st);

template<typename T>
vector<T> calcFactors(mymap const &data, int &st);

#endif /* ALGORITHMS_H */
