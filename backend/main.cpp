#include "crow.h"
#include <mysql/mysql.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <queue>
#include <mutex>
#include <condition_variable>

struct TradeEvent
{
    std::string symbol;
    int quantity;
    int user_id;
};

std::queue<TradeEvent> tradeQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

void trade_processor()
{
    while(true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        queueCV.wait(lock, []{ return !tradeQueue.empty(); });

        TradeEvent trade = tradeQueue.front();
        tradeQueue.pop();

        lock.unlock();

        MYSQL *conn = mysql_init(NULL);

        if(mysql_real_connect(conn,"localhost","root","anu123",
                              "artha_ai",3306,NULL,0))
        {
            std::string query =
            "INSERT INTO Portfolio(user_id,stock_symbol,quantity) VALUES(" +
            std::to_string(trade.user_id) + ",'" +
            trade.symbol + "'," +
            std::to_string(trade.quantity) + ")";

            mysql_query(conn, query.c_str());

            mysql_close(conn);
        }
    }
}

void market_engine()
{
    while(true)
    {
        MYSQL *conn = mysql_init(NULL);

        if(mysql_real_connect(conn,"localhost","root","YOUR_PASSWORD",
                              "artha_ai",3306,NULL,0))
        {
            mysql_query(conn,"SELECT stock_symbol,current_price FROM Market_Prices");

            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;

            while((row=mysql_fetch_row(res)))
            {
                std::string symbol = row[0];
                double price = atof(row[1]);

                double change = (rand()%10 - 5);  // -5 to +5 movement

                double new_price = price + change;

                std::string update =
                "UPDATE Market_Prices SET current_price="+
                std::to_string(new_price)+
                " WHERE stock_symbol='"+symbol+"'";

                mysql_query(conn,update.c_str());
            }

            mysql_free_result(res);
            mysql_close(conn);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void update_prices()
{
    MYSQL *conn = mysql_init(NULL);

    mysql_real_connect(conn,"localhost","root","anu123",
                       "artha_ai",3306,NULL,0);

    mysql_query(conn,
    "UPDATE Market_Prices "
    "SET current_price=current_price + FLOOR(RAND()*20 - 10)");

    mysql_close(conn);
}

int main()
{

    std::thread tradeThread(trade_processor);
    tradeThread.detach();

    crow::SimpleApp app;

    std::thread marketThread(market_engine);
    marketThread.detach();

    // Home route
    CROW_ROUTE(app, "/")([](){
        return "Artha-AI Server Running";
    });

    // Stocks API
    CROW_ROUTE(app, "/stocks")([](){

        MYSQL *conn = mysql_init(NULL);

        if (!conn)
        {
            return crow::response(500, "MySQL initialization failed");
        }

        if (!mysql_real_connect(conn,
                                "localhost",
                                "root",
                                "anu123",
                                "artha_ai",
                                3306,
                                NULL,
                                0))
        {
            std::string error = mysql_error(conn);
            mysql_close(conn);
            return crow::response(500, "Database connection failed: " + error);
        }

        if (mysql_query(conn, "SELECT stock_symbol, current_price FROM Market_Prices"))
        {
            std::string error = mysql_error(conn);
            mysql_close(conn);
            return crow::response(500, "Query failed: " + error);
        }

        MYSQL_RES *res = mysql_store_result(conn);

        if (!res)
        {
            mysql_close(conn);
            return crow::response(500, "Failed to fetch results");
        }

        MYSQL_ROW row;

        crow::json::wvalue stocks;

        int index = 0;

        while ((row = mysql_fetch_row(res)))
        {  
            crow::json::wvalue stock;

            stock["symbol"] = row[0];
            stock["price"] = std::stod(row[1]);

            stocks[index++] = std::move(stock);
        }

        mysql_free_result(res);
        mysql_close(conn);

        return crow::response(stocks);
    });

    CROW_ROUTE(app, "/portfolio")
([](){

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                           "artha_ai",3306,NULL,0))
        {
        return crow::response(500,"Database connection failed");
        }

        mysql_query(conn,
        "SELECT stock_symbol, quantity FROM Portfolio WHERE user_id=1");

        MYSQL_RES *res = mysql_store_result(conn);
        MYSQL_ROW row;

        std::string result;

        while((row=mysql_fetch_row(res)))
        {
           result += std::string(row[0]) + " : "
                + std::string(row[1]) + " shares\n";
        }

        mysql_free_result(res);
        mysql_close(conn);

        crow::response response(result);
        response.add_header("Access-Control-Allow-Origin","*");

        return response;
    });

    CROW_ROUTE(app, "/trades")
([](){

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                           "artha_ai",3306,NULL,0))
        {
           return crow::response(500,"Database connection failed");
        }

        mysql_query(conn,
        "SELECT symbol,type,price,quantity FROM Trades ORDER BY id DESC");

        MYSQL_RES *res = mysql_store_result(conn);
        MYSQL_ROW row;

        std::string result;

        while((row=mysql_fetch_row(res)))
        {
            result += std::string(row[0]) + " | "
               + std::string(row[1]) + " | "
               + std::string(row[2]) + " | "
               + std::string(row[3]) + " shares\n";
        }

        mysql_free_result(res);
        mysql_close(conn);

        crow::response response(result);
        response.add_header("Access-Control-Allow-Origin","*");

        return response;
    });

    CROW_ROUTE(app, "/buy").methods("POST"_method)
    ([](const crow::request& req){

        auto data = crow::json::load(req.body);

        std::string symbol = data["symbol"].s();
        int quantity = data["quantity"].i();
        int user_id = 1;

        TradeEvent trade;

        trade.symbol = symbol;
        trade.quantity = quantity;
        trade.user_id = user_id;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tradeQueue.push(trade);
        }

        queueCV.notify_one();

        
        crow::response response("Trade queued");
        response.add_header("Access-Control-Allow-Origin","*");

        return response;

    });

    CROW_ROUTE(app,"/sell").methods("POST"_method)
