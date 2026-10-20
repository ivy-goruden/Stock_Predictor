#include "gui.hpp"
#include <QVBoxLayout>
#include "graphDrawer.hpp"
#include "graphControls.hpp"
#include <nlohmann/json.hpp> // import the library
using json = nlohmann::json;

using namespace s21;

Gui::Gui(QWidget *parent) : QMainWindow(parent) {
    initWidgets();
}

void Gui::initWidgets() {
    // Центральный виджет + компоновка
    auto *central = new QWidget(this);
    auto *layout  = new QVBoxLayout(central);

    

    auto *controls = new GraphControls(central, 5);
    auto *drawer = new GraphDrawer(central, 5);
    layout->addWidget(controls);
    layout->addWidget(drawer);

    connect(controls, &GraphControls::drawRequested, drawer, &GraphDrawer::draw);

    connect(controls, &GraphControls::loadRequested, this, [this](const QString& filename){
        notify("onLoadFile", json({"filename": filename.toStdString()}));
    });

    setCentralWidget(central);
    resize(800, 600);
}