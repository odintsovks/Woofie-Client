#include "glossarymodel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

GlossaryModel::GlossaryModel(QObject* parent)
  : QAbstractTableModel(parent)
{
}

int GlossaryModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_terms.size();
}

int GlossaryModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return 3;
}

QVariant GlossaryModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() >= m_terms.size())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    const GlossaryTerm& term = m_terms.at(index.row());

    switch (index.column()) {
    case 0: return term.translated;
    case 1: return term.original;
    case 2: return term.description;
    default: return QVariant();
    }
  }

  return QVariant();
}

QVariant GlossaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0: return "Переведённый термин";
    case 1: return "Изначальный термин";
    case 2: return "Описание";
    default: return QVariant();
    }
  }

  return QVariant();
}

void GlossaryModel::addTerm(const GlossaryTerm& term)
{
  beginInsertRows(QModelIndex(), m_terms.size(), m_terms.size());
  m_terms.append(term);
  endInsertRows();
}

void GlossaryModel::addTerm(const QString& translated, const QString& original, const QString& description)
{
  addTerm(GlossaryTerm(translated, original, description));
  emit dataUpdatedByUser();
  emit termAddedByUser(GlossaryTerm(translated, original, description));
}

void GlossaryModel::removeTerm(int index)
{
  if (index < 0 || index >= m_terms.size())
    return;

  beginRemoveRows(QModelIndex(), index, index);
  m_terms.removeAt(index);
  endRemoveRows();

  emit dataUpdatedByUser();
  emit termRemovedByUser(index);
}

void GlossaryModel::updateTerm(int index, const GlossaryTerm& term)
{
  if (index < 0 || index >= m_terms.size())
    return;

  m_terms[index] = term;
  emit dataChanged(this->index(index, 0), this->index(index, 2));
  emit dataUpdatedByUser();
}

void GlossaryModel::clear()
{
  beginResetModel();
  m_terms.clear();
  endResetModel();
}

GlossaryTerm GlossaryModel::getTerm(int index) const
{
  if (index >= 0 && index < m_terms.size())
    return m_terms.at(index);
  return GlossaryTerm();
}

QVector<GlossaryTerm> GlossaryModel::getAllTerms() const
{
  return m_terms;
}

bool GlossaryModel::loadFromFile(const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  beginResetModel();
  m_terms.clear();

  QTextStream in(&file);

  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList parts = line.split('\t');
    if (parts.size() >= 3) {
      m_terms.append(GlossaryTerm(parts[0], parts[1], parts[2]));
    }
  }

  file.close();
  endResetModel();
  return true;
}

bool GlossaryModel::saveToFile(const QString& filename) const
{
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QTextStream out(&file);

  for (const GlossaryTerm& term : m_terms) {
    out << term.translated << '\t'
      << term.original << '\t'
      << term.description << '\n';
  }

  file.close();
  return true;
}

QVector<GlossaryTerm> GlossaryModel::findTerms(const QString& searchText) const
{
  QVector<GlossaryTerm> result;
  QString searchLower = searchText.toLower();

  for (const GlossaryTerm& term : m_terms) {
    if (term.translated.toLower().contains(searchLower) ||
      term.original.toLower().contains(searchLower) ||
      term.description.toLower().contains(searchLower)) {
      result.append(term);
    }
  }

  return result;
}
