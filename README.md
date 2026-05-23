# Artha-AI Trading Simulator

A real-time stock trading simulator built with **C++, Crow Web Framework, and MySQL**.
The platform allows users to view live market prices, execute buy/sell trades, track portfolio holdings, and view trade history through a web dashboard.

---
# Overview

Artha-AI simulates a real-world stock trading environment where users can:

* View live (simulated) market prices
* Execute buy/sell orders with real-time balance updates
* Track portfolio holdings and P&L across sessions
* Review complete trade history with timestamps

The project was built to explore concurrent request handling in C++, low-latency database design, and RESTful API architecture in the context of financial systems — areas directly relevant to high-frequency trading infrastructure.
Key technical decisions:

* Chose Crow Framework for its lightweight, high-performance C++ HTTP server
* Used MySQL for ACID-compliant trade logging and portfolio state persistence
* Designed the REST API with stateless endpoints to support scalable concurrent access
* Built a dynamic JavaScript frontend with async API calls for real-time UI updates without page reloads

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

The backend processes all trade logic (balance validation, order execution, portfolio updates) before committing to the database, ensuring data consistency across concurrent sessions.

---
# Features

* Real-time market simulation — stock prices update dynamically
* Buy / Sell execution — validates balance, updates holdings atomically
* Portfolio tracking — live view of holdings, quantity, and current value
* P&L calculation — tracks profit/loss across sessions via persistent DB state
* Trade history — complete log of all transactions with price, quantity, type, and timestamp
* RESTful API backend — clean, stateless endpoints consumable by any client
* Interactive dashboard — single-page JS frontend with live data rendering

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

Prerequisites

* C++17 or higher
* MySQL Server
* CMake 3.10+
* Crow Framework (included in /Crow)

Clone repository

```
git clone https://github.com/heyAnukritiJain/artha-ai-trading-simulator.git
cd artha-ai-trading-simulator
```
Set up the database

```
mysql -u root -p

CREATE DATABASE artha_ai;
USE artha_ai;
-- Run the schema from database/schema.sql
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
# Server starts at http://localhost:8080
```

Open dashboard

```
Open frontend/index.html in your browser```
```
---

## Future Improvements

* AI stock prediction
* user authentication
* Docker deployment
* websocket live market feed

---
