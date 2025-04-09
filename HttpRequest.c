#include "HttpRequest.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include"HttpResponse.h"
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#define MAX 12
//初始化
HttpRequest* HttpRequestInit() {
	HttpRequest* httpRequest = (HttpRequest*)malloc(sizeof(HttpRequest));
	httpRequest->requestHeader = (RequestHeader*)malloc(sizeof(RequestHeader) * MAX);
	httpRequestReset(httpRequest);
	return httpRequest;
}
//重置
void httpRequestReset(HttpRequest* httpRequest) {
	httpRequest->state = parseReqLine;
	httpRequest->method = NULL;
	httpRequest->size = 0;
	httpRequest->url = NULL;
	httpRequest->version = NULL;
}
void httpRequestResetEx(HttpRequest* httpRequest) {
	free(httpRequest->method);
	free(httpRequest->version);
	free(httpRequest->url);
	if (httpRequest->requestHeader != NULL) {
		for (int i = 0; i < httpRequest->size; i++) {
			free(httpRequest->requestHeader[i].key);
			free(httpRequest->requestHeader[i].value);
		}
		free(httpRequest->requestHeader);
	}
	httpRequestReset(httpRequest);
}
//销毁
void httpRequestDestory(HttpRequest* httpRequest) {
	if (httpRequest == NULL) return;
	httpRequestResetEx(httpRequest);
	free(httpRequest);
}

//获取状态
enum HttpRequestState getHttpRequestState(HttpRequest* httpRequest) {
	return httpRequest->state;
}
//添加请求头
void httpRequestAddHeader(HttpRequest* httpRequest, char* key, char* value) {
	httpRequest->requestHeader[httpRequest->size].key = key;
	httpRequest->requestHeader[httpRequest->size].value = value;
	httpRequest->size++;
}

//根据KEY获取value
char* getHttpRequestHeader(HttpRequest* httpRequest, char* key) {
	for (int i = 0; i < httpRequest->size; i++) {
		if(strncasecmp(httpRequest->requestHeader[i].key,key,strlen(key))==0)
			if (httpRequest->requestHeader[i].key == key) {
				return httpRequest->requestHeader[i].value;
		}
	}
	return NULL;
}

//解析请求行
bool praseHttpReuqestLine(HttpRequest* httpRequest, Buffer* buffer) {
	//读出请求行
	char* end = bufferFindCRLF(buffer);//一行结尾地址
	char* begin = buffer->data + buffer->readPos;//起始地址
	int line = end - begin;//一行的长度
	if (line) {//长度不为0
		//获取method
		char* space = strstr(begin, " ");
		//char* space = memmem(begin,line," ",1);//找到空格的位置
		assert(space != NULL);
		int len = space - begin;
		httpRequest->method = (char*)malloc(len + 1);//动态分配长度
		strncpy(httpRequest->method, begin, len);
		httpRequest->method[len] = '\0';
		printf("method:%s\n", httpRequest->method);
		//获取url
		begin = space + 1;
		space = strstr(begin," ");//找到空格的位置
		assert(space != NULL);
		len = space - begin;
		httpRequest->url = (char*)malloc(len + 1);//动态分配长度
		strncpy(httpRequest->url, begin, len);
		httpRequest->url[len] = '\0';
		printf("url:%s\n", httpRequest->url);
		//获取版本
		begin = space + 1;
		len = end - begin;
		httpRequest->version = (char*)malloc(len + 1);//动态分配长度
		strncpy(httpRequest->version, begin, len);
		httpRequest->version[len] = '\0';
		printf("version:%s\n", httpRequest->version);
		buffer->readPos+= line+2;//移动到下一行
		httpRequest->state = parseReqHeadLine;//将状态变为请求头
		return true;
	}
	return false;
}

//解析请求头中的一行，也就是一个键值对
bool praseHttpRequestHeadLine(HttpRequest* httpRequest, Buffer* buffer) {
	char* end = bufferFindCRLF(buffer);//第一行结尾地址
	if (end == buffer->data+buffer->readPos) {//解析到空行 请求头已解析完成
		buffer->readPos += 2;//跳过空行
		httpRequest->state = parseReqDone;//对于get请求 ，此时已解析完成
		return false;
	}
	else {
		char* begin = buffer->data + buffer->readPos;
		int line = end - begin;
		char* space = strstr(begin, ": ");
		//char* space = memmem(begin, line, ": ", 2);//找到冒号和空格的位置
		if (space == NULL) return false;
		char* key = (char*)malloc(space - begin + 1);
		strncpy(key, begin, space - begin);
		key[space - begin] = '\0';
		begin = space + 2;
		char* value = (char*)malloc(end - begin + 1);
		strncpy(value, begin, end - begin);
		value[end - begin] = '\0';
		httpRequestAddHeader(httpRequest, key, value);//将键值对添加
		buffer->readPos += line + 2;
	}
	return true;
}

//解析HTTP请求  get请求
bool praseHttpRequest(HttpRequest* httpRequest, Buffer* buffer, HttpResponse* httpResponse, Buffer* sendBuffer, int socket) {
	bool flag = true; 
	flag=praseHttpReuqestLine(httpRequest, buffer);//获取请求行
	while (httpRequest->state== parseReqHeadLine &&praseHttpRequestHeadLine(httpRequest, buffer));//循环获取键值对
	if (httpRequest->state != parseReqDone) {
		flag = false;
		printf("httpRequest->state  is not parseReqDone\n");
	}
	else {//解析完成，准备回复数据
		//1.根据请求做出处理
		printf("ready response msg\n");
		processHttpRequest(httpRequest,httpResponse);
		//2.组织数据块回复
		httpResponsePrepareMsg(httpResponse, sendBuffer, socket);

	}
	httpRequest->state = parseReqLine;
	return flag;
}

