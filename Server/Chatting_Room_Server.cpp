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

// ȫ�ֱ��������ڴ洢��ʷ��Ϣ�б������ڲ�ͬ�����䴫��
std::vector<std::string> g_historyMessages;

using namespace std;

//int online_poeple = 0;        //��ǰ��������
//int message_number = 0;        //������������Ϣ����
bool status1 = true;        //�Ƿ�������������
bool status2 = true;        //�Ƿ��ڷ�����Ϣ
const int username_length = 1024;
const int password_length = 1024;
const int message_length = 1024;
const int line_len = 120;


// �����������
typedef struct {
    SOCKET socket;
    char* client_ip;
    char username[username_length];
    SOCKET c_socket;
} Data;

// ������Ϣ
typedef struct {
    SOCKET sender_socket;
    char* client_ip;
    string message;
    char* username;
} Messages;

vector<Data> clients;
queue<Messages> messages;        //������Ϣ����

inline string utg(const string& utf8Str) {
    // ���ȼ�����Ҫ�Ŀ��ַ�������
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::vector<wchar_t> wideStr(wideLength);

    // ��UTF-8ת��Ϊ���ַ���
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideLength);

    // ����GBK�ַ�������
    int gbkLength = WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> gbkStr(gbkLength);

    // �����ַ���ת��ΪGBK�ַ���
    WideCharToMultiByte(CP_ACP, 0, &wideStr[0], -1, &gbkStr[0], gbkLength, nullptr, nullptr);

    return string(gbkStr.begin(), gbkStr.end() - 1); // ��ȥĩβ�Ŀ��ַ�
}

// ģ�����ݿ��ļ�·�����ɸ���ʵ������޸�
const char* DATABASE_DIR = "./.data";
const char* DATABASE_USER_INFO = "./.data/users.txt";
const char* DATABASE_MESSAGES = "./.data/messages.txt";
const char* DATABASE_BAN_LIST = "./.data/ban.txt";

// ����û����Ƿ��Ѵ��������ݿ���
bool userExists(const char* username) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    if (file == NULL) {
        // ����ļ������ڣ���Ϊû���ظ��û�������false
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

// �����û���Ϣд�����ݿ��ļ�
void writeUserToDatabase(const char* username, const char* password) {
    FILE* file = fopen(DATABASE_USER_INFO, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", username, password);
        fclose(file);
    }
    else {
        // ����Ӹ����ƵĴ����������ӡ������Ϣ��
        printf("\r�޷������ݿ��ļ�����д�����������������룺%d\n", GetLastError());
        printf("\r/>");
    }
}

//����Ϣ�洢�����ݿ�
void writeMessageToDatabase(const char* message) {
    FILE* file = fopen(DATABASE_MESSAGES, "a");
    if (file != NULL) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }
    else
    {
        printf("\r�޷������ݿ��ļ�����д�����������������룺%d\n", GetLastError());
        printf("\r/>");
    }
}

// ��¼��֤
string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    FILE* fileb = fopen(DATABASE_BAN_LIST, "r");
    // �����ļ�ָ��history_file����ͨ��fopen��ֻ����ʽ����ʷ��Ϣ�ļ���".data/messages.txt"��������ʧ���򷵻�"reject"
    FILE* history_file = fopen(DATABASE_MESSAGES, "r");
    if (file == NULL || fileb == NULL) {
        return "reject";
    }

    char line[username_length];
    //������
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
    //�˻����ڼ��
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
            g_historyMessages.clear();  // �����ȫ�ֱ����е���ʷ��Ϣ�б�����֮ǰ��������Ӱ�죩
            char history_line[message_length];
            // ʹ��fgets��history_fileָ�����ʷ��Ϣ�ļ������ж�ȡ��Ϣ����ÿ����Ϣ����g_historyMessagesȫ��������
            while (fgets(history_line, message_length, history_file) != NULL) {
                history_line[strlen(history_line) - 1] = '\0';
                g_historyMessages.push_back(history_line);
            }
            fclose(history_file);  // �����ʷ��Ϣ��ȡ�󣬹ر��ļ����ͷ���Դ
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

// ע����֤
string check_data_regist(char* username) {
    if (userExists(username)) {
        return "reject";
    }
    return "accept";
}

// �Ƴ��ͻ������������Ϣ
void remove_client(SOCKET target_socket) {
    clients.erase(
        remove_if(clients.begin(), clients.end(),
            [&target_socket](const Data& s) {
                return target_socket == s.socket;
            }), clients.end()
                );
}

/*����ָ���*/
void print_lines(int n) {
    printf("\r");
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= line_len; j++) {
            printf("-");
        }
        printf("\n");
    }
}

