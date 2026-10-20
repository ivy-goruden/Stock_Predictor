#include "graphDrawer.hpp"
using namespace s21;

void GraphDrawer::drawAll(){
    auto i = 0;
    xAxis->setRange(-1, 1);
    yAxis->setRange(0, 1);
    for (auto g: graphs){
        addGraph();
        graph(i)->setData(g.xData, g.yData);
        graph(i)->setName("y = x^2");
        i++;
    }
    replot();
}

void GraphDrawer::draw(int index){
    if (index < 0 || index >= static_cast<int>(graphs.size())) {
        return;
    }
    while (graphCount() <= index) {
        addGraph();
    }
    graph(index)->setData(graphs[index].xData, graphs[index].yData);
    graph(index)->setVisible(true);
    replot();
}


void GraphDrawer::clear(){
    for (int index = 0; index < graphs.size(); index++) {
        graph(index)->setVisible(false);
    }

    replot();
}