#ifndef TRANSLATIONUNITCONNECTION_H
#define TRANSLATIONUNITCONNECTION_H

#include <QObject>
#include <QGraphicsScene>
#include "translationunitwidget.h"

class TranslationUnitConnection : public QObject
{
    Q_OBJECT
public:
    TranslationUnitConnection(QGraphicsScene *scene, TranslationUnitWidget *lhs, TranslationUnitWidget *rhs);
public slots:
    void reset();
private:
    TranslationUnitWidget* lhs;
    TranslationUnitWidget* rhs;
    QGraphicsLineItem* line;
};

#endif // TRANSLATIONUNITCONNECTION_H
