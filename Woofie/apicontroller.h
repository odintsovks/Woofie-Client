#ifndef APICONTROLLER_H
#define APICONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "glossarymodel.h"

class ApiController : public QObject
{
    Q_OBJECT
public:
    explicit ApiController(GlossaryModel *glossaryModel, QObject *parent = nullptr);
private slots:
    void glossaryDataUpdatedByUser(const QModelIndex& lhs, const QModelIndex& rhs);
    void glossaryTermAddedByUser(const GlossaryTerm& term);
    void glossaryTermRemovedByUser(int index);
private:
    QNetworkAccessManager manager;
    QList<qint64> glossaryIndices;
};

#endif // APICONTROLLER_H
