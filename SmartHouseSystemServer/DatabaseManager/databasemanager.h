#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }

    bool openConnection();
    QSqlDatabase getDatabase();
    void initializeDatabaseFromScript(const QString &scriptPath);

    bool registerUser(const QString &username, const QString &password, const QString &role);
    bool authenticateUser(const QString &username, const QString &password);
    bool userExists(const QString &username);
    bool adminExists();
    QString getUserRole(const QString &username);

    bool addRoom(const QString &roomName);
    bool addDevice(const QString &roomName, const QString &deviceType, QString &generatedDeviceName);
    bool addScenario(const QString &scenario);

    QMap<QString, QStringList> getAllDevices();
    QStringList getAllRooms();
    QStringList getAllScenarios();
    QStringList getDevicesForRoom(const QString &roomName);
    QMap<QString, QStringList> getDevicesGroupedByType();

private:
    DatabaseManager();
    ~DatabaseManager();
    QSqlDatabase db;
    DatabaseManager(DatabaseManager const&) = delete;
    void operator=(DatabaseManager const&) = delete;
};

#endif // DATABASEMANAGER_H
