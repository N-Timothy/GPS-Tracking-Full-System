#ifndef _GPS_TRACKING_SERVER_ASIO_HPP_
#define _GPS_TRACKING_SERVER_ASIO_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <vector>

namespace karlo {
namespace tcp {

using boost::asio::ip::tcp;

// Session class handles individual connections
class Session : public std::enable_shared_from_this<Session> {
public:
  explicit Session(tcp::socket socket);

  void start();

private:
  void do_read();
  void do_write();

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

// ConnectionPool class manages a pool of reusable connections
class ConnectionPool {
public:
  ConnectionPool(boost::asio::io_context &io_context, std::size_t pool_size);

  std::shared_ptr<tcp::socket> get_connection();
  void release_connection(std::shared_ptr<tcp::socket> conn);

private:
  boost::asio::io_context &io_context_;
  std::size_t pool_size_;
  std::queue<std::shared_ptr<tcp::socket>> available_connections_;
};

// Server class listens for incoming connections and manages sessions
class Server {
public:
  Server(boost::asio::io_context &io_context, short port,
         std::size_t pool_size);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  ConnectionPool connection_pool_;
};

} // namespace tcp
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_TCP_HPP_