//判断请求文件类型
const char* getFileType(const char* name) {
	const char* dot = strrchr(name, '.');//从右往左找'.'
	if (dot == NULL)
		return "text/plain;charset=utf-8";
	if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
		return "text/html;charset=utf-8";
	if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
		return "image/jpeg;";
	if (strcmp(dot, ".png") == 0)
		return "image/png;";
	if (strcmp(dot, ".css") == 0)
		return "text/css;";
	if (strcmp(dot, ".au") == 0)
		return "audio/basic;";
	if (strcmp(dot, ".wav") == 0)
		return "audio/wav;";
	if (strcmp(dot, ".js") == 0)
		return "text/javascript;";
	if (strcmp(dot, ".ico") == 0)
		return "image / x - icon;";
	if (strcmp(dot, ".tif") == 0 || strcmp(dot, ".tiff") == 0)
		return "image/tiff;";
	if (strcmp(dot, ".svg") == 0)
		return "text/xml;";
	if (strcmp(dot, ".woff") == 0)
		return "application/font-woff;";
	if (strcmp(dot, ".svg") == 0)
		return "image/svg+xml‌;";

	return "text/plain;charset=utf-8";

}

//发送文件
int sendFile(const char* filename, Buffer* sendBuffer, int cfd) {
	//打开文件
	int fd = open(filename, O_RDONLY);
	assert(fd > 0);
	while (1) {
		char buf[1024];
		int len = read(fd, buf, sizeof(buf));
		if (len > 0) {
			//printf("1..... len=%d\n", len);
			//send(cfd, buf, len, 0);//读到的数据直接发送到写缓冲区
			bufferAppendData(sendBuffer,buf,len);
			bufferSendData(sendBuffer, cfd);//先发送一部分
			usleep(10);//让接收端喘口气
		}
		else if (len == 0) {
			break;
		}
		else {
			close(fd);
			perror("read");
		}
	}
	printf("%s had been send\n", filename);
	close(cfd);
	close(fd);
	return 0;
}

//发送目录char* fileName,Buffer* sendBuffer,int socket
int sendDir(const char* dirName, Buffer* sendBuffer, int cfd) {
	char buf[4096] = { 0 };
	sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);
	struct dirent** namelist;//文件名通过下方函数传到namelist中
	int num = scandir(dirName, &namelist, NULL, alphasort);
	for (int i = 0; i < num; i++) {
		//取出文件名
		char* name = namelist[i]->d_name;
		struct stat st;
		char path[1024] = { 0 };
		sprintf(path, "%s/%s", dirName, name);
		stat(path, &st);
		if (S_ISDIR(st.st_mode)) {//判断是否为文件类型
			sprintf(buf + strlen(buf), "<tr><td><a href=\"%s/\">%s</a></td> <td>%ld</td></tr>", name, name, st.st_size);
		}
		else {//非目录
			sprintf(buf + strlen(buf), "<tr><td><a href=\"%s\">%s</a></td> <td>%ld</td></tr>", name, name, st.st_size);
		}
		bufferAppendString(sendBuffer, buf);//将内容写至写缓冲区
		bufferSendData(sendBuffer, cfd);//先发送一部分
		free(namelist[i]);
		memset(buf, 0, sizeof(buf));
	}
	sprintf(buf, "</table><body></html>");
	bufferAppendString(sendBuffer, buf);
	bufferSendData(sendBuffer, cfd);//先发送一部分
	free(namelist);
	return 0;
}


//处理HTTP请求
bool processHttpRequest(HttpRequest* httpRequest, HttpResponse* httpResponse) {
	if (strcasecmp(httpRequest->method, "get") != 0) {//只处理get请求
		return -1;
	}
	//处理静态资源(目录/文件)
	char* file = NULL;//相对路径
	if (strcmp(httpRequest->url, "/") == 0) {//只有根目录
		file = "./";
	}
	else {
		file = httpRequest->url + 1;
	}
	//获取文件属性
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1) {//文件不存在 回复404
		httpResponse->stateCode = 404;
		httpResponse->filename = "404.html";
		strcpy(httpResponse->statusMsg, "Not Found");
		//添加响应头
		httpResponseAddHeader(httpResponse,"Content-type", getFileType(".html"));
		httpResponse->sendFunc = sendFile;//函数指针
		//httpResponsePrepareMsg(HttpResponse * httpResponse, Buffer * sendBuffer, int socket, char* filename);
		//sendHeadMesg(cfd, 404, "not found", getFileType(".html"), -1);
		//sendfile("404.html", cfd);
		return -1;
	}
	httpResponse->stateCode = 200;
	httpResponse->filename = file;
	strcpy(httpResponse->statusMsg, "OK");
	if (S_ISDIR(st.st_mode)) {//资源为目录
		//添加响应头
		httpResponseAddHeader(httpResponse, "Content-type", getFileType(".html"));//目录按网页格式上传
		httpResponse->sendFunc = sendDir;//函数指针
	}
	else {//资源为文件
		//添加响应头
		char tmp[12] = {0};
		sprintf(tmp,"", st.st_size);
		printf("httpResponse filename:%s\n", httpResponse->filename);
		httpResponseAddHeader(httpResponse, "Content-type", getFileType(file));
		//httpResponseAddHeader(httpResponse, "Content-length", tmp);
		httpResponse->sendFunc = sendFile;//函数指针
	}
	return 0;
}