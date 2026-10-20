#include <QApplication>
#include "Gui/gui.hpp"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    s21::Gui window;
    window.show();
    

    return app.exec();
}