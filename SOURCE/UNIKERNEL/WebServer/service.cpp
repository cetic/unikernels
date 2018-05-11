/*
 * This code is adapted from the IncludeOS Acorn web server example.
 */

#include <service>
#include <net/inet4>
#include <net/http/server.hpp>
#include <memdisk>
std::unique_ptr<http::Server> server;

void Service::start()
{
  // Retreive the stack (configured from outside)
  auto& inet = net::Inet4::stack<0>();
  Expects(inet.is_configured());

  // Init the memdisk
  auto& disk = fs::memdisk();
  disk.init_fs([] (auto err, auto&) {
    Expects(not err);
  });
  // Retreive the HTML page from the disk
  auto file = disk.fs().read_file("/index.html");
  Expects(file.is_valid());
  net::tcp::buffer_t html(
      new std::vector<uint8_t> (file.data(), file.data() + file.size()));

  // Create a HTTP Server and setup request handling
  server = std::make_unique<http::Server>(inet.tcp());
  server->on_request([html] (auto req, auto rw)
  {
    // We only support get
    if(req->method() != http::GET) {
      rw->write_header(http::Not_Found);
      return;
    }
    // Serve HTML on /
    if(req->uri() == "/") {
      rw->write(html);
    } else {
      rw->write_header(http::Not_Found);
    }
  });

  // Start listening on port 80
  server->listen(80);
}
