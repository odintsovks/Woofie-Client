#include "translationunitframe.h"
#include "ui_translationunitframe.h"
#include <QGraphicsProxyWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QFormLayout>

AddUnitDialog::AddUnitDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Редактирование единицы");
    setModal(true);

    m_targetEdit = new QLineEdit(this);
    m_sourceEdit = new QLineEdit(this);

    m_okButton = new QPushButton("Добавить", this);
    m_cancelButton = new QPushButton("Отмена", this);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Переведённая единица:", m_targetEdit);
    formLayout->addRow("Изначальная единица:", m_sourceEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_targetEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_okButton->setEnabled(!text.isEmpty() && !m_sourceEdit->text().isEmpty());
    });

    connect(m_sourceEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_okButton->setEnabled(!text.isEmpty() && !m_targetEdit->text().isEmpty());
    });

    m_okButton->setEnabled(false);
}

QString AddUnitDialog::getTargetText() const
{
    return m_targetEdit->text();
}

QString AddUnitDialog::getSourceText() const
{
    return m_sourceEdit->text();
}

void AddUnitDialog::setTargetText(const QString& target)
{
    m_targetEdit->setText(target);
}

void AddUnitDialog::setSourceText(const QString& source)
{
    m_sourceEdit->setText(source);
}


void AddUnitDialog::clearFields()
{
    m_targetEdit->clear();
    m_sourceEdit->clear();
}

TranslationUnitFrame::TranslationUnitFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::TranslationUnitFrame)
    , scene()
    , units()
    , connections()
    , connectingFromPtr(nullptr)
    , addDialog(new AddUnitDialog(this))
{
    ui->setupUi(this);
    scene.setSceneRect(-1000, -1000, 2000, 2000);
    ui->graphicsView->setScene(&scene);
    connect(ui->graphicsView, &QWidget::customContextMenuRequested,
            this, [this](const QPoint &pos){
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Добавить единицу", this);
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

void TranslationUnitFrame::editTranslationUnit(TranslationUnitWidget* widget)
{
    addDialog->setTargetText(widget->getTargetText());
    addDialog->setSourceText(widget->getSourceText());
    if (addDialog->exec() != QDialog::Accepted) {
        return;
    }
    QString target = addDialog->getTargetText();
    QString source = addDialog->getSourceText();
    widget->setTargetText(target);
    widget->setSourceText(source);
    emit editedTranslationUnitByUser(units.indexOf(widget), widget);
}
void TranslationUnitFrame::removeTranslationUnit(TranslationUnitWidget* widget)
{
    int index = units.indexOf(widget);
    units.removeAt(index);
    widget->setVisible(false);
    emit removedTranslationUnitByUser(index);
}

void TranslationUnitFrame::addTranslationUnitFromStrings(const QString& target, const QString& source, const QPoint& pos)
{
    TranslationUnitWidget* unitPtr = new TranslationUnitWidget(target, source);
    units.push_back(unitPtr);
    connect(unitPtr, &TranslationUnitWidget::attemptConnection, this, &TranslationUnitFrame::attemptConnection);
    connect(unitPtr, &TranslationUnitWidget::positionSet, this, [this](TranslationUnitWidget* unit){emit editedTranslationUnitByUser(units.indexOf(unit), unit);});
    QGraphicsProxyWidget* proxy = scene.addWidget(unitPtr);
    connect(unitPtr, &TranslationUnitWidget::attemptEdit,
            this, [this](TranslationUnitWidget* widget){
                QMenu contextMenu(tr("Context menu"), this);

                QAction action1("Редактировать", this);
                connect(&action1, &QAction::triggered, this, [this, widget]{editTranslationUnit(widget);});
                contextMenu.addAction(&action1);
                QAction action2("Удалить", this);
                connect(&action2, &QAction::triggered, this, [this, widget]{removeTranslationUnit(widget);});
                contextMenu.addAction(&action2);
                contextMenu.exec(QCursor::pos());
            });
    proxy->setPos(pos.x(), pos.y());
}

void TranslationUnitFrame::addTranslationUnit()
{
    QPointF pos = ui->graphicsView->mapToScene(QCursor::pos());
    addDialog->clearFields();
    if (addDialog->exec() != QDialog::Accepted) {
        return;
    }
    QString target = addDialog->getTargetText();
    QString source = addDialog->getSourceText();
    addTranslationUnitFromStrings(target, source, QPoint(pos.x(), pos.y()));
    emit addedTranslationUnitByUser(units.back());
}
