/*
 * FileDialog.hpp
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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>

using namespace Gtk;
using namespace Glib;

class FileDialog : public FileChooserDialog {
    public:
        FileDialog(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder);

        void setMode(bool mode);            // true - otwieranie pliku, false - zapisywanie bazy do pliku
    private:
        RefPtr<Builder> builder;
};

#endif /* FILEDIALOG_H */
