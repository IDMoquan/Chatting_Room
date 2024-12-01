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

//int online_poeple = 0;        //��ǰ��������
//int message_number = 0;        //������������Ϣ����
bool status1 = true;        //�Ƿ�������������
bool status2 = true;        //�Ƿ��ڷ�����Ϣ

// �����������
typedef struct {
    SOCKET socket;
    char* client_ip;
    char username[256];
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

// ģ�����ݿ��ļ�·�����ɸ���ʵ������޸�
const char* DATABASE_FILE = "users.txt";

// ����û����Ƿ��Ѵ��������ݿ���
bool userExists(const char* username) {
    FILE* file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        // ����ļ������ڣ���Ϊû���ظ��û�������false
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[256];
        sscanf(line, "%s", storedUsername);
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
    FILE* file = fopen(DATABASE_FILE, "a");
    if (file != NULL) {
        fprintf(file, "%s %s\n", username, password);
        fclose(file);
    }
    else {
        // ����Ӹ����ƵĴ����������ӡ������Ϣ��
        printf("�޷������ݿ��ļ�����д�������\n");
    }
}

// ��¼��֤
string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        return "reject";
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        char storedUsername[256];
        char storedPassword[256];
        sscanf(line, "%s %s", storedUsername, storedPassword);
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            fclose(file);
            return "accept";
        }
    }

    fclose(file);
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

// �����߳�
DWORD WINAPI Send(LPVOID lpThreadParameter) {
    while (1) {
        //for (int i = 0; i < clients.size(); i++) {
        //    cout << clients[i].client_ip << " ";
        //}
        //cout << "end" << endl;
        //Sleep(1000);
        if (!messages.empty()) {
            //��Ƿַ���Ϣ�Ŀͻ��˷�����Ϣ����ص���Ϣ
            status1 = false;
            cout << "�ַ���Ϣ��:[";
            for (auto& clt : clients) {
                if (messages.front().sender_socket == clt.socket) {
                    continue;
                }
                string temp;
                temp.append(clt.username);
                temp.append(":");
                temp.append(messages.front().message);
                while (!status2);
                send(clt.socket, temp.c_str(), sizeof(temp), 0);
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

// �����߳�
DWORD WINAPI Receive(LPVOID lpThreadParameter) {
    //�𿪽ṹ���װ
    Data data = *(Data*)lpThreadParameter;
    SOCKET client_socket = data.socket;    //ȡ��socket
    char* client_ip = data.client_ip;        //ȡ��client_ip
    char* input_username = (char*)malloc(1024 * sizeof(char));
    char* input_password = (char*)malloc(1024 * sizeof(char));
    char* status = (char*)malloc(256 * sizeof(char));
    delete (lpThreadParameter);                //�ͷ��ڴ�
    if (status == nullptr) {
        puts("char*����ָ��Ϊ�գ�����");
        return -1;
    }
    int ret = recv(client_socket, status, 256, 0);
    if (ret <= 0) {
        free(input_username);
        free(input_password);
        free(status);
        printf("%s�ѶϿ���\n", client_ip);
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
                    puts("char*����ָ��Ϊ�գ�����");
                    return -1;
                }
                ret = recv(client_socket, input_username, 1024, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s�ѶϿ���\n", client_ip);
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (!strcmp(input_username, "regist")) {
                    strcpy(status, "regist");
                    break;
                }
                ret = recv(client_socket, input_password, 1024, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s�ѶϿ���\n", client_ip);
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                printf("username:%s\npassword:%s\n", input_username, input_password);
                //�����ݿ����û���Ϣ
                string check = check_data_login(input_username, input_password);
                ret = send(client_socket, check.c_str(), 256, 0);
                if (ret <= 0) {
                    free(input_username);
                    free(input_password);
                    free(status);
                    printf("%s�ѶϿ���\n", client_ip);
                    remove_client(client_socket);
                    closesocket(client_socket);
                    return -1;
                }
                if (check == "accept") {
                    printf("%s��¼�ɹ���\n", client_ip);
                    login_success = true;
                    strcpy(data.username, input_username);
                    const char* back_info = "accept";
                    send(client_socket, back_info, 256, 0);
                    break;
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
                puts("char*����ָ��Ϊ�գ�����");
                return -1;
            }
            ret = recv(client_socket, input_username, 256, 0);    //�����û���
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s�ѶϿ���\n", client_ip);
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            //���յ����ص�¼����Ϣ
            if (!strcmp(input_username, "login")) {
                strcpy(status, "login");
                continue;
            }
            ret = recv(client_socket, input_password, 256, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s�ѶϿ���\n", client_ip);
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            printf("username:%s\npassword:%s\n", input_username, input_password);
            //�����ݿ����û���Ϣ
            string check = check_data_regist(input_username);
            ret = send(client_socket, check.c_str(), 256, 0);
            if (ret <= 0) {
                free(input_username);
                free(input_password);
                free(status);
                printf("%s�ѶϿ���\n", client_ip);
                remove_client(client_socket);
                closesocket(client_socket);
                return -1;
            }
            if (check == "accept") {
                printf("%sע��ɹ����û�����%s\n", client_ip, input_username);
                writeUserToDatabase(input_username, input_password);
                recv(client_socket, input_username, 256, 0);
                strcpy(status, "login");
            }
        }
    }
    free(input_username);
    free(input_password);
    free(status);
    clients.push_back(data);
    status1 = true;
    cout << data.username << "��¼��" << endl;
    while (true) {
        char buffer[1024] = { 0 };
        //������Ϣ
        int ret = recv(client_socket, buffer, 1024, 0);
        if (ret <= 0) {
            break;
        }
        Messages temp;
        temp.client_ip = client_ip;
        temp.message = buffer;
        temp.sender_socket = client_socket;
        temp.username = data.username;
        cout << temp.username << "(" << temp.sender_socket << ")" << ":" << temp.message << endl;
        messages.push(temp);
        status2 = true;
        //while (!status1);
    }
    printf("%s�ѶϿ���\n", client_ip);
    remove_client(client_socket);
    closesocket(client_socket);
    return 0;
}

int main() {
    //system("chcp 65001");
    //��������
    WSADATA wsaDATA;
    if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
        printf("WSA��������ʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("WSA���������ɹ���");

    //����socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        printf("����listen_socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("����listen_socket�ɹ���");

    //��������
    struct sockaddr_in local = { 0 };
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = inet_addr("0.0.0.0");
    //�󶨶˿�
    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == -1) {
        printf("��Socketʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }
    puts("��Socket�ɹ���");

    //��������
    if (listen(listen_socket, 10) == -1) {
        printf("����listen_socket����ʧ�ܣ�����������룺%d\n", GetLastError());
        return -1;
    }

    puts("����listen_socket�����ɹ���");
    cout << "������IP��" << getlocalip() << endl;
    CreateThread(NULL, 0, Send, NULL, 0, NULL);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);  //����
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        status1 = false;
        char client_ip[1024];

        recv(client_socket, client_ip, 256, 0);
        printf("%s�����ӣ�\n", client_ip);
        //online_poeple++;

        //�����ڴ洴��Data����ָ�벢��ֵ(CreateThreadֻ�ܴ�һ����������װ��������ṹ��)
        Data* data = (Data*)calloc(1, sizeof(Data));
        if (data != NULL) {
            data->client_ip = client_ip;
            data->socket = client_socket;
            //data->username = zsbd;

            //�����߳�
            CreateThread(NULL, 0, Receive, (LPVOID)data, 0, NULL);
        }
    }

    WSACleanup();

    return 0;
}