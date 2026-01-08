#include "translationunitframe.h"
#include "ui_translationunitframe.h"
#include <QGraphicsProxyWidget>
#include <QMouseEvent>
#include <QMenu>

TranslationUnitFrame::TranslationUnitFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::TranslationUnitFrame)
    , scene()
    , units()
    , connections()
    , connectingFromPtr(nullptr)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    connect(ui->graphicsView, &QWidget::customContextMenuRequested,
            this, [this](const QPoint &pos){
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Add Translation Unit", this);
        connect(&action1, &QAction::triggered, this, &TranslationUnitFrame::addTranslationUnit);
        contextMenu.addAction(&action1);
        contextMenu.exec(mapToGlobal(pos));
    });
}

TranslationUnitFrame::~TranslationUnitFrame()
{
    for (TranslationUnitWidget* unitPtr : units)
    {
        delete unitPtr;
    }
    for (TranslationUnitConnection* connectionPtr : connections)
    {
        delete connectionPtr;
    }
    delete ui;
}

void TranslationUnitFrame::attemptConnection(TranslationUnitWidget* unit)
{
    if (!connectingFromPtr)
    {
        connectingFromPtr = unit;
        return;
    }
    if (unit != connectingFromPtr)
    {
        TranslationUnitConnection* connectionPtr = new TranslationUnitConnection(&scene, connectingFromPtr, unit);
        connections.push_back(connectionPtr);
    }
    connectingFromPtr = nullptr;
}

void TranslationUnitFrame::addTranslationUnit()
{
    TranslationUnitWidget* unitPtr = new TranslationUnitWidget();
    units.push_back(unitPtr);
    connect(unitPtr, &TranslationUnitWidget::attemptConnection, this, &TranslationUnitFrame::attemptConnection);
    QGraphicsProxyWidget* proxy = scene.addWidget(unitPtr);
    proxy->setPos(ui->graphicsView->mapToScene(QCursor::pos()));
}
