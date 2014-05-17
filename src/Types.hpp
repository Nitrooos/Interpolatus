/*
 * Types.hpp
 *
 * Copyright 2014 Bartosz Kostaniak <bartas@debian>
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

#ifndef TYPES_H
#define TYPES_H

#include "IntervalArithmetic.hpp"

enum class ColumnEdit : char { NODE, VALUE, NODE_LEFT, NODE_RIGHT, VALUE_LEFT, VALUE_RIGHT };
enum class Arthmetic  : char { FLOAT_POINT, HALF_INTERV, FULL_INTERV };
enum class Algorithm  : char { LAGRANGE, NEVILLE };

namespace Info {
    struct DataRecord {
        DataRecord()
            : node(0.0), value(0.0), nodeI(0), valueI(0) { }

        long double node, value;
        interval nodeI, valueI;
    };

    struct AddNodes {
        int nNodes;                         // ilość węzłów
        long double startValue, step;       // pierwszy węzeł, odstęp między węzłami
        long double intervalWidth;          // opcjonalnie szerokość przedziału (a. interwałowa 2)
    };

} // namespace Info

#endif /* TYPES_H */
