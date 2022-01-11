#ifndef server_h
#define server_h

#include <map>
#include <string>
#include <thread>

class Server {
 public:
  Server();
  ~Server(){};

  int StartServer();
  int StopServer();

 private:
  void HandleConnection(int fd);
  void HandleCommunication(int fd);
  int IsPartnerOnline(std::string name);
  const int port = 1234;
  std::map<std::string, std::string> known_pairs;
  std::map<int, std::string> name_fd_pairs;
  std::map<int, std::thread> threads;
};

#endif