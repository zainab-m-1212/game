#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "usermanager.h"
#include "mainwindow.h"

#include <QMessageBox>
#include "signupwindow.h"
#include <QInputDialog>
#include <QKeyEvent>


LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    ui->btnLogin->setFocusPolicy(Qt::StrongFocus);
    ui->btnForgot->setFocusPolicy(Qt::StrongFocus);
    ui->btnSignup->setFocusPolicy(Qt::StrongFocus);

    ui->editUsername->installEventFilter(this);
    ui->editPassword->installEventFilter(this);
    ui->btnLogin->installEventFilter(this);
    ui->btnForgot->installEventFilter(this);
    ui->btnSignup->installEventFilter(this);

    connect(ui->editUsername, &QLineEdit::returnPressed,ui->btnLogin, &QPushButton::click);
    connect(ui->editPassword, &QLineEdit::returnPressed,ui->btnLogin, &QPushButton::click);

    ui->btnLogin->setAutoDefault(true);
    ui->btnForgot->setAutoDefault(true);
    ui->btnSignup->setAutoDefault(true);

    ui->btnLogin->setDefault(true);
}



LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::on_btnLogin_clicked()
{
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    UserManager userManager;
    QString errorMessage;

    if (userManager.login(username, password, errorMessage))
    {
        QMessageBox::information(this, "Success", "Login successful");

        MainWindow *mw = new MainWindow(username);
        mw->show();
        this->hide();
    }
    else
    {
        QMessageBox::critical(this, "Login Failed", errorMessage);
    }
}



void LoginWindow::on_btnSignup_clicked()
{
    SignupWindow *signup = new SignupWindow();
    signup->show();
    this->hide();
}


void LoginWindow::on_btnForgot_clicked()
{

    UserManager um;

    bool ok = false;
    const QString username = QInputDialog::getText(this, "Reset Password", "Username:", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || username.isEmpty()) return;

    const QString phone = QInputDialog::getText(this, "Reset Password", "Phone number:", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || phone.isEmpty()) return;

    const QString newPass = QInputDialog::getText(this, "Reset Password", "New password (min 8 chars):", QLineEdit::Password, "", &ok);
    if (!ok || newPass.isEmpty()) return;

    QString err;
    if (um.resetPassword(username, phone, newPass, err))
        QMessageBox::information(this, "Reset Password", "Password updated successfully.");
    else
        QMessageBox::warning(this, "Reset Password", err);
}


bool LoginWindow::eventFilter(QObject *obj, QEvent *event)
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

