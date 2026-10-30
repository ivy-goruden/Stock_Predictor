#include <QObject>
#include <QVector>
#include <QString>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <vector>
#include "../classes/CubicSplineInterpolator.hpp"
#include "../classes/PolynomialApproximator.hpp"
#include <cmath>
namespace s21{
class InterController : public QObject {
    Q_OBJECT

    private:
        struct parsedGraph{
            CubicSplineInterPolator::RawData data;
            long long dateOrigin;
            float dateRange;
            float maxValue; 
        };
        std::vector<parsedGraph> parsedGraphs;
        std::vector<std::unique_ptr<CubicSplineInterPolator>> interpolatedGraphs;
        std::vector<std::unique_ptr<PolynomialApproximator>> approximatedGraphs;
    public:
        explicit InterController(int maxGraphs, QObject *parent = nullptr)
                        : QObject(parent), parsedGraphs(maxGraphs), interpolatedGraphs(maxGraphs),
                            approximatedGraphs(maxGraphs) {
        };
        void parseFile(const QString& filename, int index){
            if(index < 0 || index >= static_cast<int>(parsedGraphs.size())) return;
            std::ifstream file(filename.toStdString());

            // confirm file opening
            if (!file.is_open()) {
                // print error message and return
                std::cerr << "Failed to open file: " << filename.toStdString() << std::endl;
                return;
            }
            CubicSplineInterPolator::RawData rd;
            std::string a, b;
            int row = 0;
            while (file >> a >> b) {
                ++row;
                float dateValue = 0.0f;
                if (!tryParseDate(a, dateValue)) {
                    std::cerr << "Invalid date at row " << row << ": " << a
                              << std::endl;
                    continue;
                }

                char *end = nullptr;
                errno = 0;
                const float value = std::strtof(b.c_str(), &end);
                if (end == b.c_str() || *end != '\0' || errno == ERANGE ||
                    !std::isfinite(value)) {
                    std::cerr << "Invalid float at row " << row << ": " << b
                              << std::endl;
                    continue;
                }

                rd.push_back(CubicSplineInterPolator::Point{dateValue, value});
            }
            if (rd.size() < 2) {
                emit errorOccurred(
                    QString("Нужно минимум 2 корректные точки; прочитано: %1")
                        .arg(rd.size()));
                return;
            }

            std::sort(rd.begin(), rd.end(),
                [](const Interpolator::Point& a, const Interpolator::Point& b) {
                    return a.x < b.x;
                });
            for (std::size_t i = 1; i < rd.size(); ++i) {
                if (rd[i].x == rd[i - 1].x) {
                    emit errorOccurred("В файле есть повторяющиеся даты");
                    return;
                }
            }

            parsedGraphs[index] = normalizeGraph(std::move(rd));
            interpolatedGraphs[index].reset();
            approximatedGraphs[index].reset();
            updatePointCountRange();
            // Close the file
            file.close();
        }

        void drawLoadedGraphs(int dotsCount) {
            for (int index = 0; index < static_cast<int>(parsedGraphs.size()); ++index) {
                if (!parsedGraphs[index].data.empty()) drawGraph(index, dotsCount);
            }
        }

        void drawGraph(int index, int dotsCount) {
            if (index < 0 || index >= static_cast<int>(parsedGraphs.size()) ||
                parsedGraphs[index].data.empty()) {
                return;
            }
            const auto& graph = parsedGraphs[index];
            if (graph.data.size() < 4) {
                emit errorOccurred(tr("Для кубического сплайна нужно минимум 4 точки"));
                return;
            }
            const int minimum = static_cast<int>(graph.data.size()) + 1;
            const int maximum = minimum * 100;
            if (dotsCount < minimum || dotsCount > maximum) {
                emit errorOccurred(QString("Для графика %1 количество точек должно быть от %2 до %3")
                                       .arg(index + 1).arg(minimum).arg(maximum));
                return;
            }

            try {
                auto inputData = graph.data;
                auto initialized = CubicSplineInterPolator::Init(inputData);
                if (!initialized.ok()) {
                    emit errorOccurred(QString::fromStdString(
                        initialized.status().message().data()));
                    return;
                }
                interpolatedGraphs[index] = std::make_unique<CubicSplineInterPolator>(
                    initialized.value());
                auto graphDataStatus = interpolatedGraphs[index]->getGraphData(dotsCount);
                if (!graphDataStatus.ok()){
                    emit errorOccurred(QString::fromStdString(
                        graphDataStatus.status().message().data()));
                    return;
                }
                const auto& graphData = graphDataStatus.value();
                QVector<double> xData;
                QVector<double> yData;
                xData.reserve(static_cast<int>(graphData.size()));
                yData.reserve(static_cast<int>(graphData.size()));
                for (const auto& point : graphData) {
                    xData.push_back(
                        (point.x * graph.dateRange + graph.dateOrigin) * 86400.0);
                    yData.push_back(point.y*graph.maxValue);
                }
                emit graphInterpolated(index, xData, yData);
            } catch (const std::exception& error) {
                emit errorOccurred(QString::fromStdString(error.what()));
            }
        }

