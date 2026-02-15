#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    buildMainMenu();
    buildCheckersPage();
    buildOthelloPage();

    ui->stackedWidget->setCurrentWidget(ui->page_mainMenu);
}

void MainWindow::buildMainMenu()
{
    ui->stackedWidget->setCurrentWidget(ui->page_mainMenu);

    connect(ui->btn_checkers, &QPushButton::clicked, this, [this]() {ui->stackedWidget->setCurrentWidget(ui->page_checkers);});

    connect(ui->btn_othello, &QPushButton::clicked, this, [this]() {ui->stackedWidget->setCurrentWidget(ui->page_othello);ui->othelloStack->setCurrentWidget(ui->page_othelloSetup);});

    connect(ui->btn_editProfile, &QPushButton::clicked, this, [this]() {ui->stackedWidget->setCurrentWidget(ui->page_editProfile);});

    connect(ui->btn_logout, &QPushButton::clicked, this, [this]() {close();});
}

void MainWindow::buildCheckersPage()
{
    ui->table_history->setColumnCount(5);

    QStringList headers;
    headers << "Opponent" << "Date" << "Role" << "Result" << "Score";
    ui->table_history->setHorizontalHeaderLabels(headers);

    ui->table_history->horizontalHeader()->setStretchLastSection(true);
    ui->table_history->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_history->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->btn_backToMenu, &QPushButton::clicked, this, [this]() {ui->stackedWidget->setCurrentWidget(ui->page_mainMenu);});

    connect(ui->btn_startGame, &QPushButton::clicked, this, [this]() {
        if (ui->radio_host->isChecked())
        {
            qDebug() << "Host selected";
        }
        else if (ui->radio_guest->isChecked())
        {
            qDebug() << "Guest selected";
        }
        else
        {
            qDebug() << "No role selected";
        }
    });
}

void MainWindow::buildOthelloPage()
{
    if (othelloConnectionsMade) return;
    othelloConnectionsMade = true;

    ui->othelloStack->setCurrentWidget(ui->page_othelloSetup);

    connect(ui->btn_othello_back, &QPushButton::clicked, this, [this]() {ui->stackedWidget->setCurrentWidget(ui->page_mainMenu);ui->othelloStack->setCurrentWidget(ui->page_othelloSetup);});

    connect(ui->btn_othello_exitGame, &QPushButton::clicked, this, [this]() {ui->othelloStack->setCurrentWidget(ui->page_othelloSetup);});

    connect(ui->btn_othello_start, &QPushButton::clicked, this, [this]() {

        if (!ui->radio_othello_host->isChecked() && !ui->radio_othello_guest->isChecked())
        {
            QMessageBox::warning(this, "Role", "Please select Host or Guest.");
            return;
        }

        if (!othelloWidget)
        {
            othelloWidget = new ReversiGameWidget(ui->othelloGameContainer);
            ui->othelloGameContainerLayout->setContentsMargins(0,0,0,0);
            ui->othelloGameContainerLayout->addWidget(othelloWidget);
        }
        else
        {
            othelloWidget->resetGame();
        }

        ui->othelloStack->setCurrentWidget(ui->page_othelloGame);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
