#ifndef TRANSLATIONUNITFRAME_H
#define TRANSLATIONUNITFRAME_H

#include <QFrame>
#include <QGraphicsView>
#include "translationunitwidget.h"
#include "translationunitconnection.h"

namespace Ui {
class TranslationUnitFrame;
}

class TranslationUnitFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TranslationUnitFrame(QWidget *parent = nullptr);
    ~TranslationUnitFrame();

public slots:
    void addTranslationUnit();

private slots:
    void attemptConnection(TranslationUnitWidget* unit);

private:
    Ui::TranslationUnitFrame *ui;
    QGraphicsScene scene;
    std::vector<TranslationUnitWidget*> units;
    std::vector<TranslationUnitConnection*> connections;
    TranslationUnitWidget* connectingFromPtr;
};

#endif // TRANSLATIONUNITFRAME_H
