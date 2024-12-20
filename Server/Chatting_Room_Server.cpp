#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <Windows.h>
#include <direct.h>
#include <io.h>
#include "ip.h"
#pragma comment(lib, "ws2_32.lib")

// 全局变量，用于存储历史消息列表，方便在不同函数间传递
std::vector<std::string> g_historyMessages;

using namespace std;

//int online_poeple = 0;        //当前在线人数
//int message_number = 0;        //缓冲区存有消息数量
bool status1 = true;        //是否有人正在连接
bool status2 = true;        //是否在发送信息
const int username_length = 1024;
const int password_length = 1024;
const int message_length = 1024;
const int line_len = 120;


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

inline string utg(const string& utf8Str) {
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
const char* DATABASE_DIR = "./.data";
const char* DATABASE_USER_INFO = "./.data/users.txt";
const char* DATABASE_MESSAGES = "./.data/messages.txt";
const char* DATABASE_BAN_LIST = "./.data/ban.txt";

// 检查用户名是否已存在于数据库中
bool userExists(const char* username) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    if (file == NULL) {
        // 如果文件不存在，视为没有重复用户，返回false
        return false;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[username_length];
        int ret = sscanf(line, "%s", storedUsername);
        if (ret != 1) {
            printf("\rstr Error!\n");
            printf("\r/>");
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
    FILE* file = fopen(DATABASE_USER_INFO, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", username, password);
        fclose(file);
    }
    else {
        // 可添加更完善的错误处理，比如打印错误信息等
        printf("\r无法打开数据库文件进行写入操作！！！错误代码：%d\n", GetLastError());
        printf("\r/>");
    }
}

//将信息存储到数据库
void writeMessageToDatabase(const char* message) {
    FILE* file = fopen(DATABASE_MESSAGES, "a");
    if (file != NULL) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }
    else
    {
        printf("\r无法打开数据库文件进行写入操作！！！错误代码：%d\n", GetLastError());
        printf("\r/>");
    }
}

// 登录验证
string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    FILE* fileb = fopen(DATABASE_BAN_LIST, "r");
    // 定义文件指针history_file，并通过fopen以只读方式打开历史消息文件（".data/messages.txt"），若打开失败则返回"reject"
    FILE* history_file = fopen(DATABASE_MESSAGES, "r");
    if (file == NULL || fileb == NULL) {
        return "reject";
    }

    char line[username_length];
    //封禁检测
    while (fgets(line, username_length, fileb) != NULL) {
        char storedUsername[username_length];
        int ret = sscanf(line, "%s", storedUsername);
        storedUsername[strlen(storedUsername)] = '\0';
        if (ret <= 0) {
            printf("\rstr Error!\n");
            printf("\r/>");
            return "reject";
        }
        if (!strcmp(storedUsername, username)) {
            return "ban";
        }
    }
    //账户存在检测
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[username_length];
        char storedPassword[password_length];
        int ret = sscanf(line, "%s %s", storedUsername, storedPassword);
        if (ret != 2) {
            printf("\rstr Error!\n");
            printf("\r/>");
            return "reject";
        }
        storedUsername[strlen(storedUsername)] = '\0';
        storedPassword[strlen(storedPassword)] = '\0';
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            g_historyMessages.clear();  // 先清空全局变量中的历史消息列表（避免之前残留数据影响）
            char history_line[message_length];
            // 使用fgets从history_file指向的历史消息文件中逐行读取消息，将每行消息存入g_historyMessages全局向量中
            while (fgets(history_line, message_length, history_file) != NULL) {
                history_line[strlen(history_line) - 1] = '\0';
                g_historyMessages.push_back(history_line);
            }
            fclose(history_file);  // 完成历史消息读取后，关闭文件，释放资源
            fclose(file);
            fclose(fileb);
            return "accept";
        }
    }

    fclose(file);
    fclose(fileb);
    fclose(history_file);
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

/*输出分隔符*/
void print_lines(int n) {
    printf("\r");
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= line_len; j++) {
            printf("-");
        }
        printf("\n");
    }
}

/*居中输出*/
void print(const char s[]) {
    int len = (int)strlen(s);                        //获取字符串长度
    int blank = (line_len - len) / 2 - 3;       //计算前部空格数
    printf("\r");
    for (int i = 1; i <= blank; i++) {
        printf(" ");
    }
    printf("%s\n", s);
}

