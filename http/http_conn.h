#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "../locker/locker.h"
#include "../CGIMySQL/sql_connection_pool.h"

class HttpConnection {
public:
    static const size_t FILENAME_LEN = 200;
    static const size_t READ_BUFFER_SIZE = 2048;
    static const size_t WRITE_BUFFER_SIZE = 1024;

    /***
     * http请求类型
    */
    enum METHOD {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };

    /***
     * 
    */
    enum CHECK_STATE {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    /***
     * http响应码
    */
    enum HTTP_CODE {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    enum LINE_STATUS {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    HttpConnection(){}
    ~HttpConnection(){}

public:
    void init(int sockfd, const sockaddr_in &addr);
    void close_conn(bool real_close = true);
    void process();
    bool read_once();
    bool write();
    constexpr sockaddr_in* get_address() {
        return &m_address;
    };
    void init_mysql_result(SQLConnectionPool *conn_p);

private:
    sockaddr_in m_address;
};

#endif