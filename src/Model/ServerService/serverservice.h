﻿// This file is part of the Silent Server.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once


// STL
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <mutex>

// ============== Network ==============
// Sockets and stuff
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#elif __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#endif

#ifdef _WIN32
// Winsock 2
#pragma comment(lib,"Ws2_32.lib")
#endif

#if _WIN32
#define SSocket SOCKET
#elif __linux__
#define SSocket int
#endif

// Custom
#include "Model/userstruct.h"



class MainWindow;
class SettingsManager;
class LogManager;
class UDPPacket;



// should be shorter than MAX_VERSION_STRING_LENGTH
#define SERVER_VERSION           "3.0.0"
#define CLIENT_SUPPORTED_VERSION "3.3.0"



// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------



class ServerService
{

public:

    ServerService(MainWindow* pMainWindow, SettingsManager* pSettingsManager, LogManager* pLogManager);




    // Starting

        bool  startWinSock                ();
        void  startToListenForConnection  ();
        void  prepareForVoiceConnection   ();


    // Listen

        void  listenForMessage            (UserStruct* userToListen);
        void  listenForVoiceMessage       (UserStruct* userToListen);
        void  listenForNewTCPConnections  ();


    // Rooms

        void  moveRoom                    (const std::string &sRoomName, bool bMoveUp);
        void  deleteRoom                  (const std::string &sRoomName);
        void  createRoom                  (const std::string &sName, size_t iMaxUsers);
        void  changeRoomSettings          (const std::string& sOldName, const std::string& sNewName, size_t iMaxUsers);


    // Ping

        void  checkPing                   ();
        void  sendPingToAll               (UserStruct* pNewUser = nullptr);


    // Closing

        void  kickUser                    (SListItemUser* pUser);
        void  shutdownAllUsers            ();


    // GET functions

        std::string    getServerVersion          ();
        std::string    getLastClientVersion      ();
        unsigned short getPingNormalBelow        ();
        unsigned short getPingWarningBelow       ();


    // Other

        void  sendMessageToAll            (const std::string& sMessage);

private:

    // TCP

        void  processMessage                     (UserStruct* userToListen);
        void  checkRoomSettings                  (UserStruct* userToListen);
        void  userEntersRoom                     (UserStruct* userToListen, std::string sRoomName);


    // UDP related

        void catchUDPPackets                     ();
        void eraseUDPPacket                      ();
        void refreshWrongUDPPackets              ();


    // "Close" function

        void responseToFIN                       (UserStruct* userToClose, bool bUserLost = false);
        void sendFINtoUser                       (UserStruct* userToClose);
        void sendFINtoSocket                     (SSocket socketToClose);
        bool closeSocket                         (SSocket socket);


    // Other

        bool setSocketBlocking                   (SSocket socket, bool bBlocking);
        int  getLastError                        ();


    // --------------------------------


    MainWindow*              pMainWindow;
    SettingsManager*         pSettingsManager;
    LogManager*              pLogManager;


    // Users
    SSocket                  listenTCPSocket;
    SSocket                  UDPsocket;
    std::vector<UserStruct*> users;
    std::vector<UDPPacket*>  vUDPPackets;


    // Ping
    unsigned short           iPingNormalBelow;
    unsigned short           iPingWarningBelow;
    std::chrono::time_point<std::chrono::steady_clock> pingCheckSendTime;


    // For wrong or empty packets (not from our users)
    size_t                   iWrongOrEmptyPacketCount; // will refresh every 'INTERVAL_REFRESH_WRONG_PACKETS_SEC'
    std::mutex               mtxRefreshWrongPacketCount;


    std::mutex               mtxUDPPackets;
    std::mutex               mtxUsersDelete;
    std::mutex               mtxConnectDisconnect;
    std::mutex               mtxRooms;


    std::string              serverVersion;
    std::string              clientLastSupportedVersion;


    int                      iUsersConnectedCount;
    int                      iUsersConnectedToVOIP;


    bool                     bWinSockStarted;
    bool                     bTextListen;
    bool                     bVoiceListen;
};
