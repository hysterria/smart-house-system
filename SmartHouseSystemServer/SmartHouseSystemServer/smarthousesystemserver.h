#ifndef SMARTHOUSESERVER_H
#define SMARTHOUSESERVER_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QNetworkAccessManager>


class SmartHouseSystemServer : public QTcpServer {
    Q_OBJECT
public:
    explicit SmartHouseSystemServer(QObject *parent = nullptr);
    bool startServer(quint16 port);

private slots:
    void processRegistrationRequest(QTcpSocket *socket, const QJsonObject &request);
    void processLoginRequest(QTcpSocket *socket, const QJsonObject &request);

    void processAddRoomRequest(QTcpSocket *socket, const QJsonObject &request);
    void processRoomLoadRequest(QTcpSocket *socket, const QJsonObject &request);

    void processLoadDevicesRequest(QTcpSocket *socket, const QJsonObject &request);
    void processLoadScenariosRequest(QTcpSocket *socket, const QJsonObject &request);
    void processRoomDevicesRequest(QTcpSocket *socket, const QJsonObject &request);

    void processAddScenarioRequest(QTcpSocket *socket, const QJsonObject &request);
    void processAddDeviceRequest(QTcpSocket *socket, const QJsonObject &request);
    void processToggleDeviceRequest(QTcpSocket *socket, const QJsonObject &request);
private:
    QNetworkAccessManager *networkManager;
    void sendRequestToFlask(const QJsonObject &request, const QString &endpoint);
    void sendCreateContainerRequest(const QString &deviceName, const QString &deviceType,const QString &roomName);
    void toggleDevice(const QString &deviceName, const QString &roomName, bool state);
    void processControlDeviceRequest(QTcpSocket *socket, const QJsonObject &request);
    void processDeviceGroupsRequest(QTcpSocket *socket, const QJsonObject &request);
};
#endif // SMARTHOUSESERVER_H
