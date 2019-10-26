﻿#pragma once

#include <string>

class MainWindow;
class ServerService;


class Controller
{

public:

    Controller(MainWindow* pMainWindow);

    std::string getServerVersion();
    std::string getLastClientVersion();
    bool isServerRunning();

    void start();

    void stop();

    ~Controller();

private:

    ServerService* pServerService;

    bool bServerStarted;
};