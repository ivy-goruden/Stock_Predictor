// gui.hpp
#ifndef GUI_HPP
#define GUI_HPP

#include <QMainWindow>
#include "../include/subscription.hpp"

namespace s21 {

class GraphControls;
class GraphDrawer;

class Gui : public QMainWindow, Subscription {
    Q_OBJECT
    public:
        explicit Gui(QWidget *parent = nullptr);

    private:
        void initWidgets();
};
} // namespace s21

#endif // GUI_HPP