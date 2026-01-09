#ifndef TRANSLATIONUNITFRAME_H
#define TRANSLATIONUNITFRAME_H

#include <QFrame>
#include <QGraphicsView>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "translationunitwidget.h"
#include "translationunitconnection.h"

class AddUnitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUnitDialog(QWidget* parent = nullptr);

    QString getTargetText() const;
    QString getSourceText() const;

    void setTargetText(const QString& source);
    void setSourceText(const QString& target);

    void clearFields();

private:
    QLineEdit* m_targetEdit;
    QLineEdit* m_sourceEdit;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

namespace Ui {
class TranslationUnitFrame;
}

class TranslationUnitFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TranslationUnitFrame(QWidget *parent = nullptr);
    ~TranslationUnitFrame();
    void addTranslationUnitFromStrings(const QString& target, const QString& source, const QPoint& pos = QPoint(0, 0));
    TranslationUnitWidget* getTranslationUnit(int index);

public slots:
    void editTranslationUnit(TranslationUnitWidget* widget);
    void removeTranslationUnit(TranslationUnitWidget* widget);
    void addTranslationUnit();

signals:
    void addedTranslationUnitByUser(TranslationUnitWidget* unit);
    void editedTranslationUnitByUser(int index, TranslationUnitWidget* unit);
    void removedTranslationUnitByUser(int index);

private slots:
    void attemptConnection(TranslationUnitWidget* unit);

private:
    Ui::TranslationUnitFrame *ui;
    QGraphicsScene scene;
    QList<TranslationUnitWidget*> units;
    QList<TranslationUnitConnection*> connections;
    TranslationUnitWidget* connectingFromPtr;
    AddUnitDialog* addDialog;
};

#endif // TRANSLATIONUNITFRAME_H
