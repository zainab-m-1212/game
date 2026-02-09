#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "usermanager.h"
#include "mainwindow.h"

#include <QMessageBox>
#include "signupwindow.h"


LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::on_btnLogin_clicked()
{
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text();

    if (username.isEmpty() || password.isEmpty()) {

        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    UserManager userManager;
    QString errorMessage;

    if (userManager.login(username, password, errorMessage)) {
        QMessageBox::information(this, "Success", "Login successful");

        MainWindow *mw = new MainWindow();
        mw->show();
        this->hide();
    } else {
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

    QMessageBox::information(this, "Forgot Password",
                             "Password recovery will be implemented next.");
}

