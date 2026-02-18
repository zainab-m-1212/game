#include "mainwindow.h"
#include <QRandomGenerator>

#include <QApplication>
#include "loginwindow.h"
#include <QDateTime>

static void applyAppTheme(QApplication &app)
{
    const char *qss = R"QSS(
QWidget
{
    background-color: #1E1F2B;
    color: #EAEAF0;
    font-family: "Segoe UI";
}

QFrame#card
{
    background-color: #2A2C3F;
    border-radius: 18px;
}

QPushButton#btnLogin,
QPushButton#btnSignup
{
    background-color: #7C83FF;
    color: white;
    border-radius: 14px;
    padding: 10px;
}

QPushButton#btnLogin:hover,
QPushButton#btnSignup:hover
{
    background-color: #5F66FF;
}
)QSS";

    app.setStyleSheet(QString::fromUtf8(qss));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    applyAppTheme(a);

    LoginWindow login;
    login.show();

    return a.exec();
}
