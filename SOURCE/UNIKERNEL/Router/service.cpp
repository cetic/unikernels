/*
 * This code is copied from the IncludeOS Router example.
 */

#include <service>
#include <net/router>

void Service::start()
{
  auto& router = net::get_router();

  auto& eth0 = net::Super_stack::get<net::IP4>(0);
  auto& eth1 = net::Super_stack::get<net::IP4>(1);

  eth0.set_forward_delg(router.forward_delg());
  eth1.set_forward_delg(router.forward_delg());
}
