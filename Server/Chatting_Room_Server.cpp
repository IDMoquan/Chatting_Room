﻿#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <Windows.h>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

//int online_poeple = 0;        //当前在线人数
//int message_number = 0;        //缓冲区存有消息数量
bool status1 = true;        //是否有人正在连接
bool status2 = true;        //是否在发送信息
const int username_length = 1024;
const int password_length = 1024;
const int message_length = 1024;


// 存放输入数据
typedef struct {
    SOCKET socket;
    char* client_ip;
    char username[username_length];
    SOCKET c_socket;
} Data;

// 输入信息
typedef struct {
    SOCKET sender_socket;
    char* client_ip;
    string message;
    char* username;
} Messages;

vector<Data> clients;
queue<Messages> messages;        //发送信息缓冲

string utg(const string& utf8Str) {
    // 首先计算需要的宽字符串长度
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::vector<wchar_t> wideStr(wideLength);

    // 将UTF-8转换为宽字符串
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideLength);

    // 计算GBK字符串长度
    int gbkLength = WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> gbkStr(gbkLength);

    // 将宽字符串转换为GBK字符串
    WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, &gbkStr[0], gbkLength, nullptr, nullptr);

    return string(gbkStr.begin(), gbkStr.end() - 1); // 减去末尾的空字符
}

// 模拟数据库文件路径，可根据实际情况修改
const char* DATABASE_FILE = "users.txt";

// 检查用户名是否已存在于数据库中
bool userExists(const char* username) {
    FILE* file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        // 如果文件不存在，视为没有重复用户，返回false
        return false;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[username_length];
        int ret = sscanf(line, "%s", storedUsername);
        if (ret != 1) {
            printf("str Error!\n");
            return false;
        }
        storedUsername[strlen(storedUsername)] = '\0';
        if (strcmp(username, storedUsername) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

// 将新用户信息写入数据库文件
void writeUserToDatabase(const char* username, const char* password) {
    FILE* file = fopen(DATABASE_FILE, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", username, password);
        fclose(file);
    }
    else {
        // 可添加更完善的错误处理，比如打印错误信息等
        printf("无法打开数据库文件进行写入操作！\n");
    }
}

// 登录验证
string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        return "reject";
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[username_length];
        char storedPassword[password_length];
        int ret = sscanf(line, "%s %s", storedUsername, storedPassword);
        if (ret != 2) {
            printf("str Error!\n");
            return "reject";
        }
        storedUsername[strlen(storedUsername)] = '\0';
        storedPassword[strlen(storedPassword)] = '\0';
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            fclose(file);
            return "accept";
        }
    }

    fclose(file);
    return "reject";
}

// 注册验证
string check_data_regist(char* username) {
    if (userExists(username)) {
        return "reject";
    }
    return "accept";
}

// 移除客户端连接相关信息
void remove_client(SOCKET target_socket) {
    clients.erase(
        remove_if(clients.begin(), clients.end(),
            [&target_socket](const Data& s) {
                return target_socket == s.socket;
            }), clients.end()
                );
}

// 发送线程
DWORD WINAPI Send(LPVOID lpThreadParameter) {
    while (1) {
        //for (int i = 0; i < clients.size(); i++) {
        //    cout << clients[i].client_ip << " ";
        //}
        //cout << "end" << endl;
        //Sleep(1000);
        if (!messages.empty()) {
            //向非分发信息的客户端发送消息缓冲池的消息
            status1 = false;
            cout << "分发消息至:[";
            for (auto& clt : clients) {
                //如果是发送者，则跳过
                if (messages.front().sender_socket == clt.socket) {
                    continue;
                }
                string temp;
                temp.append(messages.front().username);
                temp.append(":");
                temp.append(messages.front().message);
                while (!status2);
                send(clt.socket, temp.c_str(), username_length + message_length + 1, 0);
                cout << clt.socket << " ";
                //cout << temp;
                //cout << "send" << endl;
            }
            cout << "]" << endl;
            status1 = true;
            messages.pop();
        }
    }
    return 0;
}

