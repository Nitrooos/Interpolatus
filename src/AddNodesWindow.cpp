#include "DataManager.hpp"
#include "AddNodesWindow.hpp"
#include "Glade.hpp"

AddNodesWindow::AddNodesWindow(BaseObjectType* cobject, const RefPtr<Builder>& refBuilder)
    : Window(cobject), builder(refBuilder)
{
    confirmButton      = dynamic_cast<Button *>     (Glade::loadWidget("confirmButton", builder));
    cancelButton       = dynamic_cast<Button *>     (Glade::loadWidget("cancelButton", builder));
    nNodesSpin         = dynamic_cast<SpinButton *> (Glade::loadWidget("nNodesSpin", builder));
    startValueSpin     = dynamic_cast<SpinButton *> (Glade::loadWidget("startValueSpin", builder));
    stepSpin           = dynamic_cast<SpinButton *> (Glade::loadWidget("stepSpin", builder));
    intervalWidthSpin  = dynamic_cast<SpinButton *> (Glade::loadWidget("intervalWidthSpin", builder));
    intervalWidthLabel = dynamic_cast<Label *>      (Glade::loadWidget("label2", builder));

    if (confirmButton && cancelButton) {
        confirmButton->signal_clicked().connect(
            sigc::bind<Button *>(sigc::mem_fun(*this, &AddNodesWindow::onClick), confirmButton));

        cancelButton->signal_clicked().connect(
            sigc::bind<Button *>(sigc::mem_fun(*this, &AddNodesWindow::onClick), cancelButton));
    }
}

AddNodesWindow::~AddNodesWindow() {
    delete intervalWidthLabel;
    delete intervalWidthSpin;
    delete stepSpin;
    delete startValueSpin;
    delete nNodesSpin;
    delete cancelButton;
    delete confirmButton;
}

void AddNodesWindow::setDataManager(DataManager *dm) {
    this->dataManager = dm;
}

void AddNodesWindow::setIntervalWidthSpinVisible(bool visibility) {
    intervalWidthSpin->set_visible(visibility);
    intervalWidthLabel->set_visible(visibility);
}

void AddNodesWindow::run() {
    auto arthm = dataManager->whatArthmetic();
    if (arthm == Arthmetic::FULL_INTERV)
        setIntervalWidthSpinVisible(true);
    else
        setIntervalWidthSpinVisible(false);

    this->show();
    this->present();
}

void AddNodesWindow::onClick(Button *btn) {
    if (btn == this->confirmButton) {
        // Przygotowanie informacji do wysłania
        Info::AddNodes info;
        info.nNodes        = nNodesSpin->get_value();
        info.startValue    = trunc(startValueSpin->get_value()*10)/(long double)10.0;
        info.step          = trunc(stepSpin->get_value()*10)/(long double)10.0;
        info.intervalWidth = trunc(intervalWidthSpin->get_value()*10)/(long double)10.0;
        dataManager->addRecords(info);
    }

    hide();
}
