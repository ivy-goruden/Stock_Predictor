#include "graphControls.hpp"
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>
#include <QSizePolicy>
#include <QSpinBox>

namespace s21 {

GraphControls::GraphControls(QWidget *parent, int maxGraphs)
: QWidget(parent), maxGraphs(maxGraphs) {
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
	actionsLayout->addStretch();
	layout->addWidget(actions);
	connect(drawButton, &QPushButton::clicked, this, [this] {
		if (dotsCount->isEnabled()) emit drawRequested(dotsCount->value());
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
	rowLayout->addWidget(calculateButton);
	rowLayout->addWidget(dateEdit);
	rowLayout->addWidget(valueEdit);
	rowsLayout->addWidget(rowWidget);
	rows.push_back(Row{seriesId, rowWidget, visibilityButton, calculateButton,
			dateEdit, valueEdit, true});

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
	if (hasRange) {
		dotsCount->setRange(minimum, maximum);
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
