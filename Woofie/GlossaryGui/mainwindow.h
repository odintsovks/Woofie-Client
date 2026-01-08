#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Предварительное объявление классов
class GlossaryWidget;
class GlossaryModel;
struct GlossaryTerm;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);

private slots:
  void onTermAdded(const GlossaryTerm& term);
  void saveGlossary();
  void loadGlossary();
  void showStats();

private:
  void setupMenu();

  GlossaryWidget* m_glossaryWidget;
  GlossaryModel* m_model;
};

#endif // MAINWINDOW_H
