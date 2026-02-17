#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>


class UserManager
{
public:
    static QString hashPassword(const QString &password);

    bool login(const QString &username,const QString &password);

    bool signup(const QString &username,const QString &hashedPassword,const QString &name,const QString &phone,const QString &email, QString &errorMessage);

    bool login(const QString &username,const QString &password,QString &error);

    bool resetPassword(const QString &username,const QString &phone,const QString &newPassword,QString &error);

private:
    bool isValidPhone(const QString &phone);
    bool isValidEmail(const QString &email);
    bool userExists(const QString &username);
};

#endif // USERMANAGER_H
