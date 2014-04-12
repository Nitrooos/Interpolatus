/*
 * AddNodesWindow.hpp
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

#ifndef ADDNODESWINDOW_H
#define ADDNODESWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/builder.h>
#include <glibmm/refptr.h>

using namespace Gtk;
using namespace Glib;
using namespace std;

class AddNodesWindow: public Window {
    public:
        AddNodesWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder);
        virtual ~AddNodesWindow();

        Button & getConfirmButton() const;
        int getnNodes() const;
        long double getStartValue() const;
        long double getStep() const;
    private:
        void on_button_click();
    
        RefPtr<Builder> builder;
        Button *confirmButton{nullptr};
        Button *cancelButton{nullptr};
        SpinButton *nNodesSpin{nullptr};
        SpinButton *startValueSpin{nullptr};
        SpinButton *stepSpin{nullptr};
};

#endif /* ADDNODESWINDOW_H */