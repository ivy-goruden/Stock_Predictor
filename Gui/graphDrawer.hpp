#include <QApplication>
#include <vector>
namespace s21{

    class graphDrawer{
        public:
            struct Graph{
                QVector<double> xData;
                QVector<double> yData;
            };
            void setGraph(Graph, int);
        private:
            std::vector<Graph> graphs;

    };
}