([](const crow::request& req){

        auto data = crow::json::load(req.body);

        std::string symbol = data["symbol"].s();
        int quantity = data["quantity"].i();
        int user_id = 1;

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                           "artha_ai",3306,NULL,0))
           return crow::response(500,"DB connection failed");

    // check portfolio
        std::string q =
        "SELECT quantity FROM Portfolio WHERE user_id=1 AND stock_symbol='"+symbol+"'";

        mysql_query(conn,q.c_str());
        MYSQL_RES *res = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(res);

        if(!row)
            return crow::response(400,"Stock not owned");

        int owned = atoi(row[0]);

        if(owned < quantity)
            return crow::response(400,"Not enough shares");

        mysql_free_result(res);

    // get price
        std::string priceQuery =
        "SELECT current_price FROM Market_Prices WHERE stock_symbol='"+symbol+"'";

        mysql_query(conn,priceQuery.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);

        double price = atof(row[0]);
        double total = price * quantity;

        mysql_free_result(res);

    // add balance
        std::string balanceUpdate =
        "UPDATE Users SET balance=balance+"+std::to_string(total)+" WHERE id=1";

        mysql_query(conn,balanceUpdate.c_str());

    // reduce portfolio
        std::string portfolioUpdate =
        "UPDATE Portfolio SET quantity=quantity-"+std::to_string(quantity)+
        " WHERE user_id=1 AND stock_symbol='"+symbol+"'";

        mysql_query(conn,portfolioUpdate.c_str());

    // record trade
        std::string tradeInsert =
        "INSERT INTO Trades(user_id,symbol,price,quantity,type)"
        " VALUES(1,'"+symbol+"',"+std::to_string(price)+","+std::to_string(quantity)+",'SELL')";

        mysql_query(conn,tradeInsert.c_str());

        mysql_close(conn);

        return crow::response("Stock sold successfully");
    });

    CROW_ROUTE(app, "/<path>")
    .methods("OPTIONS"_method)
    ([](const crow::request& req, std::string path){

        crow::response response;

        response.add_header("Access-Control-Allow-Origin","*");
        response.add_header("Access-Control-Allow-Methods","GET, POST, OPTIONS");
        response.add_header("Access-Control-Allow-Headers","Content-Type");

        return response;
    });

    CROW_ROUTE(app,"/balance")([](){

        MYSQL *conn = mysql_init(NULL);

        mysql_real_connect(conn,"localhost","root","anu123",
                       "artha_ai",3306,NULL,0);

        mysql_query(conn,"SELECT balance FROM Users WHERE id=1");

        MYSQL_RES *res = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(res);

        std::string balance = row[0];

        mysql_free_result(res);
        mysql_close(conn);

        return crow::response(balance);
    });

    std::thread market([](){

        while(true)
        {
            update_prices();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

    });

market.detach();

    market.detach();

    app.port(18080).multithreaded().run();
}