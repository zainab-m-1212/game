#include "usermanager.h"
#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QCoreApplication>

QString UserManager::hashPassword(const QString &password)
{
    return QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
}

    bool UserManager::isValidPhone(const QString &phone)
    {
        QRegularExpression re("^09\\d{9}$");
        return re.match(phone).hasMatch();
    }

    bool UserManager::isValidEmail(const QString &email)
    {
        QRegularExpression re("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        return re.match(email).hasMatch();
    }


    bool UserManager::userExists(const QString &username)
    {
        QString usersFile = QCoreApplication::applicationDirPath() + "/users.txt";
        QFile file(usersFile);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList parts = line.split("|");
            if (parts.size() >= 1 && parts[0] == username)
            {
                file.close();
                return true;
            }
    }

        file.close();
        return false;
    }


    bool UserManager::signup(const QString &username,const QString &password,const QString &name,const QString &phone,const QString &email,QString &error)
    {
        QString usersFile = QCoreApplication::applicationDirPath() + "/users.txt";
        QFile file(usersFile);

        if (username.isEmpty() || password.isEmpty() || name.isEmpty() || phone.isEmpty() || email.isEmpty())
        {
            error = "all fields must be filled";
            return false;
        }

        if (password.length() < 8)
        {
            error = "password must be at least 8 characters";
            return false;
        }

        if (!isValidPhone(phone))
        {
            error = "the phone number is invalid";
            return false;
        }

        if (!isValidEmail(email))
        {
            error = "email is invalid";
            return false;
        }

        if (userExists(username))
        {
            error = "username is already registered";
            return false;
        }

        if (!file.open(QIODevice::Append | QIODevice::Text))
        {
            error = "error opening file";
            return false;
        }

        QTextStream out(&file);
        out << username << "|"<< hashPassword(password) << "|"<< name << "|"<< phone << "|"<< email << "\n";

        file.close();
        return true;
    }

    bool UserManager::login(const QString &username,const QString &password,QString &error)
    {
        QString usersFile = QCoreApplication::applicationDirPath() + "/users.txt";
        QFile file(usersFile);



        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            error = "Cannot open users file.";
            return false;
        }

        QTextStream in(&file);
        QString hashed = hashPassword(password);

        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList parts = line.split("|");

            if (parts.size() >= 2 &&parts[0] == username &&parts[1] == hashed)
            {
                file.close();
                return true;
            }
        }

        file.close();
        error = "Invalid username or password.";
        return false;
    }

    bool UserManager::resetPassword(const QString &username,const QString &phone,const QString &newPassword,QString &error)
    {
        QString usersFile = QCoreApplication::applicationDirPath() + "/users.txt";
        QFile file(usersFile);


        if (newPassword.length() < 8)
        {
            error = "new password must be 8 characters";
            return false;
        }


        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            error = "user file not found";
            return false;
        }

        QStringList lines;
        QTextStream in(&file);

        bool found = false;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split("|");

            if (parts.size() == 5 &&parts[0] == username &&parts[3] == phone)
            {

                parts[1] = hashPassword(newPassword);
                line = parts.join("|");
                found = true;
            }

            lines << line;
        }

        file.close();

        if (!found)
        {
            error = "information incorrect";
            return false;
        }


        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            error = "error saving file";
            return false;
        }

        QTextStream out(&file);
        for (const QString &l : lines)
            out << l << "\n";

        file.close();
        return true;

    }
