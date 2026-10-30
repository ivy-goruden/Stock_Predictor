#include <QApplication>
#include <QVector>
#include <vector>
#include "../include/qcustomplot.h"
#include "gui_global.hpp"
namespace s21{

    class GraphDrawer: public QCustomPlot{
        public:
            void clear();
            void newGraph(int seriesId, QVector<double> xData,
                          QVector<double> yData);
            void newApproximationGraph(int seriesId, QVector<double> xData,
                                       QVector<double> yData,
                                       QVector<double> sourceXData,
                                       QVector<double> sourceYData,
                                       int forecastDays);
            void removeGraphAt(int seriesId);
            void toggleVisibility(int seriesId);
            void setGraphColor(int seriesId, const QColor &color);
            GraphDrawer(QWidget *parent = nullptr, int maxGraphs = 0): QCustomPlot(parent){
                MAX_GRAPHS = maxGraphs;
                graphIndices.fill(-1, maxGraphs);
                sourceGraphIndices.fill(-1, maxGraphs);
                colors.fill(Qt::black, maxGraphs);
                setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
                xAxis->setLabel("x");
                yAxis->setLabel("y");
                legend->setVisible(true);
            };
        private:
            int MAX_GRAPHS;
            QVector<int> graphIndices;
            QVector<int> sourceGraphIndices;
            QVector<QColor> colors;
            int approximationForecastDays = -1;
            void removePlotGraph(int graphIndex);
        signals:
            void setLineWidth(int width);

    };
}