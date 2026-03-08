# Artha-AI Trading Simulator

A real-time stock trading simulator built with **C++, Crow Web Framework, and MySQL**.
The platform allows users to view live market prices, execute buy/sell trades, track portfolio holdings, and view trade history through a web dashboard.

---

## Tech Stack

Backend

* C++
* Crow Web Framework
* REST API

Database

* MySQL

Frontend

* HTML
* CSS
* JavaScript

---

## Features

* Real-time stock market simulation
* Buy and sell stocks
* Portfolio tracking
* Trade history logging
* Live price updates
* RESTful API backend
* Interactive web dashboard

---

## Project Architecture

Frontend (HTML + JavaScript)
↓
REST API Server (C++ Crow)
↓
Trading Engine
↓
MySQL Database

---

## API Endpoints

GET /stocks → Get market prices
GET /portfolio → View user holdings
GET /trades → Trade history
GET /balance → User balance

POST /buy → Buy stock
POST /sell → Sell stock

---

## Database Schema

Users

* id
* username
* balance

Market_Prices

* stock_symbol
* current_price

Portfolio

* user_id
* stock_symbol
* quantity

Trades

* user_id
* symbol
* price
* quantity
* type
* timestamp

---

## How to Run

Clone repository

```
git clone <repo-url>
cd Artha-AI
```

Build project

```
mkdir build
cd build
cmake ..
make
```

Run server

```
./artha_ai
```

Open dashboard

```
frontend/index.html
```

---

## Future Improvements

* AI stock prediction
* user authentication
* Docker deployment
* websocket live market feed

---

## Screenshots

### Market Dashboard
![Dashboard](screenshots/dashboard)

### Portfolio
![Portfolio](screenshots/portfolio)

### Trade History
![Trades](screenshots/trades)
