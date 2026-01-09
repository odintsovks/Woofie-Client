#include "apicontroller.h"
#include "glossarymodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>

QByteArray awaitReply(QNetworkReply* reply, int timeout = 10000)
{
    if (!reply)
        return QByteArray();
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);
    reply->connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    reply->connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    loop.exec();
    return reply->readAll();
}

ApiController::ApiController(GlossaryModel *glossaryModel, TranslationUnitFrame *unitFrame, QObject *parent)
    : QObject{parent}
    , manager()
    , glossaryIndices()
    , unitFrame(unitFrame)
    , unitIndices()
    , unitTimestamp(0)
    , updateTimer()
{
    QUrl url("http://127.0.0.1:8080/glossary");
    QNetworkRequest request;
    request.setUrl(url);
    QByteArray result = awaitReply(manager.get(request));
    QJsonDocument jsonResult = QJsonDocument::fromJson(result);
    QJsonArray entries = jsonResult.object().value("entries").toArray();

    for (QJsonValue val : entries)
    {
        QJsonObject obj = val.toObject();
        glossaryModel->addTerm(obj.value("targetTerm").toString(), obj.value("sourceTerm").toString(), obj.value("definition").toString());
        glossaryIndices.append(obj.value("id").toInteger());
    }

    connect(glossaryModel, &GlossaryModel::termAddedByUser, this, &ApiController::glossaryTermAddedByUser);
    connect(glossaryModel, &GlossaryModel::termRemovedByUser, this, &ApiController::glossaryTermRemovedByUser);
    connect(glossaryModel, &GlossaryModel::dataChanged, this, &ApiController::glossaryDataUpdatedByUser);

    url.setUrl("http://127.0.0.1:8080/api/translations");
    request.setUrl(url);
    result = awaitReply(manager.get(request));
    jsonResult = QJsonDocument::fromJson(result);
    unitTimestamp = jsonResult.object().value("timestamp").toInteger();
    entries = jsonResult.object().value("translations").toArray();

    for (QJsonValue val : entries)
    {
        QJsonObject obj = val.toObject();
        unitFrame->addTranslationUnitFromStrings(obj.value("targetText").toString(), obj.value("sourceText").toString(), QPoint(obj.value("xposition").toInteger(), obj.value("yposition").toInteger()));
        unitIndices.append(obj.value("id").toInteger());
    }

    connect(unitFrame, &TranslationUnitFrame::addedTranslationUnitByUser, this, &ApiController::unitAddedByUser);
    connect(unitFrame, &TranslationUnitFrame::removedTranslationUnitByUser, this, &ApiController::unitRemovedByUser);
    connect(unitFrame, &TranslationUnitFrame::editedTranslationUnitByUser, this, &ApiController::unitUpdatedByUser);

    updateTimer.setInterval(1000);
    updateTimer.start();
    connect(&updateTimer, &QTimer::timeout, this, &ApiController::fetchUpdates);
}

void ApiController::glossaryDataUpdatedByUser(const QModelIndex& lhs, const QModelIndex& rhs)
{
    int index = lhs.row();
    QUrl url("http://127.0.0.1:8080/glossary/" + QString::number(glossaryIndices[index]));
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj.insert("sourceTerm", lhs.data().toString());
    obj.insert("targetTerm", lhs.siblingAtColumn(1).data().toString());
    obj.insert("definition", rhs.data().toString());
    manager.put(request, QJsonDocument(obj).toJson());
}
void ApiController::glossaryTermAddedByUser(const GlossaryTerm& term)
{
    qsizetype newIndex = glossaryIndices.size();
    glossaryIndices.append(-1);
    QUrl url("http://127.0.0.1:8080/glossary");
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj.insert("sourceTerm", term.original);
    obj.insert("targetTerm", term.translated);
    obj.insert("definition", term.description);
    QByteArray result = awaitReply(manager.post(request, QJsonDocument(obj).toJson()));
    QJsonDocument jsonResult = QJsonDocument::fromJson(result);
    glossaryIndices[newIndex] = jsonResult.object().value("id").toInteger();
}
void ApiController::glossaryTermRemovedByUser(int index)
{
    qint64 id = glossaryIndices[index];
    glossaryIndices.removeAt(index);
    QUrl url("http://127.0.0.1:8080/glossary/" + QString::number(id));
    QNetworkRequest request;
    request.setUrl(url);
    manager.deleteResource(request);
}
void ApiController::unitUpdatedByUser(int index, const TranslationUnitWidget* unit)
{
    QUrl url("http://127.0.0.1:8080/api/translations/" + QString::number(unitIndices[index]));
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj.insert("sourceText", unit->getSourceText());
    obj.insert("targetText", unit->getTargetText());
    obj.insert("xposition", unit->x());
    obj.insert("yposition", unit->y());
    manager.put(request, QJsonDocument(obj).toJson());
}
void ApiController::unitAddedByUser(const TranslationUnitWidget* unit)
{
    qsizetype newIndex = unitIndices.size();
    unitIndices.append(-1);
    QUrl url("http://127.0.0.1:8080/api/translations");
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj.insert("sourceText", unit->getSourceText());
    obj.insert("targetText", unit->getTargetText());
    obj.insert("xposition", unit->x());
    obj.insert("yposition", unit->y());
    QByteArray result = awaitReply(manager.post(request, QJsonDocument(obj).toJson()));
    QJsonDocument jsonResult = QJsonDocument::fromJson(result);
    unitIndices[newIndex] = jsonResult.object().value("id").toInteger();
}
void ApiController::unitRemovedByUser(int index)
{
    qint64 id = unitIndices[index];
    unitIndices.removeAt(index);
    QUrl url("http://127.0.0.1:8080/api/translations/" + QString::number(id));
    QNetworkRequest request;
    request.setUrl(url);
    manager.deleteResource(request);
}
void ApiController::fetchUpdates()
{
    QUrl url("http://127.0.0.1:8080/api/translations/fetch");
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj.insert("timestamp", unitTimestamp);
    QByteArray result = awaitReply(manager.post(request, QJsonDocument(obj).toJson()));
    QJsonDocument jsonResult = QJsonDocument::fromJson(result);
    QJsonArray entries = jsonResult.object().value("translations").toArray();
    if (!entries.empty())
        unitTimestamp = jsonResult.object().value("timestamp").toInteger();
    for (QJsonValue val : entries) {
        QJsonObject obj = val.toObject();
        qint64 id = obj.value("id").toInteger();
        qsizetype trueIndex = unitIndices.indexOf(id);
        if (trueIndex == -1) {
            unitFrame->addTranslationUnitFromStrings(obj.value("targetText").toString(), obj.value("sourceText").toString(), QPoint(obj.value("xposition").toInteger(), obj.value("yposition").toInteger()));
            unitIndices.append(id);
            continue;
        }
        TranslationUnitWidget* unit = unitFrame->getTranslationUnit(trueIndex);
        unit->setTargetText(obj.value("targetText").toString());
        unit->setSourceText(obj.value("sourceText").toString());
        unit->move(obj.value("xposition").toInteger(), obj.value("yposition").toInteger());
    }
}
