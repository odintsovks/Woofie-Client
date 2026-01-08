#include "translationunitwidget.h"
#include "ui_translationunitwidget.h"
#include <QMouseEvent>

TranslationUnitWidget::TranslationUnitWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TranslationUnitWidget)
    , isBeingDragged(false)
    , draggingPosition()
{
    ui->setupUi(this);
    connect(ui->visibilityButton, &QPushButton::clicked, this, [this]{ this->ui->textLabel->setVisible(!this->ui->textLabel->isVisible()); });
}

TranslationUnitWidget::~TranslationUnitWidget()
{
    delete ui;
}

void TranslationUnitWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isBeingDragged = true;
        draggingPosition = event->pos();
    }
}

void TranslationUnitWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isBeingDragged) {
        move(mapToParent(event->pos() - draggingPosition));
    }
}

void TranslationUnitWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isBeingDragged = false;
    }
}
