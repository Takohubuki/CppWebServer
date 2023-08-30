#include "sql_connection_pool.h"
#include <iostream>

SQLConnectionPool::SQLConnectionPool() {
    this->cur_conn = 0;
    this->free_conn = 0;
}

SQLConnectionPool* SQLConnectionPool::getInstance() {
    static SQLConnectionPool* conn_pool;
    return conn_pool;
}

/**
 * initialize connection pool
 * @param[in] url
 * @param[in] user
 * @param[in] pwd
 * @param[in] db_name
 * @param[in] port
 * @param[in] max_conn
*/
void SQLConnectionPool::init(std::string url, std::string user, std::string pwd, std::string db_name, int port, size_t max_conn) {
    this->url = url;
    this->user = user;
    this->pwd = pwd;
    this->db_name = db_name;
    this->port = port;
    // this->max_conn = max_conn;

    // lock to initial connection pool
    locker.lock();
    for (int i = 0; i < max_conn; i++) {
        MYSQL* conn = nullptr;
        
        // allcates or initializes a MYSQL obj suitable for mysql_real_connect()
        // if mysql_init() allocates a new obj, it will be freed when mysql_close() is called to close the connection.
        conn = mysql_init(conn);

        if (conn == nullptr) {
            std::cout << "Error: " << mysql_error(conn);
            exit(1);
        }

        // mysql_real_connect() attempts to establish a connection to a MySQL server running on host. Client programs must successfully connect to a server before executing any other API functions that require a valid MySQL connection handler structure.
        conn = mysql_real_connect(conn, url.c_str(), user.c_str(), pwd.c_str(), db_name.c_str(), port, nullptr, 0);

        if (conn == nullptr) {
            std::cout << "Error: " << mysql_error(conn);
            exit(1);
        }
        conn_pool.emplace_back(conn);
        ++free_conn;
    }

    reserve = Sem(free_conn);

    this->max_conn = free_conn;
    
    // initial complete, unlock
    locker.unlock();
}

// get a new connection fron connection pool
MYSQL* SQLConnectionPool::getConnection() {
    MYSQL* conn = nullptr;

    if (conn_pool.size() == 0) return nullptr;

    reserve.wait();

    locker.lock();

    // conn = conn_pool.front();
    // conn_pool.pop_front();
    conn = conn_pool.back();
    conn_pool.pop_back();

    free_conn--;
    cur_conn++;

    locker.unlock();
    return conn;
}

bool SQLConnectionPool::releaseConnection(MYSQL* conn) {
    if (conn == nullptr) return false;

    locker.lock();
    conn_pool.emplace_back(conn);
    free_conn++;
    cur_conn--;
    locker.unlock();

    reserve.post();
    return true;
}

void SQLConnectionPool::destroyConn() {
    locker.lock();

    if (conn_pool.size() > 0) {
        for (MYSQL* con: conn_pool) {
            mysql_close(con);
        }
        cur_conn = 0;
        free_conn = 0;

        conn_pool.clear();
    }

    locker.unlock();
}

int SQLConnectionPool::getFreeConn() { return free_conn; }

SQLConnectionPool::~SQLConnectionPool() {
    destroyConn();
}

ConnectionRAII::ConnectionRAII(MYSQL** SQL, SQLConnectionPool* conn) {
    *SQL = conn->getConnection();
    con_raii = *SQL;
    pool_raii = conn;
}

ConnectionRAII::~ConnectionRAII() {
    pool_raii->releaseConnection(con_raii);
}