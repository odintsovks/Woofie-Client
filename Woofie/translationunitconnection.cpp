#include "translationunitconnection.h"
#include <QGraphicsLineItem>

TranslationUnitConnection::TranslationUnitConnection(QGraphicsScene* scene, TranslationUnitWidget* lhs, TranslationUnitWidget* rhs)
    : lhs(lhs)
    , rhs(rhs)
    , line(scene->addLine(lhs->x() + lhs->width() * 0.5, lhs->y() + lhs->height() * 0.5, rhs->x() + rhs->width() * 0.5, rhs->y() + rhs->height() * 0.5))
{
    connect(lhs, &TranslationUnitWidget::positionChanged, this, &TranslationUnitConnection::reset);
    connect(rhs, &TranslationUnitWidget::positionChanged, this, &TranslationUnitConnection::reset);
    line->setZValue(-1);
}

void TranslationUnitConnection::reset()
{
    line->setLine(lhs->x() + lhs->width() * 0.5, lhs->y() + lhs->height() * 0.5, rhs->x() + rhs->width() * 0.5, rhs->y() + rhs->height() * 0.5);
}
