#include "graphDrawer.hpp"
using namespace s21;


void GraphDrawer::newGraph(int seriesId, QVector<double> xData,
                           QVector<double> yData){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    if (graphIndices[seriesId] >= 0) {
        removeGraph(graphIndices[seriesId]);
        for (int &index : graphIndices) {
            if (index > graphIndices[seriesId]) --index;
        }
    }
    if (graphCount() >= MAX_GRAPHS) return;
    const int graphIndex = graphCount();
    addGraph();
    graphIndices[seriesId] = graphIndex;
    graph(graphIndex)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    graph(graphIndex)->setData(xData, yData);
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
    graph(graphIndex)->keyAxis()->setTicker(dateTicker);

    graph(graphIndex)->setName(QString("Graph %1").arg(seriesId + 1));
    rescaleAxes();
    replot();
}


void GraphDrawer::clear(){
    clearGraphs();
    graphIndices.fill(-1);
    replot();
}

void GraphDrawer::toggleVisibility(int seriesId){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    const int index = graphIndices[seriesId];
    if (index < 0 || index >= graphCount()) return;
    bool currentVis = graph(index)->visible();
    graph(index)->setVisible(!currentVis);
    replot();
}

void GraphDrawer::removeGraphAt(int seriesId){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    const int index = graphIndices[seriesId];
    if (index < 0 || index >= graphCount()) return;
    removeGraph(index);
    graphIndices[seriesId] = -1;
    for (int &graphIndex : graphIndices) {
        if (graphIndex > index) --graphIndex;
    }
    replot();
}