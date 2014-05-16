//// Zaznaczanie wiersza
auto refTreeSelection = treeView->get_selection();
refTreeSelection->select(row);

//// Walidacja świeżo wprowadzonych danych
//Convert the inputed text to an integer, as needed by our model column:
char *pchEnd = 0;
int new_value = strtol(new_text.c_str(), &pchEnd, 10);
if (new_value > 10) {
    //Prevent entry of numbers higher than 10.

    //Tell the user:
    Gtk::MessageDialog dialog(*this,
            "The number must be less than 10. Please try again.",
            false, Gtk::MESSAGE_ERROR);
    dialog.run();

    //Start editing again, with the bad text, so that the user can correct it.
    //A real application should probably allow the user to revert to the
    //previous text.

    //Set the text to be used in the start_editing signal handler:
    m_invalid_text_for_retry = new_text;
    m_validate_retry = true;

    //Start editing again:
    m_TreeView.set_cursor(path, m_treeviewcolumn_validated,
            m_cellrenderer_validated, true /* start_editing */);
}
else
{

}

//// Generowanie wiersza run-time
TreeModel::Row row = *(dataBase->append());
row.set_value(0, ustring("gtk-about"));
row.set_value(0, ustring("5.5"));
row.set_value(1, ustring("34.798"));
row.set_value(2, ustring("Row generated at run-time!"));

//// Dodanie nowego wiersza do bazy danych
TreeModel::Row row = *(dataBase->append());
row.set_value(0, ustring("0.0"));
row.set_value(1, ustring("0.0"));

treeView->set_cursor(TreePath{to_string(nRowsInBase)}, *treeView->get_column(0), true);
++nRowsInBase;