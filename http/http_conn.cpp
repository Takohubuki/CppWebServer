#include "http_conn.h"

#define connfdLT
#define listenfdLT

const char *ok_200_title = "OK";
const char *error_400_title = "Bad request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to satsify";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server";
const char *error_404_title = "Not found";
const char *error_404_form = "The request file was not found on this server";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file";

const char* doc_root = "/somepath"; // TODO: need to be replaced

Locker m_lock;

void HttpConnection::init_mysql_result(SQLConnectionPool* conn) {
    
}