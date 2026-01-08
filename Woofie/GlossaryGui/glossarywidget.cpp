#include "glossarywidget.h"
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

AddTermDialog::AddTermDialog(QWidget* parent)
  : QDialog(parent)
{
  setWindowTitle("Добавить новый термин");
  setModal(true);

  m_translatedEdit = new QLineEdit(this);
  m_originalEdit = new QLineEdit(this);
  m_descriptionEdit = new QTextEdit(this);
  m_descriptionEdit->setMaximumHeight(100);

  m_okButton = new QPushButton("Добавить", this);
  m_cancelButton = new QPushButton("Отмена", this);

  QFormLayout* formLayout = new QFormLayout;
  formLayout->addRow("Переведённый термин:", m_translatedEdit);
  formLayout->addRow("Изначальный термин:", m_originalEdit);
  formLayout->addRow("Описание:", m_descriptionEdit);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_okButton);
  buttonLayout->addWidget(m_cancelButton);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(formLayout);
  mainLayout->addLayout(buttonLayout);

  connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  connect(m_translatedEdit, &QLineEdit::textChanged, [this](const QString& text) {
    m_okButton->setEnabled(!text.isEmpty() && !m_originalEdit->text().isEmpty());
    });

  connect(m_originalEdit, &QLineEdit::textChanged, [this](const QString& text) {
    m_okButton->setEnabled(!text.isEmpty() && !m_translatedEdit->text().isEmpty());
    });

  m_okButton->setEnabled(false);
}

QString AddTermDialog::getTranslatedTerm() const
{
  return m_translatedEdit->text().trimmed();
}

QString AddTermDialog::getOriginalTerm() const
{
  return m_originalEdit->text().trimmed();
}

QString AddTermDialog::getDescription() const
{
  return m_descriptionEdit->toPlainText().trimmed();
}

void AddTermDialog::clearFields()
{
  m_translatedEdit->clear();
  m_originalEdit->clear();
  m_descriptionEdit->clear();
}

GlossaryWidget::GlossaryWidget(QWidget* parent)
  : QWidget(parent)
  , m_model(new GlossaryModel(this))
  , m_proxyModel(new QSortFilterProxyModel(this))
  , m_addDialog(new AddTermDialog(this))
{
  setupUI();
  setupConnections();

  m_proxyModel->setSourceModel(m_model);
  m_tableView->setModel(m_proxyModel);
  m_tableView->setSortingEnabled(true);
  m_tableView->horizontalHeader()->setStretchLastSection(true);
  m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void GlossaryWidget::setupUI()
{
  m_tableView = new QTableView(this);

  m_searchEdit = new QLineEdit(this);
  m_searchEdit->setPlaceholderText("Поиск терминов...");

  m_addButton = new QPushButton("Добавить термин", this);
  m_removeButton = new QPushButton("Удалить выбранный", this);
  m_clearButton = new QPushButton("Очистить поиск", this);

  QHBoxLayout* searchLayout = new QHBoxLayout;
  searchLayout->addWidget(m_searchEdit);
  searchLayout->addWidget(m_clearButton);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(m_addButton);
  buttonLayout->addWidget(m_removeButton);
  buttonLayout->addStretch();

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(searchLayout);
  mainLayout->addWidget(m_tableView);
  mainLayout->addLayout(buttonLayout);
}

void GlossaryWidget::setupConnections()
{
  connect(m_addButton, &QPushButton::clicked, this, &GlossaryWidget::addNewTerm);
  connect(m_removeButton, &QPushButton::clicked, this, &GlossaryWidget::removeSelectedTerm);
  connect(m_searchEdit, &QLineEdit::textChanged, this, &GlossaryWidget::searchTerms);
  connect(m_clearButton, &QPushButton::clicked, m_searchEdit, &QLineEdit::clear);
  connect(m_tableView, &QTableView::customContextMenuRequested, this, &GlossaryWidget::showContextMenu);

  connect(m_model, &GlossaryModel::dataUpdatedByUser, this, [this]() {
    qDebug() << "Данные глоссария обновлены пользователем";
    });

  connect(m_model, &GlossaryModel::termAddedByUser, this, [this](const GlossaryTerm& term) {
    qDebug() << "Добавлен новый термин пользователем:" << term.translated;
  emit termAdded(term);
    });
}

void GlossaryWidget::setModel(GlossaryModel* model)
{
  if (m_model != model) {
    m_model = model;
    m_proxyModel->setSourceModel(m_model);
  }
}

void GlossaryWidget::addNewTerm()
{
  if (m_addDialog->exec() == QDialog::Accepted) {
    QString translated = m_addDialog->getTranslatedTerm();
    QString original = m_addDialog->getOriginalTerm();
    QString description = m_addDialog->getDescription();

    if (!translated.isEmpty() && !original.isEmpty()) {
      m_model->addTerm(translated, original, description);
      m_addDialog->clearFields();
    }
  }
}

void GlossaryWidget::removeSelectedTerm()
{
  QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
  if (selected.isEmpty()) {
    QMessageBox::warning(this, "Предупреждение", "Выберите термин для удаления");
    return;
  }

  if (QMessageBox::question(this, "Подтверждение",
    "Удалить выбранный термин?") == QMessageBox::Yes) {
    QList<int> rows;
    for (const QModelIndex& index : selected) {
      rows.append(m_proxyModel->mapToSource(index).row());
    }

    std::sort(rows.begin(), rows.end(), std::greater<int>());

    for (int row : rows) {
      m_model->removeTerm(row);
    }
  }
}

void GlossaryWidget::searchTerms(const QString& text)
{
  m_proxyModel->setFilterFixedString(text);
  m_proxyModel->setFilterKeyColumn(-1);
}

void GlossaryWidget::showContextMenu(const QPoint& pos)
{
  QModelIndex index = m_tableView->indexAt(pos);
  if (!index.isValid())
    return;

  QMenu menu(this);
  menu.addAction("Редактировать", [this, index]() {
    QMessageBox::information(this, "Информация",
    "Редактирование можно добавить при необходимости");
    });

  menu.addAction("Копировать термин", [this, index]() {
    QString text = m_proxyModel->data(index).toString();
  QApplication::clipboard()->setText(text);
    });

  menu.addSeparator();
  menu.addAction("Удалить", this, &GlossaryWidget::removeSelectedTerm);

  menu.exec(m_tableView->viewport()->mapToGlobal(pos));
}
