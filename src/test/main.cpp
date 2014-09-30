
/* c++ headers */
#include <ctime>
#include <iostream>
#include <string>

/* boost headers */
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

static uint8_t swahello[10];
static uint8_t swacsv_data1[10];
static uint8_t swacsv_data2[12];
static uint8_t swacsv_end[6];

static bool swahello_send = false;

static std::string udp_addr = "127.0.0.1";
static uint16_t    udp_port = 9001;

int main()
{
  swahello[0] = 0x00;
  swahello[1] = 0x00;
  swahello[2] = 0x00;
  swahello[3] = 0x06;
  swahello[4] = 0x68;
  swahello[5] = 0x65;
  swahello[6] = 0x6C;
  swahello[7] = 0x6C;
  swahello[8] = 0x6F;
  swahello[9] = 0x00;

  swacsv_data1[0] = 0x01;
  swacsv_data1[1] = 0x00;
  swacsv_data1[2] = 0x00;
  swacsv_data1[3] = 0x06;
  swacsv_data1[4] = 0x11;
  swacsv_data1[5] = 0x6C;
  swacsv_data1[6] = 0x6C;
  swacsv_data1[7] = 0x20;
  swacsv_data1[8] = 0x6F;
  swacsv_data1[9] = 0x00;

  swacsv_data2[0] = 0x01;
  swacsv_data2[1] = 0x00;
  swacsv_data2[2] = 0x00;
  swacsv_data2[3] = 0x08;
  swacsv_data2[4] = 0x34;
  swacsv_data2[5] = 0x6F;
  swacsv_data2[6] = 0x6F;
  swacsv_data2[7] = 0x20;
  swacsv_data2[8] = 0x6F;
  swacsv_data2[9] = 0x6F;
  swacsv_data2[10] = 0x6F;
  swacsv_data2[11] = 0x00;

  swacsv_end[0] = 0x01;
  swacsv_end[1] = 0x00;
  swacsv_end[2] = 0x00;
  swacsv_end[3] = 0x01;
  swacsv_end[4] = 0xFF;
  swacsv_end[5] = 0x00;

  try
  {
    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(
        boost::asio::ip::address::from_string(udp_addr), udp_port));

    boost::array<char, 65535> recv_buf;
    udp::endpoint remote_endpoint;
    boost::system::error_code error;
    boost::system::error_code ignored_error;

    for (;;)
    {
      if (!swahello_send) {
        socket.receive_from(boost::asio::buffer(recv_buf),
            remote_endpoint, 0, error);

        std::cout << "receive" << std::endl;

        if (error && error != boost::asio::error::message_size)
          throw boost::system::system_error(error);

        socket.send_to(boost::asio::buffer(swahello),
            remote_endpoint, 0, ignored_error);
        usleep(1000*1000);
        swahello_send = true;

        std::cout << "send swahello" << std::endl;
      } else {
        swacsv_data1[4] = 198;
        swacsv_data2[4] = 199;
        socket.send_to(boost::asio::buffer(swacsv_data1),
            remote_endpoint, 0, ignored_error);
        usleep(1000);
        socket.send_to(boost::asio::buffer(swacsv_data2),
            remote_endpoint, 0, ignored_error);
        socket.send_to(boost::asio::buffer(swacsv_end),
            remote_endpoint, 0, ignored_error);
        for (int i = 1; i < 10; ++i) {
          swacsv_data1[4] = (i + 5) % 200;
          swacsv_data2[4] = (i + 15) % 200;
          socket.send_to(boost::asio::buffer(swacsv_data1),
              remote_endpoint, 0, ignored_error);
          usleep(1000 * 10);
          socket.send_to(boost::asio::buffer(swacsv_data2),
              remote_endpoint, 0, ignored_error);
          usleep(1000 * 20);
          socket.send_to(boost::asio::buffer(swacsv_end),
              remote_endpoint, 0, ignored_error);
          usleep(1000 * 250);
        }
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
