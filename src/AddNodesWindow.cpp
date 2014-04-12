#include "AddNodesWindow.hpp"

#include <gtkmm.h>
#include <glibmm.h>

#include <iostream>

AddNodesWindow::AddNodesWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject), builder(refBuilder)
{
    builder->get_widget("confirmButton", confirmButton);
    builder->get_widget("cancelButton", cancelButton);
    builder->get_widget("nNodesSpin", nNodesSpin);
    builder->get_widget("startValueSpin", startValueSpin);
    builder->get_widget("stepSpin", stepSpin);

    if (confirmButton && cancelButton) {
        confirmButton->signal_clicked().connect(
            sigc::mem_fun(*this, &AddNodesWindow::on_button_click));

        cancelButton->signal_clicked().connect(
            sigc::mem_fun(*this, &AddNodesWindow::on_button_click));
    }
}

AddNodesWindow::~AddNodesWindow() {
    delete stepSpin;
    delete startValueSpin;
    delete nNodesSpin;
    delete cancelButton;
    delete confirmButton;

    cout << "AddNodesWindow destructor end\n";
}

Button & AddNodesWindow::getConfirmButton() const {
    return *confirmButton;
}

int AddNodesWindow::getnNodes() const {
    return nNodesSpin->get_value();
}

long double AddNodesWindow::getStartValue() const {
    return startValueSpin->get_value();
}

long double AddNodesWindow::getStep() const {
    return stepSpin->get_value();
}

void AddNodesWindow::on_button_click() {
    hide();
}