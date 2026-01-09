#ifndef APICONTROLLER_H
#define APICONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include "glossarymodel.h"
#include "translationunitframe.h"

class ApiController : public QObject
{
    Q_OBJECT
public:
    explicit ApiController(GlossaryModel *glossaryModel, TranslationUnitFrame *unitFrame, QObject *parent = nullptr);
private slots:
    void glossaryDataUpdatedByUser(const QModelIndex& lhs, const QModelIndex& rhs);
    void glossaryTermAddedByUser(const GlossaryTerm& term);
    void glossaryTermRemovedByUser(int index);
    void unitUpdatedByUser(int index, const TranslationUnitWidget* unit);
    void unitAddedByUser(const TranslationUnitWidget* unit);
    void unitRemovedByUser(int index);
    void fetchUpdates();
private:
    QNetworkAccessManager manager;
    QList<qint64> glossaryIndices;
    TranslationUnitFrame *unitFrame;
    QList<qint64> unitIndices;
    qint64 unitTimestamp;
    QTimer updateTimer;
};

#endif // APICONTROLLER_H
