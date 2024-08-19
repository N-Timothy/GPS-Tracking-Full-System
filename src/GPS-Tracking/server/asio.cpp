#include "GPS-Tracking/server/asio.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

namespace karlo {
namespace tcp {

using boost::asio::ip::tcp;
// Implementation of Session class
Session::Session(tcp::socket socket) : socket_(std::move(socket)) {}

void Session::start() { do_read(); }

void Session::do_read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(data_, max_length),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {

          std::cout << "Received data: " << std::string(data_, length)
                    << std::endl;
          do_write(length);
        }
      });
}

void Session::do_write(std::size_t length) {
  auto self(shared_from_this());
  boost::asio::async_write(
      socket_, boost::asio::buffer(data_, length),
      [this, self](boost::system::error_code ec, std::size_t) {
        if (!ec) {
          do_read();
        }
      });
}

// Implementation of ConnectionPool class
ConnectionPool::ConnectionPool(boost::asio::io_context &io_context,
                               std::size_t pool_size)
    : io_context_(io_context), pool_size_(pool_size) {
  for (std::size_t i = 0; i < pool_size_; ++i) {
    available_connections_.emplace(std::make_shared<tcp::socket>(io_context_));
  }
}

std::shared_ptr<tcp::socket> ConnectionPool::get_connection() {
  if (available_connections_.empty()) {
    return std::make_shared<tcp::socket>(io_context_);
  } else {
    auto conn = available_connections_.front();
    available_connections_.pop();
    return conn;
  }
}

void ConnectionPool::release_connection(std::shared_ptr<tcp::socket> conn) {
  available_connections_.push(conn);
}

// Implementation of Server class
Server::Server(boost::asio::io_context &io_context, short port,
               std::size_t pool_size)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      connection_pool_(io_context, pool_size) {
  do_accept();
}

void Server::do_accept() {
  auto conn = connection_pool_.get_connection();
  acceptor_.async_accept(*conn, [this, conn](boost::system::error_code ec) {
    if (!ec) {
      std::make_shared<Session>(std::move(*conn))->start();
      connection_pool_.release_connection(conn);
    }
    do_accept();
  });
}

} // namespace tcp
} // namespace karlo