        void drawLoadedApproximations(int dotsCount, int degree,
                                      int forecastDays) {
            for (int index = 0; index < static_cast<int>(parsedGraphs.size());
                 ++index) {
                if (!parsedGraphs[index].data.empty()) {
                    drawApproximatedGraph(index, dotsCount, degree, forecastDays);
                }
            }
        }

        void drawApproximatedGraph(int index, int dotsCount, int degree,
                                   int forecastDays) {
            if (index < 0 || index >= static_cast<int>(parsedGraphs.size()) ||
                parsedGraphs[index].data.empty()) {
                return;
            }
            const auto& graph = parsedGraphs[index];
            if (degree < 0 || degree >= static_cast<int>(graph.data.size())) {
                emit errorOccurred(
                    tr("Степень полинома должна быть от 0 до числа точек минус 1"));
                return;
            }
            if (forecastDays < 0) {
                emit errorOccurred(tr("Количество дней прогноза не может быть отрицательным"));
                return;
            }
            if (dotsCount < static_cast<int>(graph.data.size())) {
                emit errorOccurred(
                    tr("Количество точек графика не должно быть меньше исходных данных"));
                return;
            }

            auto initialized = PolynomialApproximator::Init(graph.data, degree);
            if (!initialized.ok()) {
                emit errorOccurred(QString::fromStdString(
                    initialized.status().message().data()));
                return;
            }

            const float maxX = 1.0f + forecastDays / graph.dateRange;
            auto graphDataStatus = initialized->getGraphData(dotsCount, maxX);
            if (!graphDataStatus.ok()) {
                emit errorOccurred(QString::fromStdString(
                    graphDataStatus.status().message().data()));
                return;
            }

            QVector<double> xData;
            QVector<double> yData;
            QVector<double> sourceXData;
            QVector<double> sourceYData;
            const auto& graphData = graphDataStatus.value();
            xData.reserve(static_cast<int>(graphData.size()));
            yData.reserve(static_cast<int>(graphData.size()));
            sourceXData.reserve(static_cast<int>(graph.data.size()));
            sourceYData.reserve(static_cast<int>(graph.data.size()));
            for (const auto& point : graphData) {
                xData.push_back(
                    (point.x * graph.dateRange + graph.dateOrigin) * 86400.0);
                yData.push_back(point.y * graph.maxValue);
            }
            for (const auto& point : graph.data) {
                sourceXData.push_back(
                    (point.x * graph.dateRange + graph.dateOrigin) * 86400.0);
                sourceYData.push_back(point.y * graph.maxValue);
            }

            approximatedGraphs[index] =
                std::make_unique<PolynomialApproximator>(
                    std::move(initialized.value()));
            emit graphApproximated(index, xData, yData, sourceXData, sourceYData,
                                   degree, forecastDays);
        }

        void removeGraph(int index) {
            if (index < 0 || index >= static_cast<int>(parsedGraphs.size())) return;
            parsedGraphs[index] = parsedGraph{};
            interpolatedGraphs[index].reset();
            approximatedGraphs[index].reset();
            updatePointCountRange();
        }

        void calculateGraphValue(int index, const QString& date) {
            if (index < 0 || index >= static_cast<int>(interpolatedGraphs.size()) ||
                !interpolatedGraphs[index]) {
                emit errorOccurred(tr("Сначала нажмите «Нарисовать», чтобы построить выбранный график"));
                return;
            }
            float dateValue = 0.0f;
            if (!tryParseDate(date.toStdString(), dateValue)) {
                emit errorOccurred(tr("Введите корректную дату в формате ДД.ММ.ГГГГ"));
                return;
            }
            const auto& graph = parsedGraphs[index];
            const float normalizedDate =
                (dateValue - graph.dateOrigin) / graph.dateRange;
            emit calculatedValue(index,
                interpolatedGraphs[index]->getValue(normalizedDate) * graph.maxValue);
        }

