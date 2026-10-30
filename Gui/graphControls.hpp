#include <QVector>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QDateEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QColor>

class QtColorPicker;

namespace s21 {
class GraphControls : public QWidget {
    Q_OBJECT
public:
    explicit GraphControls(QWidget *parent = nullptr, int maxGraphs = 0,
                           bool approximationMode = false);

signals:
    void loadRequested(QString fileName, int seriesId);
    void removeRequested(int seriesId);
    void visibilityRequested(int seriesId);
    void calculateRequested(int seriesId, QString date);
    void drawRequested(int dotsCount);
    void approximationDrawRequested(int dotsCount, int degree,
                                    int forecastDays);
    void colorSelected(int seriesId, const QColor &color);

public slots:
    void setCalculatedValue(int seriesId, float value);
    void changePointsRange(int minimum, int maximum);

private:
    struct Row {
        int seriesId;
        QWidget *widget;
        QPushButton *visibilityButton;
        QPushButton *calculateButton;
        QtColorPicker *colorPicker;
        QDateEdit *dateEdit;
        QLineEdit *valueEdit;
        bool visible = true;
    };

    void chooseFile();
    void createNewRow(const QString& fileName, int seriesId);
    void removeRow(int seriesId);
    void updateAddButton();

    int maxGraphs;
    QVBoxLayout *rowsLayout;
    QPushButton *addButton;
    QVector<Row> rows;
    QSpinBox *dotsCount;
    QSpinBox *polynomialDegree = nullptr;
    QSpinBox *forecastDays = nullptr;
    bool approximationMode;
};
} // namespace s21