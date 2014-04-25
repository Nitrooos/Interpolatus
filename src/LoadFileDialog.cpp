#include "LoadFileDialog.hpp"

#include <gtkmm.h>

LoadFileDialog::LoadFileDialog(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder)
    : FileChooserDialog(cobject), builder(refBuilder) {
    // Filtr plików (rozszerzenie .idf)
    RefPtr<FileFilter> idf;
    idf = RefPtr<FileFilter>::cast_static(builder->get_object("InterpolatusDataFile"));
    idf->set_name("Interpolatus Data File (*.idf)");
    add_filter(idf);
    set_filter(idf);

    // Dodaj przyciski Open i Cancel do dialogu
    add_button(Stock::CANCEL, RESPONSE_CANCEL);
    add_button(Stock::OPEN, RESPONSE_OK);

    hide();
}
