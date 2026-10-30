#include "graphDrawer.hpp"
using namespace s21;


void GraphDrawer::newGraph(int seriesId, QVector<double> xData,
                           QVector<double> yData){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    if (sourceGraphIndices[seriesId] >= 0) {
        removePlotGraph(sourceGraphIndices[seriesId]);
    }
    if (graphIndices[seriesId] >= 0) {
        removePlotGraph(graphIndices[seriesId]);
    }
    int activeGraphs = 0;
    for (int index : graphIndices) {
        if (index >= 0) ++activeGraphs;
    }
    if (activeGraphs >= MAX_GRAPHS) return;
    const int graphIndex = graphCount();
    addGraph();
    graphIndices[seriesId] = graphIndex;
    sourceGraphIndices[seriesId] = -1;
    graph(graphIndex)->setPen(QPen(colors[seriesId]));
    graph(graphIndex)->setBrush(Qt::NoBrush);
    graph(graphIndex)->setData(xData, yData);
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
    graph(graphIndex)->keyAxis()->setTicker(dateTicker);

    graph(graphIndex)->setName(QString("Graph %1").arg(seriesId + 1));
    rescaleAxes();
    replot();
}

void GraphDrawer::setGraphColor(int seriesId, const QColor &color) {
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    colors[seriesId] = color;
    const int graphIndex = graphIndices[seriesId];
    if (graphIndex < 0 || graphIndex >= graphCount()) return;

    graph(graphIndex)->setPen(QPen(color));
    graph(graphIndex)->setBrush(Qt::NoBrush);
    replot();
}

void GraphDrawer::newApproximationGraph(int seriesId, QVector<double> xData,
                                        QVector<double> yData,
                                        QVector<double> sourceXData,
                                        QVector<double> sourceYData,
                                        int forecastDays) {
    if (approximationForecastDays >= 0 &&
        approximationForecastDays != forecastDays) {
        clear();
    }
    approximationForecastDays = forecastDays;
    newGraph(seriesId, std::move(xData), std::move(yData));
    if (seriesId < 0 || seriesId >= MAX_GRAPHS || graphIndices[seriesId] < 0) {
        return;
    }

    addGraph();
    sourceGraphIndices[seriesId] = graphCount() - 1;
    auto *sourceGraph = graph(sourceGraphIndices[seriesId]);
    sourceGraph->setData(sourceXData, sourceYData);
    sourceGraph->setLineStyle(QCPGraph::lsNone);
    sourceGraph->setPen(QPen(Qt::black));
    sourceGraph->setBrush(QBrush(Qt::black));
    sourceGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,
                                                  Qt::black, 9));
    sourceGraph->setName(QString("Исходные точки %1").arg(seriesId + 1));
    replot();
}

void GraphDrawer::removePlotGraph(int graphIndex) {
    if (graphIndex < 0 || graphIndex >= graphCount()) return;
    removeGraph(graphIndex);
    for (int &index : graphIndices) {
        if (index == graphIndex) index = -1;
        else if (index > graphIndex) --index;
    }
    for (int &index : sourceGraphIndices) {
        if (index == graphIndex) index = -1;
        else if (index > graphIndex) --index;
    }
}


void GraphDrawer::clear(){
    clearGraphs();
    graphIndices.fill(-1);
    sourceGraphIndices.fill(-1);
    approximationForecastDays = -1;
    replot();
}

void GraphDrawer::toggleVisibility(int seriesId){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    const int index = graphIndices[seriesId];
    if (index < 0 || index >= graphCount()) return;
    bool currentVis = graph(index)->visible();
    graph(index)->setVisible(!currentVis);
    const int sourceIndex = sourceGraphIndices[seriesId];
    if (sourceIndex >= 0 && sourceIndex < graphCount()) {
        graph(sourceIndex)->setVisible(!currentVis);
    }
    replot();
}

void GraphDrawer::removeGraphAt(int seriesId){
    if (seriesId < 0 || seriesId >= MAX_GRAPHS) return;
    const int sourceIndex = sourceGraphIndices[seriesId];
    if (sourceIndex >= 0) removePlotGraph(sourceIndex);
    const int index = graphIndices[seriesId];
    if (index >= 0) removePlotGraph(index);
    replot();
}