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

ApiController::ApiController(GlossaryModel *glossaryModel, QObject *parent)
    : QObject{parent}
    , manager()
    , glossaryIndices()
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
    qDebug() << id;
    glossaryIndices.removeAt(index);
    QUrl url("http://127.0.0.1:8080/glossary/" + QString::number(id));
    QNetworkRequest request;
    request.setUrl(url);
    manager.deleteResource(request);
}