/*�������*/
void print(const char s[]) {
    int len = (int)strlen(s);                        //��ȡ�ַ�������
    int blank = (line_len - len) / 2 - 3;       //����ǰ���ո���
    printf("\r");
    for (int i = 1; i <= blank; i++) {
        printf(" ");
    }
    printf("%s\n", s);
}

//������ָ��
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
            print("������ָ���б�");
            print_lines(1);
            std::cout << "ban [username]  :  ����û�\n" <<
                "clear           :  ����\n" <<
                "get             :  ��ȡ��ʷ��Ϣ\n" <<
                "help            :  ��ӡ�����б�\n" <<
                "kick [username] :  �߳��û�\n" <<
                "list            :  �г������û�\n";
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
                        printf("�û�:[ %s ]�ѱ������\n", command.c_str());
                        success = true;
                        closesocket(clt.socket);
                        closesocket(clt.c_socket);
                        remove_client(clt.socket);
                        fclose(filew);
                    }
                    else {
                        cout << "���ݿ��ļ���ʧ�ܣ�����" << endl;
                    }
                }
            }
            if (!success) {
                printf("δ�ҵ��û�:[ %s ]��\n", command.c_str());
            }
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "kick") {
            cin >> command;
            for (auto& clt : clients) {
                if (clt.username == command) {
                    printf("�û�:[ %s ]�ѱ��߳���\n", command.c_str());
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
            cout << "������ip��" << getlocalip() << endl;
            printf("\r/>");
            getchar();
            continue;
        }
        if (command == "list") {
            print_lines(1);
            if (clients.size() == 0) {
                print("�������û�");
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
        printf("δָ֪����� help �鿴ָ���б�\n/>");
        getchar();
    }
}

// �����߳�
DWORD WINAPI Send(LPVOID lpThreadParameter) {
    while (1) {
        /*for (int i = 0; i < clients.size(); i++) {
            cout << clients[i].client_ip << " ";
        }
        cout << "end" << endl;
        Sleep(1000);*/
        if (!messages.empty()) {
            //��Ƿַ���Ϣ�Ŀͻ��˷�����Ϣ����ص���Ϣ
            status1 = false;
            string temp;
            temp.append(messages.front().username);
            temp.append(":");
            temp.append(messages.front().message);
            writeMessageToDatabase(temp.c_str());
            cout << "\r�ַ���Ϣ��:[ ";
            for (auto& clt : clients) {
                //����Ƿ����ߣ�������
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

// �����߳�
DWORD WINAPI Receive(LPVOID lpThreadParameter) {
    //�𿪽ṹ���װ
    Data data = *(Data*)lpThreadParameter;
    SOCKET client_socket = data.socket;    //ȡ��socket
    SOCKET c_client_socket = data.c_socket;
    char* client_ip = data.client_ip;        //ȡ��client_ip
    char* input_username = (char*)malloc(username_length * sizeof(char));
    char* input_password = (char*)malloc(password_length * sizeof(char));
    char* status = (char*)malloc(256 * sizeof(char));
    delete (lpThreadParameter);                //�ͷ��ڴ�
    if (status == nullptr) {
        puts("\rchar*����ָ��Ϊ�գ�����");
        return -1;
    }
    int ret = recv(client_socket, status, 256, 0);
    if (ret <= 0) {
        free(input_username);
        free(input_password);
        free(status);
        printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
        printf("\r/>");
        closesocket(client_socket);
        return -1;
    }
    while (1) {
        //login��¼
        if (!strcmp(status, "login")) {
            bool login_success = false;
            for (int i = 1; i <= 5; i++) {
                if (input_username == nullptr || input_password == nullptr) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    puts("\rchar*����ָ��Ϊ�գ�����");
                    return -1;
                }
                ret = recv(client_socket, input_username, username_length, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
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
                    printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
                    printf("\r/>");
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                //printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
                //�����ݿ����û���Ϣ
                string check = check_data_login(input_username, input_password);
                ret = send(client_socket, check.c_str(), 256, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
                    printf("\r/>");
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (check == "accept") {
                    login_success = true;
                    strcpy(data.username, input_username);
                    printf("\r%s(%s : %zu)��¼�ɹ���\n", utg(client_ip).c_str(), utg(data.username).c_str(), data.socket);
                    printf("\r/>");
                    char info[256];// ������ʷ��Ϣ����¼�ɹ��Ŀͻ���
                    for (const auto& msg : g_historyMessages) {
                        send(client_socket, msg.c_str(), message_length, 0);
                    }
                    g_historyMessages.clear();  // ����������ȫ�ֱ����е���ʷ��Ϣ�б������´�ʹ��ʱ�����ظ����͵�����
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
                    printf("\r����˻�:[ %s ]��¼ʧ��\n", input_username);
                    printf("\r/>");
                }
            }
            if (login_success) {
                break;
            }
        }
        //ע��
        else {
            if (input_username == nullptr || input_password == nullptr) {
                free(input_username);
                free(input_password);
                free(status);
                puts("\rchar*����ָ��Ϊ�գ�����");
                return -1;
            }
            ret = recv(client_socket, input_username, username_length, 0);    //�����û���
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
                printf("\r/>");
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            //���յ����ص�¼����Ϣ
            if (!strcmp(input_username, "login")) {
                strcpy(status, "login");
                continue;
            }
            ret = recv(client_socket, input_password, password_length, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
                printf("\r/>");
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            //printf("username:%s\npassword:%s\n", utg(input_username).c_str(), utg(input_password).c_str());
            //�����ݿ����û���Ϣ
            string check = check_data_regist(input_username);
            ret = send(client_socket, check.c_str(), 256, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
                printf("\r/>");
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            if (check == "accept") {
                printf("\r%sע��ɹ����û�����%s\n", utg(client_ip).c_str(), utg(input_username).c_str());
                printf("\r/>");
                writeUserToDatabase(input_username, input_password);
                recv(client_socket, input_username, username_length, 0);
                strcpy(status, "login");
            }
        }
    }
    status1 = true;
    //cout << utg(data.username) << "��¼��" << endl;
    while (true) {
        char buffer[message_length] = { 0 };
        //������Ϣ
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
    printf("\r%s�ѶϿ���\n", utg(client_ip).c_str());
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
    //�������ݿ��ļ���
    if (_access(DATABASE_DIR, 0) == -1) {
        int ret = _mkdir(DATABASE_DIR);
        if (ret != 0) {
            printf("\r���ݿ��ļ��д���ʧ�ܣ�����������룺%d\n", GetLastError());
            return -1;
        }
    }
    FILE* init_file = fopen(DATABASE_BAN_LIST, "a");
    fclose(init_file);
    init_file = fopen(DATABASE_USER_INFO, "a");
    fclose(init_file);
    init_file = fopen(DATABASE_MESSAGES, "a");
    fclose(init_file);
    //��������
    WSADATA wsaDATA;
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
        printf("\rWSA��������ʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("\rWSA���������ɹ���");

    //����socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        printf("\r����listen_socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }
    SOCKET c_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_listen_socket == INVALID_SOCKET) {
        printf("\r����listen_socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("\r����listen_socket�ɹ���");

    //��������
    struct sockaddr_in local = { 0 };
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = inet_addr("0.0.0.0");
    //�󶨶˿�
    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
        printf("\r��Socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    struct sockaddr_in local_c = { 0 };
    local_c.sin_family = AF_INET;
    local_c.sin_port = htons(8081);
    local_c.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(c_listen_socket, (struct sockaddr*)&local_c, sizeof(local_c)) == -1) {
        printf("\r��Socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }
    puts("\r��Socket�ɹ���");

    //��������
    if (listen(listen_socket, 10) == -1) {
        printf("\r����listen_socket����ʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }
    if (listen(c_listen_socket, 10) == -1) {
        printf("\r����listen_socket����ʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("\r����listen_socket�����ɹ���");
    puts("���з��������ɹ���(���� help �鿴ָ���б�)");
    cout << "������IP��" << getlocalip() << endl;
    printf("\r/>");
    CreateThread(NULL, 0, Send, NULL, 0, NULL);
    CreateThread(NULL, 0, Server_Command, NULL, 0, NULL);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);  //����
        SOCKET c_client_socket = accept(c_listen_socket, NULL, NULL);  //����

        if (client_socket == INVALID_SOCKET || c_client_socket == INVALID_SOCKET) {
            continue;
        }

        status1 = false;
        char client_ip[256];

        recv(client_socket, client_ip, 256, 0);
        printf("\r%s�����ӣ�\n", utg(client_ip).c_str());
        printf("\r/>");
        //online_poeple++;

        //�����ڴ洴��Data����ָ�벢��ֵ(CreateThreadֻ�ܴ�һ����������װ��������ṹ��)
        Data* data = (Data*)calloc(1, sizeof(Data));
        if (data != NULL) {
            data->client_ip = client_ip;
            data->socket = client_socket;
            data->c_socket = c_client_socket;
            //data->username = zsbd;

            //�����߳�
            CreateThread(NULL, 0, Receive, (LPVOID)data, 0, NULL);
        }
    }

    WSACleanup();

    return 0;
}