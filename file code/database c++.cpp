#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace sql;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std;

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

void fetch_and_save_data(MySQLConnector& connector) {
    // Tạo HTTP client
    http_client client(U("https://jsonplaceholder.typicode.com"));

    // Gửi yêu cầu GET đến API
    client.request(methods::GET, U("/todos")).then([=](http_response response) {
        // Nếu kết nối thành công
        if (response.status_code() == status_codes::OK) {
            // Lấy nội dung JSON từ phản hồi
            return response.extract_json();
        }
        else {
            throw runtime_error("Failed to fetch data from API");
        }
        }).then([=, &connector](json::value data) {
            // Thêm dữ liệu vào cơ sở dữ liệu MySQL
            for (const auto& item : data.as_array()) {
                // Lấy các trường dữ liệu từ JSON
                int userId = item.at("userId").as_integer();
                int id = item.at("id").as_integer();
                string title = item.at("title").as_string();
                bool completed = item.at("completed").as_bool();

                // Thực hiện truy vấn SQL để chèn dữ liệu vào cơ sở dữ liệu
                sql::Statement* stmt = connector.con->createStatement();
                stmt->execute("INSERT INTO todos (userId, id, title, completed) VALUES (" + to_string(userId) + ", " + to_string(id) + ", '" + title + "', " + (completed ? "1" : "0") + ")");
                delete stmt;
            }
            std::cout << "Dữ liệu đã được lấy và lưu vào cơ sở dữ liệu thành công!" << std::endl;
            }).wait();
}

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

    fetch_and_save_data(connector);

    return 0;
}