//服务器指令
DWORD WINAPI Server_Command(LPVOID lpThreadParameter) {
    std::string command;
    while (1) {
        char s = getchar();
        if (s == '\n') {
            printf("\r/>");
            continue;
        }
        std::cin >> command;
        command = s + command;
        if (command == "help") {
            print_lines(1);
            print("服务器指令列表");
            print_lines(1);
            std::cout << "ban [username]  :  封禁用户\n" <<
                "clear           :  清屏\n" <<
                "get             :  获取历史消息\n" <<
                "help            :  打印帮助列表\n" <<
                "kick [username] :  踢出用户\n" <<
                "list            :  列出在线用户\n";
            print_lines(1);
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "ban") {
            cin >> command;
            bool success = false;
            for (auto& clt : clients) {
                if (clt.username == command) {
                    FILE* filew = fopen(DATABASE_BAN_LIST, "a+");
                    if (filew != NULL) {
                        fprintf(filew, "%s\n", command.c_str());
                        printf("用户:[ %s ]已被封禁！\n", command.c_str());
                        success = true;
                        closesocket(clt.socket);
                        closesocket(clt.c_socket);
                        remove_client(clt.socket);
                        fclose(filew);
                    }
                    else {
                        cout << "数据库文件打开失败！！！" << endl;
                    }
                }
            }
            if (!success) {
                printf("未找到用户:[ %s ]！\n", command.c_str());
            }
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "kick") {
            cin >> command;
            for (auto& clt : clients) {
                if (clt.username == command) {
                    printf("用户:[ %s ]已被踢出！\n", command.c_str());
                    closesocket(clt.socket);
                    closesocket(clt.c_socket);
                    remove_client(clt.socket);
                }
            }
            printf("\r/>");
            getchar();
        }
        if (command == "clear") {
            system("cls");
            cout << "服务器ip：" << getlocalip() << endl;
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "list") {
            print_lines(1);
            if (clients.size() == 0) {
                print("无在线用户");
            }
            for (auto& clt : clients) {
                printf("%s %s %zu\n", utg(clt.client_ip).c_str(), utg(clt.username).c_str(), clt.socket);
            }
            print_lines(1);
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "get") {
            continue;
        }
        printf("未知指令！输入 help 查看指令列表\n/>");
        getchar();
    }
}

