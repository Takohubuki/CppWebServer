#ifndef _CONNECTION_POOL_

#include <vector>
#include <string>
#include <mariadb/mysql.h>
#include <../locker/locker.h>

/**
 * @class SQLConnectionPool
 * @brief connection pool of database.
 * using singleton to keep unique, 
*/
class SQLConnectionPool {
public:
    MYSQL *getConnection();
    bool releaseConnection(MYSQL *conn);
    int getFreeConn();
    void destroyConn();

    static SQLConnectionPool *getInstance();

    void init(std::string url, std::string user, std::string pwd, std::string db_name, int port, size_t max_conn);

    SQLConnectionPool() {}
    ~SQLConnectionPool() {}

private:
    size_t max_conn; /**< max number of connection*/
    size_t cur_conn; /**< number of currently used connection*/
    size_t free_conn;/**< number of currently free connection*/

    Locker locker;
    std::vector<MYSQL*> conn_pool; /**< connection pool*/
    Sem reserve;

    std::string url;        /**< host address*/
    std::string user;       /**< database user name*/
    std::string pwd;        /**< database user password*/
    std::string port;       /**< database connection port*/
    std::string db_name;    /**< databse name*/

};

class ConnectionRAII {
private:
    MYSQL* con_raii;
    SQLConnectionPool* pool_raii;
public:
    ConnectionRAII() {}
    ConnectionRAII(MYSQL **conn, SQLConnectionPool *conn_p);
    ~ConnectionRAII() {}
};
#endif // !_CONNECTION_POOL_