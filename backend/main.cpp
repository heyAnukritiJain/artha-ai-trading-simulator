#include "crow.h"
#include <mysql/mysql.h>
#include <sstream>

void enable_cors(crow::response &res)
{
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

void add_cors(crow::response& res)
{
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main()
{
    crow::SimpleApp app;

    // ---------- STOCKS API ----------
    CROW_ROUTE(app, "/stocks")
    ([](){

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                           "artha_ai",3306,NULL,0))
        {
            return crow::response(500,"Database connection failed");
        }

        mysql_query(conn,"SELECT stock_symbol,current_price FROM Market_Prices");

        MYSQL_RES *result = mysql_store_result(conn);

        crow::json::wvalue stocks;

        int i = 0;

        MYSQL_ROW row;

        while((row=mysql_fetch_row(result)))
        {
            stocks[i]["symbol"] = row[0];
            stocks[i]["price"] = atof(row[1]);
            i++;
        }

        mysql_free_result(result);
        mysql_close(conn);

        crow::response res(stocks);
        enable_cors(res);
        return res;
    });


    // ---------- PORTFOLIO API ----------
    CROW_ROUTE(app,"/portfolio")
    ([](){

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                               "artha_ai",3306,NULL,0))
        {
            return crow::response(500,"Database connection failed");
        }

        mysql_query(conn,"SELECT stock_symbol,quantity FROM Portfolio");

        MYSQL_RES *result = mysql_store_result(conn);

        if(!result)
        {
            mysql_close(conn);
            return crow::response("No portfolio");
        }

        MYSQL_ROW row;
        std::stringstream output;

        while((row=mysql_fetch_row(result)))
        {
            output << row[0] << " : " << row[1] << " shares\n";
        }

        mysql_free_result(result);
        mysql_close(conn);

        crow::response res(output.str());
        enable_cors(res);
        return res;
    });


    // ---------- TRADES API ----------
    CROW_ROUTE(app,"/trades")
    ([](){

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                               "artha_ai",3306,NULL,0))
        {
            return crow::response(500,"Database connection failed");
        }

        mysql_query(conn,"SELECT symbol,type,price,quantity FROM Trades");

        MYSQL_RES *result = mysql_store_result(conn);

        if(!result)
        {
            mysql_close(conn);
            return crow::response("No trades");
        }

        MYSQL_ROW row;
        std::stringstream output;

        while((row=mysql_fetch_row(result)))
        {
            output << row[0] << " | "
                   << row[1] << " | "
                   << row[2] << " | "
                   << row[3] << " shares\n";
        }

        mysql_free_result(result);
        mysql_close(conn);

        crow::response res(output.str());
        enable_cors(res);
        return res;
    });


    // ---------- BUY API ----------
    CROW_ROUTE(app,"/buy").methods("POST"_method)
    ([](const crow::request& req){

        auto data = crow::json::load(req.body);

        if(!data)
            return crow::response(400,"Invalid JSON");

        std::string symbol = data["symbol"].s();
        int quantity = data["quantity"].i();

        MYSQL *conn = mysql_init(NULL);

        if(!mysql_real_connect(conn,"localhost","root","anu123",
                               "artha_ai",3306,NULL,0))
        {
            return crow::response(500,"Database connection failed");
        }

        std::string query =
        "SELECT current_price FROM Market_Prices WHERE stock_symbol='"+symbol+"'";

        mysql_query(conn,query.c_str());

        MYSQL_RES *result = mysql_store_result(conn);

        if(!result)
        {
            mysql_close(conn);
            return crow::response("Stock not found");
        }

        MYSQL_ROW row = mysql_fetch_row(result);

        double price = atof(row[0]);
        mysql_free_result(result);

        double total = price * quantity;

        std::stringstream trade;

        trade << "INSERT INTO Trades(symbol,price,quantity,type) VALUES('"
              << symbol << "',"
              << price << ","
              << quantity << ",'BUY')";

        mysql_query(conn,trade.str().c_str());

        std::stringstream portfolio;

        portfolio << "INSERT INTO Portfolio(stock_symbol,quantity) VALUES('"
                  << symbol << "',"
                  << quantity << ")";

        mysql_query(conn,portfolio.str().c_str());

        mysql_close(conn);

        crow::response res("Trade executed");
        enable_cors(res);
        return res;
    });


    // ---------- SERVER START ----------
    app.port(18080).concurrency(1).run();
}