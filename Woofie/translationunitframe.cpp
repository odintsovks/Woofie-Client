#include "translationunitframe.h"
#include "ui_translationunitframe.h"

TranslationUnitFrame::TranslationUnitFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::TranslationUnitFrame)
{
    ui->setupUi(this);
}

TranslationUnitFrame::~TranslationUnitFrame()
{
    delete ui;
}
