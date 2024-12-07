#include "mainwindow.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QJsonArray>
#include <QGraphicsDropShadowEffect>
#include <QStackedWidget>
#include <QFontDatabase>

#include <QMenu>
#include <QAction>
#include <QCoreApplication>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    initUI();

    qDebug() << "----IBUSKO---- MainWindow(QWidget *parent)";
    connect(
        &NetworkManager::instance(),
        &NetworkManager::responseReceived,
        this,
        &MainWindow::handleServerResponse);
    loadRoomsFromDatabase();
}
void MainWindow::setUserRole(const QString &role) {
    userRole = role;
    configureUIBasedOnRole();
}
void MainWindow::configureUIBasedOnRole() {
    bool isAdmin = (userRole == "admin");

    addRoomButton->setVisible(isAdmin);
    addDeviceButton->setVisible(isAdmin);
    addScenarioButton->setVisible(isAdmin);
    addScenarioButton->setVisible(isAdmin);
}
MainWindow::~MainWindow() {}
auto addShadowEffect = [](QPushButton* button) {
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setOffset(0, 3);
    shadowEffect->setColor(QColor(0, 0, 0, 127));
    shadowEffect->setBlurRadius(5);
    button->setGraphicsEffect(shadowEffect);
};
void MainWindow::initUI() {
    logoutButton = new QPushButton("Выйти", this);
    connect(logoutButton, &QPushButton::clicked, this, [this](){ emit backToMain(); });

    addRoomButton = new QPushButton("Добавить комнату", this);
    connect(addRoomButton, &QPushButton::clicked, this, &MainWindow::onAddRoomButtonClicked);

    addDeviceButton = new QPushButton("Добавить устройство", this);
    connect(addDeviceButton, &QPushButton::clicked, this, &MainWindow::onAddDeviceButtonClicked);

    scenarioButton = new QPushButton("Сценарии", this);
    connect(scenarioButton, &QPushButton::clicked, this, &MainWindow::onScenarioButtonClicked);

    addScenarioButton = new QPushButton("Добавить сценарий", this);
    connect(addScenarioButton, &QPushButton::clicked, this, &MainWindow::onAddScenarioButtonClicked);

    allDevicesButton = new QPushButton("Все устройства", this);
    connect(allDevicesButton, &QPushButton::clicked, this, &MainWindow::onAllDevicesButtonClicked);

    noticeButton = new QPushButton("Уведомления",this);
    connect(noticeButton, &QPushButton::clicked, this, &MainWindow::onNoticeButtonClicked);

    addDeviceButton->setFixedSize(200, 50);
    scenarioButton->setFixedSize(200, 60);
    allDevicesButton->setFixedSize(200, 60);
    addRoomButton->setFixedSize(200, 50);
    addScenarioButton->setFixedSize(200, 50);
    logoutButton->setFixedSize(100, 35);
    noticeButton->setFixedSize(50,50);

    sideMenu = new QWidget(this);
    sideMenuLayout = new QVBoxLayout(sideMenu);
    sideMenuLayout->addWidget(scenarioButton);
    sideMenuLayout->addWidget(allDevicesButton);
    sideMenuLayout->addStretch();
    sideMenuLayout->setSpacing(20);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    headerLayout->addWidget(addRoomButton);
    headerLayout->addWidget(addDeviceButton);
    headerLayout->addWidget(addScenarioButton);

    headerLayout->addStretch();
    headerLayout->addWidget(noticeButton);

    headerLayout->addWidget(logoutButton);


    displayWidget = new QStackedWidget(this);
    QWidget *defaultView = new QWidget(displayWidget);
    gridLayout = new QGridLayout(defaultView);
    gridLayout->setSpacing(18); // Установить расстояние между элементами

    displayWidget->addWidget(defaultView);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(sideMenu);
    mainLayout->addWidget(displayWidget);

    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->addLayout(headerLayout);
    windowLayout->addLayout(mainLayout);

    setLayout(windowLayout);
    setWindowTitle("Умный дом");
    resize(800, 600);

    currentRoom = QString();

    addShadowEffect(scenarioButton);
    addShadowEffect(allDevicesButton);
    addShadowEffect(addDeviceButton);
    addShadowEffect(addRoomButton);
    addShadowEffect(logoutButton);
    addShadowEffect(addScenarioButton);
    addShadowEffect(noticeButton);

    setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,  "
                  "stop: 0.5 #333464, stop: 1 #7a54a6);");

    QString buttonStyle = "QPushButton {"
                          "background-color: rgb(183, 148, 232,50);"
                          "border-radius: 25px;"
                          "padding: 10px;"
                          "color: #e7c9ef;"
                          "font: bold 16px 'New York';"
                          "}"
                          "QPushButton:hover {"
                          "background-color: rgb(114, 7, 168, 40);"
                          "}";
    scenarioButton->setObjectName("scenarioButton");
    scenarioButton->setStyleSheet(buttonStyle);
    allDevicesButton->setObjectName("allDevicesButton");
    allDevicesButton->setStyleSheet(buttonStyle);
    logoutButton -> setStyleSheet("QPushButton {""background-color: #7471c4; ""border-radius: 10px;""padding: 6px;""font: bold 20px  'New York';""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
    addRoomButton->setStyleSheet(buttonStyle);
    addDeviceButton ->setStyleSheet(buttonStyle);
    addScenarioButton->setStyleSheet(buttonStyle);
    noticeButton->setStyleSheet(buttonStyle);

}

