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
		- [2.1 ���ݽṹ](#21-���ݽṹ)
			- [2.1.1 �����](#211-�����)
			- [2.1.2 �ͻ���](#212-�ͻ���)
		- [2.2 ���ֺ�������](#22-���ֺ�������)
			- [2.2.1 UTF-8תGBK(ͨ��)](#221-utf-8תgbkͨ��)
			- [2.2.2 ��֤��¼/ע��](#222-��֤��¼ע��)
	- [3. ��Ҫ����](#3-��Ҫ����)
		- [3.1 �����](#31-�����)
		- [3.2 �ͻ���](#32-�ͻ���)
	- [4. �߼�˼��](#4-�߼�˼��)
		- [4.1 ע��](#41-ע��)
		- [4.2 ��¼](#42-��¼)
		- [4.3 ����](#43-����)


## 1. ��������
�������Ϊ������ɲ���: �����&�ͻ���<br>
### 1.1 �����
   - C����
   - Winsock2��
### 1.2 �ͻ���
   - C\C++����
   - Qt��
   - Winsock2��

## 2. �������

### 2.1 ���ݽṹ

#### 2.1.1 �����<br>
<details><summary>����鿴����</summary>

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
</details>

#### 2.1.2 �ͻ���
<details><summary>����鿴����</summary>

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
</details>

### 2.2 ���ֺ�������
#### 2.2.1 UTF-8תGBK(ͨ��)
<details><summary>����鿴����</summary>

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

#### 2.2.2 ��֤��¼/ע��
<details><summary>����鿴����</summary>

```C++
	string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    FILE* fileb = fopen(DATABASE_BAN_LIST, "r");
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
            fclose(file);
            return "accept";
        }
    }

    fclose(file);
    fclose(fileb);
    return "reject";
}
```
</details>

## 3. ��Ҫ����
### 3.1 �����
 1. [x] �洢�û���Ϣ
 2. [x] �û���¼ע����֤
 3. [x] ��Ϣ��ת�ַ�
 4. [x] �û�Ȩ�޹���
 5. [x] ָ����� 
 6. [ ] ��Ϣ�洢
### 3.2 �ͻ���
1. [x] ����ָ��������
2. [x] ��¼ע��
3. [x] �鿴�����û�
4. [x] ���ͽ�����Ϣ
5. [ ] ͷ����ʾ

## 4. �߼�˼��
### 4.1 ע��
<details><summary>����鿴ͼƬ</summary>

![ע��](.\\Src\\ע��.drawio.png)
</details>

### 4.2 ��¼
<details><summary>����鿴ͼƬ</summary>

![��¼](.\\Src\\��¼.drawio.png)
</details>

### 4.3 ����
<details><summary>����鿴ͼƬ</summary>

>>>>>>>>> Temporary merge branch 2
