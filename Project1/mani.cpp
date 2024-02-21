#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

using namespace sql;

class MySQLConnector {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

public:
    MySQLConnector(const char* host, const char* user, const char* password, const char* database, unsigned int port = 3306) {
        try {
            // Khởi tạo driver và kết nối đến MySQL Server
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(host, user, password);

            // Kiểm tra kết nối
            if (!con->isValid()) {
                std::cerr << "Không thể kết nối đến MySQL Server." << std::endl;
                delete con;
                exit(1);
            }

            // Chọn cơ sở dữ liệu
            con->setSchema(database);
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: " << e.what() << std::endl;
            exit(1);
        }
        catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            exit(1);
        }
    }

    ~MySQLConnector() {
        // Đóng kết nối
        delete con;
    }

    void createDatabase(const char* database) {
        try {
            // Tạo cơ sở dữ liệu mới
            sql::Statement* stmt = con->createStatement();
            stmt->execute("CREATE DATABASE IF NOT EXISTS " + std::string(database));
            delete stmt;
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: " << e.what() << std::endl;
        }
    }

    void createTable(const std::string& query) {
        try {
            // Tạo bảng mới
            sql::Statement* stmt = con->createStatement();
            stmt->execute(query);
            delete stmt;
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Exception: " << e.what() << std::endl;
        }
    }
};

int main() {
    // Thông tin đăng nhập MySQL
    const char* host = "localhost";
    const char* user = "root";
    const char* password = "";
    const char* database = "DATABASE_4";

    // Tạo đối tượng connector và kết nối đến cơ sở dữ liệu
    MySQLConnector connector(host, user, password, "");
    connector.createDatabase(database);

    // Tạo bảng "Table_devices"
    std::string createTableQuery1 = "CREATE TABLE IF NOT EXISTS Table_devices ("
        "id INT PRIMARY KEY AUTO_INCREMENT, "
        "name VARCHAR(255), "
        "owner_name VARCHAR(255), "
        "address VARCHAR(255), "
        "drone_type VARCHAR(255), "
        "year INT, "
        "manufacturing VARCHAR(255)"
        ")";
    connector.createTable(createTableQuery1);

    // Tạo bảng "Drone_data"
    std::string createTableQuery2 = "CREATE TABLE IF NOT EXISTS Drone_data ("
        "id INT PRIMARY KEY AUTO_INCREMENT, "
        "device_id INT, "
        "image_1 VARCHAR(255), "
        "image_2 VARCHAR(255), "
        "start_at DATETIME, "
        "end_at DATETIME, "
        "result VARCHAR(255)"
        ")";
    connector.createTable(createTableQuery2);

    std::cout << "Cơ sở dữ liệu và bảng đã được tạo thành công!" << std::endl;

    return 0;
}