        void calculateApproximatedGraphValue(int index, const QString& date) {
            if (index < 0 || index >= static_cast<int>(approximatedGraphs.size()) ||
                !approximatedGraphs[index]) {
                emit errorOccurred(
                    tr("Сначала постройте аппроксимацию выбранного графика"));
                return;
            }
            float dateValue = 0.0f;
            if (!tryParseDate(date.toStdString(), dateValue)) {
                emit errorOccurred(tr("Введите корректную дату в формате ДД.ММ.ГГГГ"));
                return;
            }
            const auto& graph = parsedGraphs[index];
            const float normalizedDate =
                (dateValue - graph.dateOrigin) / graph.dateRange;
            emit approximationValueCalculated(
                index, approximatedGraphs[index]->getValue(normalizedDate) *
                           graph.maxValue);
        }

signals:
    void graphInterpolated(int index, QVector<double> xData,
                               QVector<double> yData);
    void graphApproximated(int index, QVector<double> xData,
                           QVector<double> yData, QVector<double> sourceXData,
                           QVector<double> sourceYData, int degree,
                           int forecastDays);
    void errorOccurred(QString message);
    void calculatedValue(int index, float value);
    void approximationValueCalculated(int index, float value);
    void pointCountRangeChanged(int minimum, int maximum);

    private:
        long long daysFromCivil(int y, unsigned m, unsigned d) {
            y -= m <= 2;
            const int era = (y >= 0 ? y : y - 399) / 400;
            const unsigned yoe = static_cast<unsigned>(y - era * 400);
            const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
            const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
            return era * 146097LL + static_cast<long long>(doe) - 719468;
        }

        // "DD.MM.YYYY HH:MM:SS" -> секунды с 1970-01-01 00:00:00 UTC
        float parseDateTime(const std::string& s) {
            int d  = std::stoi(s.substr(0, 2));
            int mo = std::stoi(s.substr(3, 2));
            int y  = std::stoi(s.substr(6, 4));

            int hh = 0, mm = 0, ss = 0;
            if (s.size() >= 19) {                 // есть " HH:MM:SS"
                hh = std::stoi(s.substr(11, 2));
                mm = std::stoi(s.substr(14, 2));
                ss = std::stoi(s.substr(17, 2));
            } else if (s.size() >= 16) {          // есть " HH:MM"
                hh = std::stoi(s.substr(11, 2));
                mm = std::stoi(s.substr(14, 2));
            }

            long long days = daysFromCivil(y, mo, d);
            return static_cast<float>(days) * 86400.0
                + hh * 3600.0 + mm * 60.0 + ss;
        }
        bool tryParseDate(const std::string& value, float& parsedDate) {
            if (value.size() != 10 ||
                (value[2] != '.' && value[2] != '-') ||
                value[5] != value[2]) {
                return false;
            }
            for (std::size_t i = 0; i < value.size(); ++i) {
                if (i != 2 && i != 5 && (value[i] < '0' || value[i] > '9')) {
                    return false;
                }
            }

            const int day = std::stoi(value.substr(0, 2));
            const int month = std::stoi(value.substr(3, 2));
            const int year = std::stoi(value.substr(6, 4));
            if (month < 1 || month > 12 || day < 1) {
                return false;
            }

            const bool leapYear = year % 4 == 0 &&
                                  (year % 100 != 0 || year % 400 == 0);
            const int daysInMonth[] = {31, leapYear ? 29 : 28, 31, 30, 31, 30,
                                       31, 31, 30, 31, 30, 31};
            if (day > daysInMonth[month - 1]) {
                return false;
            }

            parsedDate = static_cast<float>(daysFromCivil(year, month, day));
            return true;
        }
        void updatePointCountRange() {
            int minimum = 0;
            int maximum = std::numeric_limits<int>::max();
            for (const auto& graph : parsedGraphs) {
                if (graph.data.empty()) continue;
                const int graphMinimum = static_cast<int>(graph.data.size()) + 1;
                minimum = std::max(minimum, graphMinimum);
                maximum = std::min(maximum, graphMinimum * 100);
            }
            if (minimum == 0) {
                emit pointCountRangeChanged(0, 0);
                return;
            }
            if (maximum < minimum) {
                emit pointCountRangeChanged(0, 0);
                emit errorOccurred(tr("У загруженных графиков нет общего допустимого диапазона количества точек"));
                return;
            }
            emit pointCountRangeChanged(minimum, maximum);
        }
    parsedGraph normalizeGraph(CubicSplineInterPolator::RawData points){
        if (points.empty()) return parsedGraph{};
        std::sort(points.begin(), points.end(),
            [](const Interpolator::Point& a, const Interpolator::Point& b){ return a.x < b.x; });
        const float dateOrigin = points.front().x;
        const float dateRange = points.back().x - dateOrigin;
        float maxVal = 0;
        if (dateRange <= 0) return parsedGraph{};
        for (auto& point : points){
            point.x = (point.x - dateOrigin) / dateRange;
            if (std::abs(point.y) > maxVal) {maxVal = std::abs(point.y);}
        }
        if (maxVal != 0){
            for (auto& point : points){
                point.y /= maxVal;
            }
        }
        else{
            maxVal = 1;
        }
        return parsedGraph{
            points, static_cast<long long>(dateOrigin), dateRange, maxVal
        };
    }

};
}