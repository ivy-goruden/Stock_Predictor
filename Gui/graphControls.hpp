#include <QGridLayout>
#include <QPushButton>
#include <QWidget>
#include <string>
#include <QFileDialog>

namespace s21{
class GraphControls : public QWidget {
    Q_OBJECT
public:
    explicit GraphControls(QWidget* parent = nullptr, int maxGraphs = 0) : QWidget(parent) {
       auto* layout = new QGridLayout(this);

    auto* loadBtn  = new QPushButton(tr("Загрузить"), this);
    auto* clearBtn = new QPushButton(tr("Очистить"), this);

    layout->addWidget(loadBtn, 0, 0);   // строка 0, колонка 0

    for (int idx = 0; idx < maxGraphs; ++idx) {
        auto* btn = new QPushButton(tr("Нарисовать %1").arg(idx + 1), this);
        connect(btn, &QPushButton::clicked, this, [this, idx] {
            emit drawRequested(idx);
        });
        layout->addWidget(btn, idx, 1);   // строки 0..maxGraphs-1, колонка 1
        auto* loadBtn  = new QPushButton(tr("Загрузить").arg(idx +1), this);
        connect(loadBtn, &QPushButton::clicked, this, [this, idx] {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),".",tr("csv (*.csv)"));
    
            emit loadRequested(fileName);
        });
        layout->addWidget(loadBtn, idx, 0);   // строки 0..maxGraphs-1, колонка 1
        }

    layout->addWidget(clearBtn, maxGraphs, 0, 1, 2);   // последняя строка, на всю ширину
    }

signals:
    void loadRequested(QString fileName);
    void drawRequested(int seriesId);
    void clearRequested();
};
}