#include "translationunitwidget.h"
#include "ui_translationunitwidget.h"
#include <QMenu>
#include <QMouseEvent>

TranslationUnitWidget::TranslationUnitWidget(const QString& target, const QString& source, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TranslationUnitWidget)
    , isBeingDragged(false)
    , draggingPosition()
{
    ui->setupUi(this);
    ui->targetLabel->setText(target);
    ui->sourceLabel->setText(source);
}

TranslationUnitWidget::~TranslationUnitWidget()
{
    delete ui;
}

QString TranslationUnitWidget::getSourceText() const
{
    return ui->sourceLabel->text();
}

QString TranslationUnitWidget::getTargetText() const
{
    return ui->targetLabel->text();
}

void TranslationUnitWidget::setSourceText(const QString& source)
{
    ui->sourceLabel->setText(source);
}

void TranslationUnitWidget::setTargetText(const QString& target)
{
    ui->targetLabel->setText(target);
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
        emit attemptConnection(this);
    }
    else if (event->button() == Qt::RightButton)
    {
        emit attemptEdit(this);
    }
}

void TranslationUnitWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isBeingDragged)
    {
        QPoint newPosition = mapToParent(event->pos() - draggingPosition);
        move(newPosition);
        emit positionChanged(newPosition + QPoint(width() * 0.5, height() * 0.5));
    }
}

void TranslationUnitWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isBeingDragged = false;
    }
}
