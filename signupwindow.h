#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>

namespace Ui
{
class SignupWindow;
}

class SignupWindow : public QMainWindow
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    explicit SignupWindow(QWidget *parent = nullptr);
    ~SignupWindow();

private slots:
    void on_btnSignup_clicked();

    void on_btnBack_clicked();

private:
    Ui::SignupWindow *ui;
};

#endif // SIGNUPWINDOW_H
