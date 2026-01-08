#include "mainwindow.h"
#include "glossarywidget.h"
#include "glossarymodel.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , m_model(new GlossaryModel(this))
  , m_glossaryWidget(new GlossaryWidget(this))
{
  setWindowTitle("Глоссарий терминов");
  setMinimumSize(800, 600);

  // Загрузка тестовых данных
  m_model->addTerm("Компилятор", "Compiler", "Программа, переводящая код с языка программирования в машинный код");
  m_model->addTerm("Переменная", "Variable", "Именованная область памяти для хранения данных");
  m_model->addTerm("Функция", "Function", "Блок кода, выполняющий определенную задачу");
  m_model->addTerm("Класс", "Class", "Шаблон для создания объектов в ООП");

  m_glossaryWidget->setModel(m_model);
  setCentralWidget(m_glossaryWidget);

  setupMenu();

  // Подключаем сигналы
  connect(m_glossaryWidget, &GlossaryWidget::termAdded, this, &MainWindow::onTermAdded);
}

void MainWindow::setupMenu()
{
  QMenu* fileMenu = menuBar()->addMenu("Файл");
  fileMenu->addAction("Загрузить", this, &MainWindow::loadGlossary);
  fileMenu->addAction("Сохранить", this, &MainWindow::saveGlossary);
  fileMenu->addSeparator();
  fileMenu->addAction("Выход", qApp, &QApplication::quit);

  QMenu* viewMenu = menuBar()->addMenu("Вид");
  viewMenu->addAction("Статистика", this, &MainWindow::showStats);

  QMenu* helpMenu = menuBar()->addMenu("Помощь");
  helpMenu->addAction("О программе", []() {
    QMessageBox::about(nullptr, "О программе",
    "Глоссарий терминов\n\n"
    "Программа для управления словарём терминов с переводом и описанием.");
    });
}

void MainWindow::onTermAdded(const GlossaryTerm& term)
{
  statusBar()->showMessage(QString("Добавлен новый термин: %1").arg(term.translated), 3000);
}

void MainWindow::saveGlossary()
{
  QString filename = QFileDialog::getSaveFileName(this, "Сохранить глоссарий",
    "", "Текстовые файлы (*.txt)");
  if (!filename.isEmpty()) {
    if (m_model->saveToFile(filename)) {
      statusBar()->showMessage("Глоссарий сохранен", 2000);
    }
    else {
      QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
    }
  }
}

void MainWindow::loadGlossary()
{
  QString filename = QFileDialog::getOpenFileName(this, "Загрузить глоссарий",
    "", "Текстовые файлы (*.txt)");
  if (!filename.isEmpty()) {
    if (m_model->loadFromFile(filename)) {
      statusBar()->showMessage("Глоссарий загружен", 2000);
    }
    else {
      QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
    }
  }
}

void MainWindow::showStats()
{
  int count = m_model->rowCount();
  QMessageBox::information(this, "Статистика",
    QString("Всего терминов: %1").arg(count));
}
