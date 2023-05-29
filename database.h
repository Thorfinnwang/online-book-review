#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/pooling.h>

using namespace sql;

//数据库类
class Database {
private:
    sql::ConnectionPool* pool;
public:
    Database(std::string host, std::string user, std::string password, std::string database);
    ~Database();
    bool registerUser(std::string username, std::string password, std::string email);
    bool loginUser(std::string username, std::string password);
    std::vector<std::map<std::string, std::string>> searchBooks(std::string keyword);
    bool addReview(int userId, int bookId, int rating, std::string comment);
    std::vector<std::map<std::string, std::string>> getReviews(int bookId);
    bool addLike(int userId, int reviewId);
    bool addComment(int userId, int reviewId, std::string content);
};

//构造函数
Database::Database(std::string host, std::string user, std::string password, std::string database) {
    sql::ConnectOptionsMap connection_properties;
    connection_properties["hostName"] = host;
    connection_properties["userName"] = user;
    connection_properties["password"] = password;
    connection_properties["schema"] = database;
    connection_properties["OPT_CONNECT_TIMEOUT"] = 30;

    this->pool = sql::PoolingOptions::getMYSQLConnectionPool(&connection_properties);
}

//析构函数
Database::~Database() {
    delete this->pool;
}

//注册
bool Database::registerUser(std::string username, std::string password, std::string email) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "INSERT INTO users (username, password, email) VALUES ('" + username + "', '" + password + "', '" + email + "')";
        stmt->executeUpdate(query);
        return true;
    } catch (SQLException &e) {
        std::cout << "Failed to register user: " << e.what() << std::endl;
        return false;
    }
}

//登陆
bool Database::loginUser(std::string username, std::string password) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'";
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
        return result->next();
    } catch (SQLException &e) {
        std::cout << "Failed to login user: " << e.what() << std::endl;
        return false;
    }
}

//搜索
std::vector<std::map<std::string, std::string>> Database::searchBooks(std::string keyword) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "SELECT * FROM books WHERE MATCH (title, author, description) AGAINST ('" + keyword + "' IN NATURAL LANGUAGE MODE)";
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
        std::vector<std::map<std::string, std::string>> books;
        while (result->next()) {
            std::map<std::string, std::string> book;
            book["id"] = result->getString("id");
            book["title"] = result->getString("title");
            book["author"] = result->getString("author");
            book["description"] = result->getString("description");
            book["image_url"] = result->getString("image_url");
            book["created_at"] = result->getString("created_at");
            book["updated_at"] = result->getString("updated_at");
            books.push_back(book);
        }
        return books;
    } catch (SQLException &e) {
        std::cout << "Failed to search books: " << e.what() << std::endl;
        return {};
    }
}

//加评论
bool Database::addReview(int userId, int bookId, int rating, std::string comment) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "INSERT INTO reviews (user_id, book_id, rating, comment) VALUES (" + std::to_string(userId) + ", " + std::to_string(bookId) + ", " + std::to_string(rating) + ", '" + comment + "')";
        stmt->executeUpdate(query);
        return true;
    } catch (SQLException &e) {
        std::cout << "Failed to add review: " << e.what() << std::endl;
        return false;
    }
}

//搜索评论，返回按最新创建顺序
std::vector<std::map<std::string, std::string>> Database::getReviews(int bookId) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "SELECT * FROM reviews WHERE book_id = " + std::to_string(bookId) + " ORDER BY created_at DESC";
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
        std::vector<std::map<std::string, std::string>> reviews;
        while (result->next()) {
            std::map<std::string, std::string> review;
            review["id"] = result->getString("id");
            review["user_id"] = result->getString("user_id");
            review["book_id"] = result->getString("book_id");
            review["rating"] = result->getString("rating");
            review["comment"] = result->getString("comment");
            review["created_at"] = result->getString("created_at");
            review["updated_at"] = result->getString("updated_at");
            reviews.push_back(review);
        }
        return reviews;
    } catch (SQLException &e) {
        std::cout << "Failed to get reviews: " << e.what() << std::endl;
        return {};
    }
}

//点赞
bool Database::addLike(int userId, int reviewId) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "INSERT INTO likes (user_id, review_id) VALUES (" + std::to_string(userId) + ", " + std::to_string(reviewId) + ")";
        stmt->executeUpdate(query);
        return true;
    } catch (SQLException &e) {
        std::cout << "Failed to add like: " << e.what() << std::endl;
        return false;
    }
}

//加评论
bool Database::addComment(int userId, int reviewId, std::string content) {
    try {
        std::unique_ptr<sql::Connection> con(this->pool->getConnection());
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string query = "INSERT INTO comments (user_id, review_id, content) VALUES (" + std::to_string(userId) + ", " + std::to_string(reviewId) + ", '" + content + "')";
        stmt->executeUpdate(query);
        return true;
    } catch (SQLException &e) {
        std::cout << "Failed to add comment: " << e.what() << std::endl;
        return false;
    }
}

