/*
 * ModelColumns.hpp
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

#ifndef MODELCOLUMNS_H
#define MODELCOLUMNS_H

#include <gtkmm/treemodelcolumn.h>
#include <string>

using namespace Gtk;
using namespace Glib;
using namespace std;

class ModelColumnsFloat : public TreeModelColumnRecord {
    public:
        ModelColumnsFloat() {
            add(node);
            add(value);
        }

        TreeModelColumn<string> node, value;
};

class ModelColumnsInterval : public TreeModelColumnRecord {
    public:
        ModelColumnsInterval() {
            add(nodeLeftEnd);
            add(nodeRightEnd);
            add(valueLeftEnd);
            add(valueRightEnd);
        }

        TreeModelColumn<string>  nodeLeftEnd,  nodeRightEnd,
                                valueLeftEnd, valueRightEnd;
};

#endif /* MODELCOLUMNS_H */
