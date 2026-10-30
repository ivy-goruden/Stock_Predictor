// gui.hpp
#ifndef GUI_HPP
#define GUI_HPP

#include <QMainWindow>

namespace s21 {

class GraphControls;
class GraphDrawer;

class Gui : public QMainWindow {
    Q_OBJECT
    public:
        explicit Gui(QWidget *parent = nullptr);

    private:
        void initWidgets();
    signals:
        void strokeWidthChanged();
};
} // namespace s21

#endif // GUI_HPP