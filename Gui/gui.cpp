#include "gui.hpp"
#include <QVBoxLayout>
#include "graphDrawer.hpp"
#include "graphControls.hpp"
#include "../Controllers/inter_controller.hpp"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>

using namespace s21;

Gui::Gui(QWidget *parent) : QMainWindow(parent) {
    initWidgets();
}

void Gui::initWidgets() {
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *tabWidget = new QTabWidget(central);
    auto *interpolationPage = new QWidget(tabWidget);
    auto *interpolationLayout = new QVBoxLayout(interpolationPage);
    auto *approximationPage = new QWidget(tabWidget);
    auto *approximationLayout = new QVBoxLayout(approximationPage);
    tabWidget->addTab(interpolationPage, tr("Интерполяция"));
    tabWidget->addTab(approximationPage, tr("Аппроксимация"));

    auto *controls = new GraphControls(interpolationPage, 5);
    auto *drawer = new GraphDrawer(interpolationPage, 5);
    auto *controller = new InterController(5, this);
    auto *approximationControls =
        new GraphControls(approximationPage, 5, true);
    auto *approximationDrawer = new GraphDrawer(approximationPage, 5);
    auto *approximationController = new InterController(5, this);

    interpolationLayout->addWidget(controls, 0);
    interpolationLayout->addWidget(drawer, 1);
    approximationLayout->addWidget(approximationControls, 0);
    approximationLayout->addWidget(approximationDrawer, 1);
    layout->addWidget(tabWidget);

    connect(controls, &GraphControls::colorSelected,
        drawer, &GraphDrawer::setGraphColor);
    connect(controls, &GraphControls::loadRequested,
        controller, &InterController::parseFile);
    connect(controls, &GraphControls::visibilityRequested,
        drawer, &GraphDrawer::toggleVisibility);
    connect(controls, &GraphControls::removeRequested,
        controller, &InterController::removeGraph);
    connect(controls, &GraphControls::removeRequested,
        drawer, &GraphDrawer::removeGraphAt);
    connect(controls, &GraphControls::calculateRequested, this,
        [controller](int index, const QString& date) {
    controller->calculateGraphValue(index, date);
    });
    connect(controls, &GraphControls::drawRequested,
        controller, &InterController::drawLoadedGraphs);
    connect(controller, &InterController::graphInterpolated, drawer,
        [drawer](int index, const QVector<double>& xData,
             const QVector<double>& yData) {
    drawer->newGraph(index, xData, yData);
    });
    connect(controller, &InterController::pointCountRangeChanged,
        controls, &GraphControls::changePointsRange);
    connect(controller, &InterController::calculatedValue,
        controls, &GraphControls::setCalculatedValue);
    connect(controller, &InterController::errorOccurred, this,
            [this](const QString& message) {
        QMessageBox::warning(this, tr("Ошибка обработки данных"), message);
    });

    connect(approximationControls, &GraphControls::colorSelected,
        approximationDrawer, &GraphDrawer::setGraphColor);
    connect(approximationControls, &GraphControls::loadRequested,
        approximationController, &InterController::parseFile);
    connect(approximationControls, &GraphControls::visibilityRequested,
        approximationDrawer, &GraphDrawer::toggleVisibility);
    connect(approximationControls, &GraphControls::removeRequested,
        approximationController, &InterController::removeGraph);
    connect(approximationControls, &GraphControls::removeRequested,
        approximationDrawer, &GraphDrawer::removeGraphAt);
    connect(approximationControls, &GraphControls::calculateRequested,
        approximationController,
        &InterController::calculateApproximatedGraphValue);
    connect(approximationControls,
        &GraphControls::approximationDrawRequested,
        approximationController,
        &InterController::drawLoadedApproximations);
    connect(approximationController,
        &InterController::graphApproximated, approximationDrawer,
        [approximationDrawer](int index, const QVector<double>& xData,
                              const QVector<double>& yData,
                              const QVector<double>& sourceXData,
                              const QVector<double>& sourceYData, int,
                              int forecastDays) {
            approximationDrawer->newApproximationGraph(
                index, xData, yData, sourceXData, sourceYData, forecastDays);
        });
    connect(approximationController,
        &InterController::pointCountRangeChanged, approximationControls,
        &GraphControls::changePointsRange);
    connect(approximationController,
        &InterController::approximationValueCalculated,
        approximationControls, &GraphControls::setCalculatedValue);
    connect(approximationController, &InterController::errorOccurred, this,
            [this](const QString& message) {
        QMessageBox::warning(this, tr("Ошибка обработки данных"), message);
    });

    setCentralWidget(central);
    resize(800, 600);
}