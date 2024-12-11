# 聊天室说明 "Chatting Room" Introduction
>小组成员：鲍梓涵、陈海桥、王天亮

---
## 0. 目录
- [聊天室说明 "Chatting Room" Introduction](#聊天室说明-chatting-room-introduction)
	- [0. 目录](#0-目录)
	- [1. 技术基础](#1-技术基础)
		- [1.1 服务端](#11-服务端)
		- [1.2 客户端](#12-客户端)
	- [2. 代码组成](#2-代码组成)
		- [2.1 数据结构](#21-数据结构)
			- [2.1.1 服务端](#211-服务端)
			- [2.1.2 客户端](#212-客户端)
		- [2.2 部分函数介绍](#22-部分函数介绍)
			- [2.2.1 UTF-8转GBK(通用)](#221-utf-8转gbk通用)
			- [2.2.2 验证登录/注册](#222-验证登录注册)
	- [3. 主要功能](#3-主要功能)
		- [3.1 服务端](#31-服务端)
		- [3.2 客户端](#32-客户端)
	- [4. 逻辑思想](#4-逻辑思想)
		- [4.1 注册](#41-注册)
		- [4.2 登录](#42-登录)
		- [4.3 聊天](#43-聊天)


## 1. 技术基础
本程序分为两个组成部分: 服务端&客户端<br>
### 1.1 服务端
   - C语言
   - Winsock2库
### 1.2 客户端
   - C\C++语言
   - Qt库
   - Winsock2库

## 2. 代码组成

### 2.1 数据结构

#### 2.1.1 服务端<br>
<details><summary>点击查看代码</summary>

```C
//存放输入数据
typedef struct {
SOCKET socket;							//对应用户socket
SOCKET c_socket;						//对应用户socket2
char* client_ip;						//用户ip地址
char username[username_length];			//用户名
} Data;

//输入信息
typedef struct {
SOCKET sender_socket;					//发送者socket
char* client_ip;						//发送者ip
char* username;							//发送者用户名
string message;							//发送的信息
} Messages;

vector<Data> clients;     				//在线人员列表
queue<Messages> messages; 				//发送信息缓冲

bool status1 = true;  					//是否有人正在连接
bool status2 = true;  					//是否在发送信息
const int username_length = 1024;		//最大用户名长度
const int password_length = 1024;		//最大密码长度
const int message_length = 1024;		//最大消息长度
const int line_len = 120;				//DOS窗口宽度
```
</details>

#### 2.1.2 客户端
<details><summary>点击查看代码</summary>

```C++
	std::string localip;					//本地ip
	SOCKET client_socket, client_socket_c;	//两个socket
	bool connect_status = true;				//是否连接成功
	int client_count = 0;					//在线用户数量
	char e_server_ip[256] = { 0 };			//服务器ip
	char c_username[1024] = { 0 };			//用户名(char)
	const int username_length = 1024;		//最大用户名长度
	const int password_length = 1024;		//最大密码长度
	const int message_length = 1024;		//最大消息长度
	const char* status = "login";			//登录界面状态
```
</details>

### 2.2 部分函数介绍
#### 2.2.1 UTF-8转GBK(通用)
<details><summary>点击查看代码</summary>

```C++
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
```
</details>

#### 2.2.2 验证登录/注册
<details><summary>点击查看代码</summary>

```C++
	string check_data_login(char* username, char* password) {
    FILE* file = fopen(DATABASE_USER_INFO, "r");
    FILE* fileb = fopen(DATABASE_BAN_LIST, "r");
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

## 3. 主要功能
### 3.1 服务端
 1. [x] 存储用户信息
 2. [x] 用户登录注册验证
 3. [x] 消息中转分发
 4. [x] 用户权限管理
 5. [x] 指令控制 
 6. [ ] 消息存储
### 3.2 客户端
1. [x] 连接指定服务器
2. [x] 登录注册
3. [x] 查看在线用户
4. [x] 发送接收信息
5. [ ] 头像显示

## 4. 逻辑思想
### 4.1 注册
<details><summary>点击查看图片</summary>

![注册](.\\Src\\注册.drawio.png)
</details>

### 4.2 登录
<details><summary>点击查看图片</summary>

![登录](.\\Src\\登录.drawio.png)
</details>

### 4.3 聊天
<details><summary>点击查看图片</summary>

>>>>>>>>> Temporary merge branch 2
