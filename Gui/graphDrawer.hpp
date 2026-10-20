#include <QApplication>
#include <vector>
#include "../include/qcustomplot.h"
namespace s21{

    class GraphDrawer: public QCustomPlot{
        public:
            struct Graph{
                QVector<double> xData;
                QVector<double> yData;
            };
            void setGraph(Graph newGraph, int index){
                graphs[index] = newGraph;
            }
            void clear();
            void draw(int);
            void drawAll();
            GraphDrawer(QWidget *parent = nullptr, int maxGraphs = 0): QCustomPlot(parent){
                graphs = std::vector<Graph>(maxGraphs);
                setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
                xAxis->setLabel("x");
                yAxis->setLabel("y");
                legend->setVisible(true);

            };
        private:
            std::vector<Graph> graphs;

    };
}