void MainWindow::loadRoomsFromDatabase()
{
    QJsonObject request;
    request["action"] = "loadRooms";
    NetworkManager::instance().sendRequest(request);
}

void MainWindow::onNoticeButtonClicked()
{
    QString pathNoticeIcon = "C:\\Programming\\smart-house-system\\SmartHouseSystem\\images\\notice.png";
    QIcon noticeIcon(pathNoticeIcon);
    noticeButton->setIcon(noticeIcon);
    noticeButton->setIconSize(QSize(32,32));

    // Меню для отображения уведомлений
    QMenu *menu = new QMenu(noticeButton);

    // Пример добавления уведомлений
    menu->addAction("Уведомление 1");
    menu->addAction("Уведомление 2");
    menu->addAction("Уведомление 3");

    // Привязываем меню к кнопке
    noticeButton->setMenu(menu);
}

void MainWindow::onAllDevicesButtonClicked()
{
    QJsonObject request;
    request["action"] = "loadAllDevices";
    NetworkManager::instance().sendRequest(request);
}


void MainWindow::onAddRoomButtonClicked()
{
    QStringList predefinedRooms = {"Баня", "Подвал", "Ванная", "Гостиная", "Детская", "Гараж", "Сауна", "Кухня", "Чердак"};
    QString selectedRoom = QInputDialog::getItem(this, "Добавить помещение", "Выберите помещение:", predefinedRooms, 0, false);

    if (selectedRoom.isEmpty() || roomDevices.contains(selectedRoom)) {
        //QMessageBox::warning(this, "Ошибка", roomDevices.contains(selectedRoom) ? "Комната уже существует." : "Не выбрано.");
        QMessageBox msgBox;
        msgBox.setText(roomDevices.contains(selectedRoom) ? "Комната уже существует." : "Не выбрано.");
        msgBox.setWindowTitle("Ошибка");
        msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                             "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                             "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
        msgBox.exec();

        return;
    }

    QJsonObject request;
    request["action"] = "addRoom";
    request["roomName"] = selectedRoom;
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::requestRoomDevices(const QString &roomName) {
    QJsonObject request;
    request["action"] = "loadRoomDevices";
    request["room"] = roomName;
    NetworkManager::instance().sendRequest(request);

}
void MainWindow::onAddDeviceButtonClicked()
{
    if (currentRoom.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите комнату.");
        return;
    }

    QStringList predefinedDevices = {"лампа", "кондиционер", "обогреватель", "тёплый пол", "увлажнитель", "колонка", "замок", "кофемашина", "сигнализация", "робот-пылесос", "шторы", "стиральная машина"};
    QString selectedDevice = QInputDialog::getItem(this, "Добавить устройство", "Выберите устройство:", predefinedDevices, 0, false);

    if (selectedDevice.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не выбрано.");
        return;
    }

    QJsonObject request;
    request["action"] = "addDevice";
    request["roomName"] = currentRoom;
    request["deviceType"] = selectedDevice;
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::onScenarioButtonClicked() {
    QJsonObject request;
    request["action"] = "loadScenarios";
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::onAddScenarioButtonClicked(){
    // Создаем диалоговое окно для редактирования сценария
    QDialog *scenarioDialog = new QDialog(this);
    scenarioDialog->setWindowTitle("Создание сценария");
    scenarioDialog->resize(600, 400);

    QVBoxLayout *dialogLayout = new QVBoxLayout(scenarioDialog);

    // Список для выбора устройств
    QListWidget *availableDevices = new QListWidget(scenarioDialog);
    availableDevices->addItems({"Устройство 1", "Устройство 2", "Устройство 3", "Устройство 4"});
    availableDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    availableDevices->setDragEnabled(true);

    // Поле для выбора устройств в сценарии
    QListWidget *scenarioField = new QListWidget(scenarioDialog);
    scenarioField->setAcceptDrops(true);
    scenarioField->setDragDropMode(QAbstractItemView::DropOnly);

    // Расположение списков
    QHBoxLayout *listsLayout = new QHBoxLayout();
    QVBoxLayout *availableLayout = new QVBoxLayout();
    QVBoxLayout *scenarioLayout = new QVBoxLayout();

    availableLayout->addWidget(new QLabel("Доступные устройства:"));
    availableLayout->addWidget(availableDevices);

    scenarioLayout->addWidget(new QLabel("Поле сценария:"));
    scenarioLayout->addWidget(scenarioField);

    listsLayout->addLayout(availableLayout);
    listsLayout->addLayout(scenarioLayout);

    dialogLayout->addLayout(listsLayout);

    // Кнопки управления
    QPushButton *saveButton = new QPushButton("Сохранить", scenarioDialog);
    QPushButton *cancelButton = new QPushButton("Отмена", scenarioDialog);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(cancelButton);

    dialogLayout->addLayout(buttonsLayout);

    // Обработка нажатия кнопок
    connect(saveButton, &QPushButton::clicked, this, [this, scenarioField, availableDevices, scenarioDialog]() {
        QStringList scenarioDevices;
        for (int i = 0; i < scenarioField->count(); ++i) {
            scenarioDevices << scenarioField->item(i)->text();
        }

        if (scenarioDevices.isEmpty()) {
            QMessageBox::information(this, "Ошибка", "Сценарий не может быть пустым.");
            return;
        }

        // Получаем имя сценария с помощью диалога
        QString selectedScenario = QInputDialog::getText(this, "Добавить сценарий", "Введите имя сценария:");

        if (!selectedScenario.isEmpty()) {
            QJsonObject request;
            request["action"] = "addScenario";
            request["scenarioName"] = selectedScenario;
            NetworkManager::instance().sendRequest(request);
        } else {
            QMessageBox::information(this, "Информация", "Добавление сценария отменено.");
        }

        // Закрытие диалога после сохранения
        scenarioDialog->accept();
    });

    connect(cancelButton, &QPushButton::clicked, scenarioDialog, &QDialog::reject);

    // Открываем диалог
    scenarioDialog->exec();
}


void MainWindow::handleServerResponse(const QJsonObject &response)
{
    QString action = response["action"].toString();
    qDebug() << "-----IBUSKO----- Received response from server:" << action;

    if (action == "loadRooms") {
        handleLoadRoomsResponse(response);
    } else if (action == "loadAllDevices") {
        handleLoadAllDevicesResponse(response);
    } else if (action == "addRoom") {
        handleAddRoomResponse(response);
    } else if (action == "addDevice") {
        handleAddDeviceResponse(response);
    } else if (action=="loadScenarios"){
        handleLoadScenariosResponse(response);
    }else if(action=="addScenario"){
        handleAddScenarioResponse(response);
    }else if(action=="loadRoomDevices"){
        handleLoadRoomDevicesResponse(response);
    }else if (action == "toggleDevice") {
        handleToggleDeviceResponse(response);
    }
}
void MainWindow::handleToggleDeviceResponse(const QJsonObject &response)
{
    QString deviceName = response["deviceName"].toString();
    bool success = response["success"].toBool();
    QString message = response["message"].toString();

    /*if (success) {
        QMessageBox::information(this, "Устройство", "Устройство " + deviceName + " успешно обновлено.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось обновить состояние устройства: " + message);
    }*/
}
void MainWindow::handleLoadRoomsResponse(const QJsonObject &response) {
    QJsonArray roomsArray = response["rooms"].toArray();

    QLayoutItem *item;
    QList<QWidget*> widgetsToRemove;
    for (int i = 0; i < sideMenuLayout->count(); ++i) {
        item = sideMenuLayout->itemAt(i);
        QWidget *widget = item ? item->widget() : nullptr;
        if (widget && widget != scenarioButton && widget != allDevicesButton) {
            widgetsToRemove.append(widget);
        }
    }

    for (QWidget *widget : widgetsToRemove) {
        sideMenuLayout->removeWidget(widget);
        delete widget;
    }


    int fontId = QFontDatabase::addApplicationFont("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/Oswald/Oswald-VariableFont_wght.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    QString oswaldFont = fontFamilies.isEmpty() ? "Arial" : fontFamilies.at(0);


    for (const QJsonValue &value : roomsArray) {
        QString roomName = value.toString();

        QPushButton *roomButton = new QPushButton(roomName, this);
        roomButton->setFixedSize(200, 60);
        connect(roomButton, &QPushButton::clicked, this, [this, roomName]() {
            currentRoom = roomName;
            requestRoomDevices(roomName);
        });
        QString buttonStyle = "QPushButton {"
                              "background-color: #b3a2ee;"
                              "border-radius: 30px;"
                              "padding: 10px;"

                              "font: bold 16px 'New York';"
                              "}"
                              "QPushButton:hover {"
                              "background-color: rgb(114, 7, 168, 40);"
                              "}";

        roomButton->setObjectName(roomName);
        roomButton->setStyleSheet(buttonStyle);
        addShadowEffect(roomButton);
        sideMenuLayout->insertWidget(sideMenuLayout->count() - 1, roomButton);
    }


    sideMenu->update();
}



void MainWindow::handleLoadRoomDevicesResponse(const QJsonObject &response) {
    QString roomName = response["room"].toString();
    QJsonArray devicesArray = response["roomDevices"].toArray();
    QVector<QString> devices;
    if (currentRoom == roomName) {
        clearDisplay();

        for (const QJsonValue &deviceValue : devicesArray) {
            QString deviceName = deviceValue.toString();
            devices.push_back(deviceName);
            displayItemsInGrid(devices, roomName, true);
        }
    }

}
void MainWindow::handleLoadAllDevicesResponse(const QJsonObject &response) {
    QJsonArray devicesArray = response["devices"].toArray();

    QVector<QString> devices;
    for (const QJsonValue &value : devicesArray) {
        QJsonObject deviceObject = value.toObject();
        QString deviceType = deviceObject["type"].toString();
        QJsonArray roomsArray = deviceObject["rooms"].toArray();
        QStringList roomList;
        for (const QJsonValue &room : roomsArray) {
            roomList.append(room.toString());
        }

        QString deviceEntry = deviceType + ": " + roomList.join(", ");
        devices.push_back(deviceEntry);
    }
    displayAllDevicesInGrid(devices);
}

void MainWindow::handleAddRoomResponse(const QJsonObject &response) {
    bool status = response["success"].toBool();
    QString roomName = response["roomName"].toString();

    if (status) {

        QMessageBox::information(this, "Success", "Room added successfully: " + roomName);
    } else {
        QMessageBox::warning(this, "Error", "Failed to add room: " + response["message"].toString());
    }
    connect(
        &NetworkManager::instance(),
        &NetworkManager::responseReceived,
        this,
        &MainWindow::handleServerResponse);
    loadRoomsFromDatabase();
}

void MainWindow::handleAddDeviceResponse(const QJsonObject &response)
{
    bool status = response["success"].toBool();
    if (status) {
        QString deviceName = response["deviceName"].toString();
        QString roomName = response["roomName"].toString();
        if (roomDevices.contains(roomName)) {
            roomDevices[roomName].append(deviceName);
        } else {
            roomDevices[roomName] = QVector<QString>({deviceName});
        }

        QMessageBox::information(this, "Success", "Device added successfully: " + deviceName);
    } else {
        QMessageBox::warning(this, "Error", "Failed to add device: " + response["message"].toString());
    }
}
void MainWindow::handleAddScenarioResponse(const QJsonObject &response)
{
    QMessageBox::information(this, "Сценарий", response["message"].toString());
}

void MainWindow::handleLoadScenariosResponse(const QJsonObject &response) {
    QJsonArray scenariosArray = response["scenarios"].toArray();
    QVector<QString> scenarios;
    for (const QJsonValue &scenario : scenariosArray) {
        QString scenarioName = scenario.toString();
        scenarios.push_back(scenarioName);
    //displayItemsInGrid(scenarios, false);

    }
}





// У Саши было без const QString roomName
void MainWindow::displayItemsInGrid(const QVector<QString> &items, const QString roomName, bool isDevices)


{
    clearGridLayout(gridLayout);

    int row = 0, col = 0;
    for (const QString &item : items) {
        QPushButton *button = new QPushButton(this);
        auto addShadowEffect = [](QPushButton* button) {
            QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
            shadowEffect->setOffset(0, 3);
            shadowEffect->setColor(QColor(0, 0, 0, 157));
            shadowEffect->setBlurRadius(7);
            button->setGraphicsEffect(shadowEffect);
        };
        if (item == "лампа") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/lamp.png"));
            QSize iconSize(90, 90);
            button->setIconSize(iconSize);
            button->setText("");

        }
        if (item == "кондиционер") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/conditioner.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "обогреватель") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/heater.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "тёплый пол") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/floor.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "увлажнитель") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/humidifier.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "колонка") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/column.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "замок") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/lock.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "кофемашина") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/coffee.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "сигнализация") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/sirena.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }

        if (item == "робот-пылесос") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/robot.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "шторы") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/curtains.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "стиральная машина") {
            button->setFixedSize(170, 150);
            button->setStyleSheet("QPushButton {""background-color: rgb(191, 161, 249, 50);""border-radius: 25px;""}""QPushButton:hover {""background-color: rgb(114, 7, 168, 40);""}");
            addShadowEffect(button);
            button->setIcon(QIcon("/home/aleksandra/Desktop/MAIN_PROJECT/smart-house-system/images/washing.png"));
            QSize iconSize(70, 70);
            button->setIconSize(iconSize);
            button->setText("");
        }
        if (item == "Наступила ночь" || item == "Наступило утро" || item == "Похолодало") {
            button->setMinimumSize(200, 50);
            button->setStyleSheet("QPushButton {"

                                  "background-color: #b3a2ee; "
                                  "border-radius: 20px;"
                                  "padding: 15px;"
                                  "font: bold 14px  'New york';"
                                  "}");
            button->setText(item);
        }
        bool *isOff = new bool(true); // Используем динамическую память


        button->setCheckable(true);
        button->setObjectName(item);
        //button->setStyleSheet(buttonStyle);
        addShadowEffect(button);
        connect(button, &QPushButton::clicked, this, [this, button, item, roomName]() {
            QJsonObject request;
            request["action"] = "toggleDevice";
            request["deviceName"] = item;
            request["roomName"]=roomName;
            request["state"] = button->isChecked(); // true (включить) или false (выключить)
            button->setStyleSheet(button->isChecked() ? "background-color: #8fc98b;""border-radius: 25px;" : "background-color: #f9e2bd;""border-radius: 25px;");
            NetworkManager::instance().sendRequest(request);

        });
        if (item == "обогреватель" || item == "кондиционер" || item == "тёплый пол") {
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(button, &QPushButton::customContextMenuRequested, this, [this, button](const QPoint &pos) {
                QMenu contextMenu(tr("Контекстное меню"), this);

                QAction *action1 = new QAction("Выбрать температуру", this);
                connect(action1, &QAction::triggered, this, [this]() {
                    bool ok;
                    int temperature = QInputDialog::getInt(this, "Выбор температуры", "Введите температуру (°C):", 20, 0, 30, 1, &ok);
                    if (ok) {
                        QMessageBox msgBox;
                        msgBox.setText(QString("Вы выбрали: %1 °C").arg(temperature));
                        msgBox.setWindowTitle("Температура");
                        msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                                             "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                                             "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
                        msgBox.exec();

                    }
                });

                contextMenu.addAction(action1);
                contextMenu.exec(button->mapToGlobal(pos));
            });
        }
        if (item == "увлажнитель") {
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(button, &QPushButton::customContextMenuRequested, this, [this, button](const QPoint &pos) {
                QMenu contextMenu(tr("Контекстное меню"), this);

                QAction *action1 = new QAction("Выбрать влажность", this);
                connect(action1, &QAction::triggered, this, [this]() {
                    bool ok;
                    int temperature = QInputDialog::getInt(this, "Выбор влажности", "Введите влажность (°C):", 50, 0, 100, 5, &ok);
                    if (ok) {
                        QMessageBox msgBox;
                        msgBox.setText(QString("Вы выбрали: %1 °C").arg(temperature));
                        msgBox.setWindowTitle("Температура");
                        msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                                             "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                                             "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
                        msgBox.exec();

                    }
                });

                contextMenu.addAction(action1);
                contextMenu.exec(button->mapToGlobal(pos));
            });
        }

        gridLayout->addWidget(button, row, col);
        if (++col >= 3) {
            col = 0;
            ++row;
        }
    }
}
void MainWindow::displayAllDevicesInGrid(const QVector<QString> &items)
{
    clearGridLayout(gridLayout);

    int row = 0, col = 0;
    for (const QString &item : items) {
        QStringList parts = item.split(":");
        if (parts.size() < 2) {
            continue;
        }

        QString device = parts[0].trimmed();
        QStringList rooms = parts[1].split(",");
        for (const QString &room : rooms) {
            QString roomTrimmed = room.trimmed();
            QString buttonText = roomTrimmed + "\n" + device;

            QPushButton *button = new QPushButton(buttonText,this);
            button->setFixedSize(170, 140);
            button->setCheckable(true);

            QString buttonStyle = "QPushButton {"
                                  "background-color: rgb(191, 161, 249, 50);"
                                  "border-radius: 20px;"
                                  "padding: 10px;"
                                  "color: #e7c9ef;"

                                  "font: bold 18px 'New York';"

                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: rgb(114, 7, 168, 40);"
                                  "}";

            button->setObjectName(buttonText);
            button->setStyleSheet(buttonStyle);

            addShadowEffect(button);

  
//             bool *isOff = new bool(true);
//             connect(button, &QPushButton::clicked, this, [button, isOff]() {
//                 if (*isOff) {
//                     button->setStyleSheet("QPushButton { background-color: rgb(251, 117, 255, 100) ;""border-radius: 20px;""padding: 20px;""color: #e7c9ef;""font: bold 18px 'New York';}");
//                 } else {
//                     button->setStyleSheet("QPushButton { background-color: rgb(191, 161, 249, 50);""border-radius: 20px;""padding: 20px;""color: #e7c9ef;""font: bold 18px 'New York';}");
//                 }
//                 *isOff = !(*isOff); // Переключаем состояние


            connect(button, &QPushButton::clicked, this, [this, button, item, roomTrimmed]() {
                QJsonObject request;
                request["action"] = "toggleDevice";
                request["deviceName"] = item;
                request["roomName"] = roomTrimmed;
                qDebug() << "Device:" << item << ", Room:" << roomTrimmed;
                request["state"] = button->isChecked(); // true (включить) или false (выключить)
                button->setStyleSheet(button->isChecked() ? "background-color: #8fc98b;""border-radius: 20px;""padding: 20px;""font: bold 23px 'Oswald';}""color: #e7c9ef;" : "background-color: rgb(191, 161, 249, 50);""border-radius: 20px;""padding: 20px;""color: #e7c9ef;""font: bold 23px 'Oswald';");
                NetworkManager::instance().sendRequest(request);


            });

            if (device == "обогреватель" || device == "кондиционер" || device == "тёплый пол") {
                button->setContextMenuPolicy(Qt::CustomContextMenu);
                connect(button, &QPushButton::customContextMenuRequested, this, [this, button](const QPoint &pos) {
                    QMenu contextMenu(tr("Контекстное меню"), this);

                    QAction *action1 = new QAction("Выбрать температуру", this);
                    connect(action1, &QAction::triggered, this, [this]() {
                        bool ok;
                        int temperature = QInputDialog::getInt(this, "Выбор температуры", "Введите температуру (°C):", 20, 0, 30, 1, &ok);
                        if (ok) {
                            QMessageBox msgBox;
                            msgBox.setText(QString("Вы выбрали: %1 °C").arg(temperature));
                            msgBox.setWindowTitle("Температура");
                            msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                                                 "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                                                 "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
                            msgBox.exec();

                        }
                    });

                    contextMenu.addAction(action1);
                    contextMenu.exec(button->mapToGlobal(pos));
                });
            }
            if (device == "увлажнитель") {
                button->setContextMenuPolicy(Qt::CustomContextMenu);
                connect(button, &QPushButton::customContextMenuRequested, this, [this, button](const QPoint &pos) {
                    QMenu contextMenu(tr("Контекстное меню"), this);

                    QAction *action1 = new QAction("Выбрать влажность", this);
                    connect(action1, &QAction::triggered, this, [this]() {
                        bool ok;
                        int temperature = QInputDialog::getInt(this, "Выбор влажности", "Введите влажность (°C):", 50, 0, 100, 5, &ok);
                        if (ok) {
                            QMessageBox msgBox;
                            msgBox.setText(QString("Вы выбрали: %1 °C").arg(temperature));
                            msgBox.setWindowTitle("Температура");
                            msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                                                 "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                                                 "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
                            msgBox.exec();

                        }
                    });

                    contextMenu.addAction(action1);
                    contextMenu.exec(button->mapToGlobal(pos));
                });
            }

            gridLayout->addWidget(button, row, col);

            if (++col >= 3) {
                col = 0;
                ++row;
            }
        }
    }
}

void MainWindow::clearGridLayout(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
}
void MainWindow::clearDisplay()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

}