// 接受线程
DWORD WINAPI Receive(LPVOID lpThreadParameter) {
    //拆开结构体包装
    Data data = *(Data*)lpThreadParameter;
    SOCKET client_socket = data.socket;    //取出socket
    SOCKET c_client_socket = data.c_socket;
    char* client_ip = data.client_ip;        //取出client_ip
    char* input_username = (char*)malloc(username_length * sizeof(char));
    char* input_password = (char*)malloc(password_length * sizeof(char));
    char* status = (char*)malloc(256 * sizeof(char));
    delete (lpThreadParameter);                //释放内存
    if (status == nullptr) {
        puts("char*类型指针为空！！！");
        return -1;
    }
    int ret = recv(client_socket, status, 256, 0);
    if (ret <= 0) {
        free(input_username);
        free(input_password);
        free(status);
        printf("%s已断开！\n", utg(client_ip).c_str());
        closesocket(client_socket);
        return -1;
    }
    while (1) {
        //login登录
        if (!strcmp(status, "login")) {
            bool login_success = false;
            for (int i = 1; i <= 5; i++) {
                if (input_username == nullptr || input_password == nullptr) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    puts("char*类型指针为空！！！");
                    return -1;
                }
                ret = recv(client_socket, input_username, username_length, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s已断开！\n", utg(client_ip).c_str());
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (!strcmp(input_username, "regist")) {
                    strcpy(status, "regist");
                    break;
                }
                ret = recv(client_socket, input_password, password_length, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s已断开！\n", utg(client_ip).c_str());
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
                //从数据库检查用户信息
                string check = check_data_login(input_username, input_password);
                ret = send(client_socket, check.c_str(), 256, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s已断开！\n", utg(client_ip).c_str());
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (check == "accept") {
                    printf("%s登录成功！\n", utg(client_ip).c_str());
                    login_success = true;
                    strcpy(data.username, input_username);
                    char info[256];
                    //sprintf(info, "%zu", clients.size());
                    //send(c_client_socket, info, 256, 0);
                    free(input_username);
                    free(input_password);
                    free(status);
                    clients.push_back(data);
                    for (int i = 0; i < clients.size(); i++) {
                        char size[256] = { 0 };
                        sprintf(size, "%zu", clients.size());
                        send(clients[i].c_socket, size, 256, 0);
                    }
                    for (int i = 0; i < clients.size(); i++) {
                        for (int j = 0; j < clients.size(); j++) {
                            sprintf(info, "%s", clients[j].username);
                            send(clients[i].c_socket, info, username_length, 0);
                        }
                    }
                    break;
                }
            }
            if (login_success) {
                break;
            }
        }
        //注册
        else {
            if (input_username == nullptr || input_password == nullptr) {
                free(input_username);
                free(input_password);
                free(status);
                puts("char*类型指针为空！！！");
                return -1;
            }
            ret = recv(client_socket, input_username, username_length, 0);    //接收用户名
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s已断开！\n", utg(client_ip).c_str());
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            //接收到返回登录的消息
            if (!strcmp(input_username, "login")) {
                strcpy(status, "login");
                continue;
            }
            ret = recv(client_socket, input_password, password_length, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s已断开！\n", utg(client_ip).c_str());
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
            //从数据库检查用户信息
            string check = check_data_regist(input_username);
            ret = send(client_socket, check.c_str(), 256, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s已断开！\n", utg(client_ip).c_str());
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            if (check == "accept") {
                printf("%s注册成功！用户名：%s\n", utg(client_ip).c_str(), utg(input_username).c_str());
                writeUserToDatabase(input_username, input_password);
                recv(client_socket, input_username, username_length, 0);
                strcpy(status, "login");
            }
        }
    }
    status1 = true;
    cout << utg(data.username) << "登录！" << endl;
    while (true) {
        char buffer[message_length] = { 0 };
        //接收消息
        int ret = recv(client_socket, buffer, message_length, 0);
        if (ret <= 0) {
            break;
        }
        Messages temp;
        //memset(&temp, 0, sizeof(Messages));
        temp.client_ip = client_ip;
        temp.message = buffer;
        temp.sender_socket = client_socket;
        temp.username = data.username;
        cout << utg(temp.username) << "(" << temp.sender_socket << ")" << ":" << utg(temp.message) << endl;
        messages.push(temp);
        status2 = true;
        //while (!status1);
    }
    printf("%s已断开！\n", utg(client_ip).c_str());
    remove_client(client_socket);
    char info[256] = { 0 };
    for (int i = 0; i < clients.size(); i++) {
        char size[256];
        sprintf(size, "%zu", clients.size());
        send(clients[i].c_socket, size, 256, 0);
    }
    for (int i = 0; i < clients.size(); i++) {
        for (int j = 0; j < clients.size(); j++) {
            sprintf(info, "%s", clients[j].username);
            send(clients[i].c_socket, info, username_length, 0);
        }
    }
    closesocket(client_socket);
    return 0;
}

int main() {
    //system("chcp 65001");
    //启动服务
    WSADATA wsaDATA;
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
        printf("WSA服务启动失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("WSA服务启动成功！");

    //创建socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        printf("创建listen_socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    SOCKET c_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_listen_socket == INVALID_SOCKET) {
        printf("创建listen_socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("创建listen_socket成功！");

    //设置属性
    struct sockaddr_in local = { 0 };
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = inet_addr("0.0.0.0");
    //绑定端口
    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
        printf("绑定Socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    struct sockaddr_in local_c = { 0 };
    local_c.sin_family = AF_INET;
    local_c.sin_port = htons(8081);
    local_c.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(c_listen_socket, (struct sockaddr*)&local_c, sizeof(local_c)) == -1) {
        printf("绑定Socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    puts("绑定Socket成功！");

    //启动监听
    if (listen(listen_socket, 10) == -1) {
        printf("启动listen_socket监听失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    if (listen(c_listen_socket, 10) == -1) {
        printf("启动listen_socket监听失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("启动listen_socket监听成功！");
    cout << "服务器IP：" << getlocalip() << endl;
    CreateThread(NULL, 0, Send, NULL, 0, NULL);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);  //阻塞
        SOCKET c_client_socket = accept(c_listen_socket, NULL, NULL);  //阻塞

        if (client_socket == INVALID_SOCKET || c_client_socket == INVALID_SOCKET) {
            continue;
        }

        status1 = false;
        char client_ip[256];

        recv(client_socket, client_ip, 256, 0);
        printf("%s已连接！\n", utg(client_ip).c_str());
        //online_poeple++;

        //开辟内存创建Data类型指针并赋值(CreateThread只能传一个参数，包装多参数至结构体)
        Data* data = (Data*)calloc(1, sizeof(Data));
        if (data != NULL) {
            data->client_ip = client_ip;
            data->socket = client_socket;
            data->c_socket = c_client_socket;
            //data->username = zsbd;

            //创建线程
            CreateThread(NULL, 0, Receive, (LPVOID)data, 0, NULL);
        }
    }

    WSACleanup();

    return 0;
}