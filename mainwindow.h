#pragma once

#include <QMainWindow>
#include "reversigamewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void buildMainMenu();
    void buildCheckersPage();
    void buildOthelloPage();

private:
    Ui::MainWindow *ui;

    ReversiGameWidget *othelloWidget = nullptr;
    bool othelloConnectionsMade = false;
};
