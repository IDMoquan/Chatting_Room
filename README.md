# ������˵�� "Chatting Room" Introduction
>С���Ա�����������º��š�������

---
## 0. Ŀ¼
- [������˵�� "Chatting Room" Introduction](#������˵��-chatting-room-introduction)
	- [0. Ŀ¼](#0-Ŀ¼)
	- [1. ��������](#1-��������)
		- [1.1 �����](#11-�����)
		- [1.2 �ͻ���](#12-�ͻ���)
	- [2. �������](#2-�������)
		- [2.1 ����ͷ�ļ�/��](#21-����ͷ�ļ���)
			- [2.1.1 �����](#211-�����)
			- [2.1.2 �ͻ���](#212-�ͻ���)
		- [2.2 ���ݽṹ](#22-���ݽṹ)
			- [2.2.1 �����](#221-�����)
			- [2.2.2 �ͻ���](#222-�ͻ���)
		- [2.3 ���ֺ�������](#23-���ֺ�������)
			- [2.3.1 UTF-8תGBK(ͨ��)](#231-utf-8תgbkͨ��)
			- [2.3.2 ��ȡ����ip(ͨ��)](#232-��ȡ����ipͨ��)
			- [2.3.3 ��֤��¼/ע��(�����)](#233-��֤��¼ע������)
			- [2.3.4 ����Socketɾ�������û�(�����)](#234-����socketɾ�������û������)
			- [2.3.5 ���������������Ϣ�������û�(�����)](#235-���������������Ϣ�������û������)
			- [2.3.6 ���������û���Ϣ(�ͻ���)](#236-���������û���Ϣ�ͻ���)
	- [3. ��Ҫ����](#3-��Ҫ����)
		- [3.1 �����](#31-�����)
		- [3.2 �ͻ���](#32-�ͻ���)
	- [4. �߼�˼��](#4-�߼�˼��)
		- [4.1 ע��](#41-ע��)
		- [4.2 ��¼](#42-��¼)
		- [4.3 ����](#43-����)
---

## 1. ��������
�������Ϊ������ɲ���: �����&�ͻ���<br>
### 1.1 �����
   - C����
   - �������
### 1.2 �ͻ���
   - C\C++����
   - Qt���ڿ���
   - �������
---

## 2. �������

### 2.1 ����ͷ�ļ�/��
#### 2.1.1 �����
```
- stdio.h		//�������
- iostream		//�������
- string.h		//�ַ������
- winsock2.h	//��������
- vector		//�����û�
- queue			//��Ϣ����
- algorithm		//remove_if()����ɾ�������û�
- Windows.h		//�����߳�
- direct.h		//�ļ��в���
- io.h			//�ļ��в���
- ws2_32.lib	//Winsock������
```

#### 2.1.2 �ͻ���
```
- stdio.h						//�������
- iostream						//�������
- string						//�ַ������
- Windows.h						//�����߳�
- winsock2.h					//��������
- qregularexpression			//���������ʽ
- QRegularExpressionValidator	//���������ʽ
- QtWidgets/QApplication		//����QWidget����
- qlistwidget.h					//����QListWidget����
- qstringlistmodel.h			//����QStrigListModel����
- ws2_32.lib					//Winsock������
- cwchar						//���ַ�ת��
- cstring						//���ַ�ת��
```

### 2.2 ���ݽṹ

#### 2.2.1 �����<br>

```C
//�����������
typedef struct {
SOCKET socket;							//��Ӧ�û�socket
SOCKET c_socket;						//��Ӧ�û�socket2
char* client_ip;						//�û�ip��ַ
char username[username_length];			//�û���
} Data;

//������Ϣ
typedef struct {
SOCKET sender_socket;					//������socket
char* client_ip;						//������ip
char* username;							//�������û���
string message;							//���͵���Ϣ
} Messages;

vector<Data> clients;     				//������Ա�б�
queue<Messages> messages; 				//������Ϣ����

bool status1 = true;  					//�Ƿ�������������
bool status2 = true;  					//�Ƿ��ڷ�����Ϣ
const int username_length = 1024;		//����û�������
const int password_length = 1024;		//������볤��
const int message_length = 1024;		//�����Ϣ����
const int line_len = 120;				//DOS���ڿ��
```

#### 2.2.2 �ͻ���

```C++
std::string localip;					//����ip
SOCKET client_socket, client_socket_c;	//����socket
bool connect_status = true;				//�Ƿ����ӳɹ�
int client_count = 0;					//�����û�����
char e_server_ip[256] = { 0 };			//������ip
char c_username[1024] = { 0 };			//�û���(char)
const int username_length = 1024;		//����û�������
const int password_length = 1024;		//������볤��
const int message_length = 1024;		//�����Ϣ����
const char* status = "login";			//��¼����״̬
```

### 2.3 ���ֺ�������
#### 2.3.1 UTF-8תGBK(ͨ��)
<details><summary><mark>����鿴����</mark></summary>

```C++
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
```
</details>

#### 2.3.2 ��ȡ����ip(ͨ��)
<details><summary><mark>����鿴����</mark></summary>

```C
std::string getlocalip() {
	int ret;
	char hostname[256] = { 0 };
	ret = gethostname(hostname, sizeof(hostname));
	hostent* host = gethostbyname(hostname);
	return inet_ntoa(*(struct in_addr*)*host->h_addr_list);
}
```
</details>

#### 2.3.3 ��֤��¼/ע��(�����)
<details><summary><mark>����鿴����</mark></summary>

```C++
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
```
</details>

#### 2.3.4 ����Socketɾ�������û�(�����)
<details><summary><mark>����鿴����</mark></summary>

```C++
// �Ƴ��ͻ������������Ϣ
void remove_client(SOCKET target_socket) {
    clients.erase(
        remove_if(clients.begin(), clients.end(),
            [&target_socket](const Data& s) {
                return target_socket == s.socket;
            }), clients.end()
    );
}
```
</details>

#### 2.3.5 ���������������Ϣ�������û�(�����)
<details><summary><mark>����鿴����</mark></summary>

```C
// �����߳�
DWORD WINAPI Send(LPVOID lpThreadParameter) {
    while (1) {
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
            }
            cout << "]" << endl;
            printf("\r/>");
            status1 = true;
            messages.pop();
        }
    }
    return 0;
}
```
</details>

#### 2.3.6 ���������û���Ϣ(�ͻ���)
<details><summary><mark>����鿴����</mark></summary>

```C++
//���������û���Ϣ�߳�
DWORD WINAPI Receive_clients(LPVOID lpThreadParameter) {
	struct Q2type data = *(struct Q2type *)lpThreadParameter;	//�����������ת�����ṹ��
	QListWidget* message_list = data.message_list;				//���ṹ����Ϣת����QListWidget����
	QLabel *label_count = data.client_count;					//���ṹ����Ϣת����QListWidget����
	//�����б�����
	message_list->setSpacing(5);
	message_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	while (connect_status);										//�ȴ����ӳɹ�,��ֹSocketδ����ʱ����recv��������
	while (1) {
		char c_client_count[256] = { 0 };						
		recv(client_socket_c, c_client_count, 256, 0);			//��������
		c_list.clear();	
		message_list->clear();
		client_count = charToint(c_client_count);				//����charToInt
		label_count->setText(c_client_count);					//��ʾ����
		char info[username_length] = { 0 };						//�����û��б�
		for (int i = 0; i < client_count; i++) {				
			recv(client_socket_c, info, username_length, 0);	//��ȡ�û���
			c_list.append(info);								//���б�����û�
		}
		QStringListModel* c_listmodel = new QStringListModel(c_list);
		message_list->addItems(c_list);							//��ʾ�б�
	}
}
```
</details>

---

## 3. ��Ҫ����
### 3.1 �����
 1. [x] �洢�û���Ϣ
 2. [x] �û���¼ע����֤
 3. [x] ��Ϣ��ת�ַ�
 4. [x] �û�Ȩ�޹���
 5. [x] ָ����� 
 6. [x] ��Ϣ�洢
### 3.2 �ͻ���
1. [x] ����ָ��������
2. [x] ��¼ע��
3. [x] �鿴�����û�
4. [x] ���ͽ�����Ϣ
5. [ ] ͷ����ʾ
---

## 4. �߼�˼��
### 4.1 ע��
<details><summary><mark>����鿴����ͼ</mark></summary>

![ע��](Src/ע��.drawio.png)
</details>

### 4.2 ��¼
<details><summary><mark>����鿴����ͼ</mark></summary>

![��¼](Src/��¼.drawio.png)
</details>

### 4.3 ����
<details><summary><mark>����鿴����ͼ</mark></summary>

![����](Src/����.drawio.png)
</details>

---
