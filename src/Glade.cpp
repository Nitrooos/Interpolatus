#include "Glade.hpp"

#include "Exceptions.hpp"

#include <glibmm/markup.h>
#include <glibmm/fileutils.h>

Widget *Glade::loadWidget(ustring name, RefPtr<Builder> const& builder) {
    Widget *ptr{nullptr};
    builder->get_widget(name, ptr);
    if (!ptr)
        throw LoadWidgetError(name);

    return ptr;
}

void Glade::loadFile(ustring name, RefPtr<Builder> const& builder) {
    try {
        builder->add_from_file(name);
    }
    catch(const FileError& ex) {
        throw AppInitError("FileError: " + ex.what());
    }
    catch(const MarkupError& ex) {
        throw AppInitError("MarkupError: " + ex.what());
    }
    catch(const BuilderError& ex) {
        throw AppInitError("BuilderError: " + ex.what());
    }
}
