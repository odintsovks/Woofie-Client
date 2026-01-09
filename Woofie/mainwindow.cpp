#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(nullptr)
{
    ui->setupUi(this);
    controller = new ApiController(ui->glossary->model(), ui->frame);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete controller;
}

