#ifndef GLOSSARYWIDGET_H
#define GLOSSARYWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QTextEdit>
#include <QFormLayout>
#include "glossarymodel.h"

class AddTermDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddTermDialog(QWidget* parent = nullptr);

  QString getTranslatedTerm() const;
  QString getOriginalTerm() const;
  QString getDescription() const;

  void clearFields();

private:
  QLineEdit* m_translatedEdit;
  QLineEdit* m_originalEdit;
  QTextEdit* m_descriptionEdit;
  QPushButton* m_okButton;
  QPushButton* m_cancelButton;
};

class GlossaryWidget : public QWidget
{
  Q_OBJECT

public:
  explicit GlossaryWidget(QWidget* parent = nullptr);

  GlossaryModel* model() const { return m_model; }
  void setModel(GlossaryModel* model);

public slots:
  void addNewTerm();
  void removeSelectedTerm();
  void searchTerms(const QString& text);
  void showContextMenu(const QPoint& pos);

signals:
  void termAdded(const GlossaryTerm& term);
  void termRemoved(int index);

private:
  void setupUI();
  void setupConnections();

  GlossaryModel* m_model;
  QSortFilterProxyModel* m_proxyModel;
  QTableView* m_tableView;
  QLineEdit* m_searchEdit;
  QPushButton* m_addButton;
  QPushButton* m_removeButton;
  QPushButton* m_clearButton;
  AddTermDialog* m_addDialog;
};

#endif // GLOSSARYWIDGET_H
