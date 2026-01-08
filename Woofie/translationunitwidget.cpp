#include "translationunitwidget.h"
#include "ui_translationunitwidget.h"
#include <QMenu>
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
    if (event->button() == Qt::LeftButton)
    {
        isBeingDragged = true;
        draggingPosition = event->pos();
    }
    else if (event->button() == Qt::MiddleButton)
    {
        attemptConnection(this);
    }
}

void TranslationUnitWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isBeingDragged)
    {
        QPoint newPosition = mapToParent(event->pos() - draggingPosition);
        move(newPosition);
        positionChanged(newPosition + QPoint(width() * 0.5, height() * 0.5));
    }
}

void TranslationUnitWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isBeingDragged = false;
    }
}