// 发送线程
DWORD WINAPI Send(LPVOID lpThreadParameter) {
    while (1) {
        /*for (int i = 0; i < clients.size(); i++) {
            cout << clients[i].client_ip << " ";
        }
        cout << "end" << endl;
        Sleep(1000);*/
        if (!messages.empty()) {
            //向非分发信息的客户端发送消息缓冲池的消息
            status1 = false;
            string temp;
            temp.append(messages.front().username);
            temp.append(":");
            temp.append(messages.front().message);
            writeMessageToDatabase(temp.c_str());
            cout << "\r分发消息至:[ ";
            for (auto& clt : clients) {
                //如果是发送者，则跳过
                if (messages.front().sender_socket == clt.socket) {
                    continue;
                }
                while (!status2);
                send(clt.socket, temp.c_str(), username_length, 0);
                cout << clt.socket << " ";
                //cout << temp;
                //cout << "send" << endl;
            }
            cout << "]" << endl;
            printf("\r/>");
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
        puts("\rchar*类型指针为空！！！");
        return -1;
    }
    int ret = recv(client_socket, status, 256, 0);
    if (ret <= 0) {
        free(input_username);
        free(input_password);
        free(status);
        printf("\r%s已断开！\n", utg(client_ip).c_str());
        printf("\r/>");
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
                    puts("\rchar*类型指针为空！！！");
                    return -1;
                }
                ret = recv(client_socket, input_username, username_length, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("\r%s已断开！\n", utg(client_ip).c_str());
                    printf("\r/>");
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
                    printf("\r%s已断开！\n", utg(client_ip).c_str());
                    printf("\r/>");
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                //printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
                //从数据库检查用户信息
                string check = check_data_login(input_username, input_password);
                ret = send(client_socket, check.c_str(), 256, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("\r%s已断开！\n", utg(client_ip).c_str());
                    printf("\r/>");
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (check == "accept") {
                    login_success = true;
                    strcpy(data.username, input_username);
                    printf("\r%s(%s : %zu)登录成功！\n", utg(client_ip).c_str(), utg(data.username).c_str(), data.socket);
                    printf("\r/>");
                    char info[256];// 发送历史消息给登录成功的客户端
                    for (const auto& msg : g_historyMessages) {
                        send(client_socket, msg.c_str(), message_length, 0);
                    }
                    g_historyMessages.clear();  // 发送完后清空全局变量中的历史消息列表，避免下次使用时出现重复发送等问题
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
                else if (check == "ban") {
                    printf("\r封禁账户:[ %s ]登录失败\n", input_username);
                    printf("\r/>");
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
                puts("\rchar*类型指针为空！！！");
                return -1;
            }
            ret = recv(client_socket, input_username, username_length, 0);    //接收用户名
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("\r%s已断开！\n", utg(client_ip).c_str());
                printf("\r/>");
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
                printf("\r%s已断开！\n", utg(client_ip).c_str());
                printf("\r/>");
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            //printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
            //从数据库检查用户信息
            string check = check_data_regist(input_username);
            ret = send(client_socket, check.c_str(), 256, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("\r%s已断开！\n", utg(client_ip).c_str());
                printf("\r/>");
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            if (check == "accept") {
                printf("\r%s注册成功！用户名：%s\n", utg(client_ip).c_str(), utg(input_username).c_str());
                printf("\r/>");
                writeUserToDatabase(input_username, input_password);
                recv(client_socket, input_username, username_length, 0);
                strcpy(status, "login");
            }
        }
    }
    status1 = true;
    //cout << utg(data.username) << "登录！" << endl;
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
        cout << "\r" << utg(temp.username) << "(" << temp.sender_socket << ")" << ":" << utg(temp.message) << endl;
        printf("\r/>");
        messages.push(temp);
        status2 = true;
        //while (!status1);
    }
    printf("\r%s已断开！\n", utg(client_ip).c_str());
    printf("\r/>");
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
    closesocket(c_client_socket);
    return 0;
}

int main() {
    //system("chcp 65001");
    //创建数据库文件夹
    if (_access(DATABASE_DIR, 0) == -1) {
        int ret = _mkdir(DATABASE_DIR);
        if (ret != 0) {
            printf("\r数据库文件夹创建失败！！！错误代码：%d\n", GetLastError());
            return -1;
        }
    }
    FILE* init_file = fopen(DATABASE_BAN_LIST, "a");
    fclose(init_file);
    init_file = fopen(DATABASE_USER_INFO, "a");
    fclose(init_file);
    init_file = fopen(DATABASE_MESSAGES, "a");
    fclose(init_file);
    //启动服务
    WSADATA wsaDATA;
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
        printf("\rWSA服务启动失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("\rWSA服务启动成功！");

    //创建socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        printf("\r创建listen_socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    SOCKET c_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_listen_socket == INVALID_SOCKET) {
        printf("\r创建listen_socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("\r创建listen_socket成功！");

    //设置属性
    struct sockaddr_in local = { 0 };
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = inet_addr("0.0.0.0");
    //绑定端口
    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
        printf("\r绑定Socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    struct sockaddr_in local_c = { 0 };
    local_c.sin_family = AF_INET;
    local_c.sin_port = htons(8081);
    local_c.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(c_listen_socket, (struct sockaddr*)&local_c, sizeof(local_c)) == -1) {
        printf("\r绑定Socket失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    puts("\r绑定Socket成功！");

    //启动监听
    if (listen(listen_socket, 10) == -1) {
        printf("\r启动listen_socket监听失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }
    if (listen(c_listen_socket, 10) == -1) {
        printf("\r启动listen_socket监听失败！！！错误代码：%d\n", GetLastError());
        return -1;
    }

    puts("\r启动listen_socket监听成功！");
    puts("所有服务启动成功！(输入 help 查看指令列表)");
    cout << "服务器IP：" << getlocalip() << endl;
    printf("\r/>");
    CreateThread(NULL, 0, Send, NULL, 0, NULL);
    CreateThread(NULL, 0, Server_Command, NULL, 0, NULL);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);  //阻塞
        SOCKET c_client_socket = accept(c_listen_socket, NULL, NULL);  //阻塞

        if (client_socket == INVALID_SOCKET || c_client_socket == INVALID_SOCKET) {
            continue;
        }

        status1 = false;
        char client_ip[256];

        recv(client_socket, client_ip, 256, 0);
        printf("\r%s已连接！\n", utg(client_ip).c_str());
        printf("\r/>");
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