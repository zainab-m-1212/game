#include "signupwindow.h"
#include "ui_signupwindow.h"
#include "usermanager.h"
#include "loginwindow.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QKeyEvent>

SignupWindow::SignupWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SignupWindow)
{
    ui->setupUi(this);

    ui->btnSignup->setFocusPolicy(Qt::StrongFocus);
    ui->btnBack->setFocusPolicy(Qt::StrongFocus);

    ui->editName->installEventFilter(this);
    ui->editUsername->installEventFilter(this);
    ui->editPhone->installEventFilter(this);
    ui->editEmail->installEventFilter(this);
    ui->editPassword->installEventFilter(this);

    ui->btnSignup->installEventFilter(this);
    ui->btnBack->installEventFilter(this);

    connect(ui->editName,     &QLineEdit::returnPressed, ui->btnSignup, &QPushButton::click);
    connect(ui->editUsername, &QLineEdit::returnPressed, ui->btnSignup, &QPushButton::click);
    connect(ui->editPhone,    &QLineEdit::returnPressed, ui->btnSignup, &QPushButton::click);
    connect(ui->editEmail,    &QLineEdit::returnPressed, ui->btnSignup, &QPushButton::click);
    connect(ui->editPassword, &QLineEdit::returnPressed, ui->btnSignup, &QPushButton::click);

    ui->btnSignup->setDefault(true);
    ui->btnSignup->setAutoDefault(true);
    ui->btnBack->setAutoDefault(true);
}


SignupWindow::~SignupWindow()
{
    delete ui;
}

void SignupWindow::on_btnSignup_clicked()
{
    QString name = ui->editName->text().trimmed();
    QString username = ui->editUsername->text().trimmed();
    QString phone = ui->editPhone->text().trimmed();
    QString email = ui->editEmail->text().trimmed();
    QString password = ui->editPassword->text();

    if (name.isEmpty() || username.isEmpty() ||phone.isEmpty() || email.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    UserManager userManager;
    QString errorMessage;

    bool success = userManager.signup
    (
        username,
        password,
        name,
        phone,
        email,
        errorMessage
    );

    if (success)
    {
        QMessageBox::information(this, "Success", "Account created successfully.");

        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();

        this->close();
    }
    else
    {
        QMessageBox::critical(this, "Error", errorMessage);
    }
}

void SignupWindow::on_btnBack_clicked()
{
    LoginWindow *login = new LoginWindow();
    login->show();
    this->hide();
}

bool SignupWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent*>(event);

        if (ke->key() == Qt::Key_Down)
        {
            focusNextChild();
            return true;
        }
        if (ke->key() == Qt::Key_Up)
        {
            focusPreviousChild();
            return true;
        }

        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Space)
        {
            if (auto *btn = qobject_cast<QPushButton*>(focusWidget()))
            {
                btn->click();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}
