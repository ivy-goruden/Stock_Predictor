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
            void removeGraphAt(int seriesId);
            void toggleVisibility(int seriesId);
            GraphDrawer(QWidget *parent = nullptr, int maxGraphs = 0): QCustomPlot(parent){
                MAX_GRAPHS = maxGraphs;
                graphIndices.fill(-1, maxGraphs);
                setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
                xAxis->setLabel("x");
                yAxis->setLabel("y");
                legend->setVisible(true);
            };
        private:
            int MAX_GRAPHS;
            QVector<int> graphIndices;
            QVector<QColor> colors = {QColor{255, 195, 160, 20}, QColor{247, 187, 151, 20}, QColor{255, 153, 102, 20},QColor{255, 81, 47, 20},QColor{255, 175, 123, 20}};
        signals:
            void setGraphColor(int index, Color color);
            void setLineWidth(int width);

    };
}