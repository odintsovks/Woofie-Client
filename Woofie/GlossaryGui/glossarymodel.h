#ifndef GLOSSARYMODEL_H
#define GLOSSARYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

struct GlossaryTerm {
  QString translated;
  QString original;
  QString description;

  GlossaryTerm() = default;
  GlossaryTerm(const QString& trans, const QString& orig, const QString& desc)
    : translated(trans), original(orig), description(desc) {}
};

class GlossaryModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit GlossaryModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  void addTerm(const GlossaryTerm& term);
  void addTerm(const QString& translated, const QString& original, const QString& description);
  void removeTerm(int index);
  void updateTerm(int index, const GlossaryTerm& term);
  void clear();
  GlossaryTerm getTerm(int index) const;
  QVector<GlossaryTerm> getAllTerms() const;

  bool loadFromFile(const QString& filename);
  bool saveToFile(const QString& filename) const;

  QVector<GlossaryTerm> findTerms(const QString& searchText) const;

signals:
  void dataUpdatedByUser();
  void termAddedByUser(const GlossaryTerm& term);
  void termRemovedByUser(int index);

private:
  QVector<GlossaryTerm> m_terms;
};

#endif // GLOSSARYMODEL_H
