#include "graphControls.hpp"
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>
#include <QSizePolicy>
#include <QSpinBox>
#include <algorithm>
#include "../include/qtcolorpicker.h"

namespace s21 {

GraphControls::GraphControls(QWidget *parent, int maxGraphs,
							 bool approximationMode)
: QWidget(parent), maxGraphs(maxGraphs), approximationMode(approximationMode) {
	auto *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	rowsLayout = new QVBoxLayout;
	rowsLayout->setAlignment(Qt::AlignTop);
	rowsLayout->setSpacing(4);
	layout->addLayout(rowsLayout);

	addButton = new QPushButton("+", this);
	addButton->setToolTip(tr("Добавить граф"));
	addButton->setFixedSize(36, 32);
	connect(addButton, &QPushButton::clicked, this,
			&GraphControls::chooseFile);
	layout->addWidget(addButton, 0, Qt::AlignLeft);

	auto *actions = new QWidget(this);
	auto *actionsLayout = new QHBoxLayout(actions);
	actionsLayout->setContentsMargins(0, 0, 0, 0);
	auto *drawButton = new QPushButton(tr("Нарисовать"), actions);
	auto *dotsLabel = new QLabel(tr("Точек:"), actions);
	dotsCount = new QSpinBox(actions);
	dotsCount->setRange(0, 0);
	dotsCount->setEnabled(false);
	actionsLayout->addWidget(drawButton);
	actionsLayout->addWidget(dotsLabel);
	actionsLayout->addWidget(dotsCount);
	if (approximationMode) {
		polynomialDegree = new QSpinBox(actions);
		polynomialDegree->setPrefix(tr("Степень: "));
		polynomialDegree->setRange(0, 100);
		polynomialDegree->setValue(2);
		polynomialDegree->setToolTip(
				tr("Высокая степень может резко менять прогноз за пределами исходных данных"));
		polynomialDegree->setEnabled(false);
		forecastDays = new QSpinBox(actions);
		forecastDays->setPrefix(tr("Прогноз, дней: "));
		forecastDays->setRange(0, 3650);
		forecastDays->setValue(2);
		actionsLayout->addWidget(polynomialDegree);
		actionsLayout->addWidget(forecastDays);
	}
	actionsLayout->addStretch();
	layout->addWidget(actions);
	connect(drawButton, &QPushButton::clicked, this, [this] {
		if (!dotsCount->isEnabled()) return;
		if (this->approximationMode) {
			emit approximationDrawRequested(dotsCount->value(),
					polynomialDegree->value(), forecastDays->value());
		} else {
			emit drawRequested(dotsCount->value());
		}
	});
	updateAddButton();
}

void GraphControls::chooseFile() {
	if (rows.size() >= maxGraphs) return;

	const QString fileName = QFileDialog::getOpenFileName(
		this, tr("Выберите файл с данными"), QString(), tr("CSV-файлы (*.csv);;Все файлы (*)"));
	if (fileName.isEmpty()) return;

	int seriesId = 0;
	for (; seriesId < maxGraphs; ++seriesId) {
		bool occupied = false;
		for (const Row &row : rows) {
			if (row.seriesId == seriesId) {
				occupied = true;
				break;
			}
		}
		if (!occupied) break;
	}
	createNewRow(fileName, seriesId);
	emit loadRequested(fileName, seriesId);
}

void GraphControls::createNewRow(const QString& fileName, int seriesId) {
	auto *rowWidget = new QWidget(this);
	auto *rowLayout = new QHBoxLayout(rowWidget);
	rowLayout->setContentsMargins(0, 0, 0, 0);

	auto *removeButton = new QPushButton(QString::fromUtf8("×"), rowWidget);
	removeButton->setToolTip(tr("Удалить граф"));
	removeButton->setFixedSize(32, 30);
	auto *fileLabel = new QLabel(QFileInfo(fileName).fileName(), rowWidget);
	fileLabel->setToolTip(fileName);
	auto *visibilityButton = new QPushButton(QString::fromUtf8("◉"), rowWidget);
	visibilityButton->setToolTip(tr("Скрыть граф"));
	visibilityButton->setFixedSize(32, 30);
	auto *calculateButton = new QPushButton(tr("="), rowWidget);
	calculateButton->setToolTip(tr("Рассчитать значение для выбранной даты"));
	calculateButton->setFixedSize(36, 30);
	auto *colorPicker = new QtColorPicker(rowWidget);
	colorPicker->setStandardColors();
	const QVector<QColor> defaultColors = {Qt::red, Qt::blue, Qt::darkGreen,
			Qt::magenta, Qt::darkCyan};
	colorPicker->setCurrentColor(defaultColors[seriesId % defaultColors.size()]);
	colorPicker->setToolTip(tr("Выбрать цвет графика"));
	colorPicker->setFixedSize(76, 30);
	auto *dateEdit = new QDateEdit(QDate::currentDate(), rowWidget);
	dateEdit->setCalendarPopup(true);
	dateEdit->setDisplayFormat("dd.MM.yyyy");
	dateEdit->setFixedWidth(112);
	auto *valueEdit = new QLineEdit(rowWidget);
	valueEdit->setReadOnly(true);
	valueEdit->setPlaceholderText(tr("Результат"));
	valueEdit->setFixedWidth(120);
	rowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	rowLayout->addWidget(removeButton);
	rowLayout->addWidget(fileLabel, 1);
	rowLayout->addWidget(visibilityButton);
	rowLayout->addWidget(colorPicker);
	rowLayout->addWidget(calculateButton);
	rowLayout->addWidget(dateEdit);
	rowLayout->addWidget(valueEdit);
	rowsLayout->addWidget(rowWidget);
	rows.push_back(Row{seriesId, rowWidget, visibilityButton, calculateButton,
			colorPicker,
			dateEdit, valueEdit, true});
	emit colorSelected(seriesId, colorPicker->currentColor());

	connect(removeButton, &QPushButton::clicked, this,
			[this, seriesId] { removeRow(seriesId); });
	connect(visibilityButton, &QPushButton::clicked, this, [this, seriesId] {
		for (Row &row : rows) {
			if (row.seriesId != seriesId) continue;
			row.visible = !row.visible;
			row.visibilityButton->setText(row.visible ? QString::fromUtf8("◉")
													   : QString::fromUtf8("○"));
			row.visibilityButton->setToolTip(
				row.visible ? tr("Скрыть граф") : tr("Показать граф"));
			emit visibilityRequested(seriesId);
			break;
		}
	});
	connect(calculateButton, &QPushButton::clicked, this,
			[this, seriesId, dateEdit, valueEdit] {
		valueEdit->clear();
		emit calculateRequested(seriesId, dateEdit->date().toString("dd.MM.yyyy"));
	});
	connect(colorPicker, &QtColorPicker::colorChanged, this,
			[this, seriesId](const QColor &color) {
		emit colorSelected(seriesId, color);
	});

}

void GraphControls::setCalculatedValue(int seriesId, float value) {
	for (const Row &row : rows) {
		if (row.seriesId != seriesId) continue;
		row.valueEdit->setText(QString::number(value, 'g', 7));
		return;
	}
}

void GraphControls::changePointsRange(int minimum, int maximum) {
	const bool hasRange = minimum > 0 && maximum >= minimum;
	dotsCount->setEnabled(hasRange);
	if (approximationMode) {
		polynomialDegree->setEnabled(hasRange);
		polynomialDegree->setRange(0, hasRange ? minimum - 2 : 0);
		if (hasRange && polynomialDegree->value() == 0) {
			polynomialDegree->setValue(1);
		}
	}
	if (hasRange) {
		dotsCount->setRange(minimum, maximum);
		if (approximationMode) {
			dotsCount->setValue(std::min(minimum * 5, maximum));
		}
	} else {
		dotsCount->setRange(0, 0);
	}
}

void GraphControls::removeRow(int seriesId) {
	for (int index = 0; index < rows.size(); ++index) {
		if (rows[index].seriesId != seriesId) continue;
		QWidget *widget = rows[index].widget;
		rows.removeAt(index);
		rowsLayout->removeWidget(widget);
		widget->deleteLater();
		emit removeRequested(seriesId);
		updateAddButton();
		return;
	}
}

void GraphControls::updateAddButton() {
	addButton->setVisible(rows.size() < maxGraphs);
}

} // namespace s21
