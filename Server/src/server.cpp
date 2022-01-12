#include "server.h"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#define SA struct sockaddr

Server::Server() {
  known_pairs.emplace("Joel", "Eli");
  known_pairs.emplace("Eli", "Joel");
}

int Server::StartServer() {
  int sockfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    exit(0);
  } else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  } else
    printf("Server listening..\n");
  len = sizeof(cli);

  while (true) {
    int conn = accept(sockfd, (struct sockaddr*)&servaddr, &len);

    if (threads.find(conn) != threads.end()) {
      threads.erase(conn);
    }

    threads.emplace(conn, std::thread(&Server::HandleConnection, this, conn));
    threads.at(conn).detach();
    sleep(10);
  }

  //   for (auto& t : threads) {
  //     t.join();
  //   }

  return 0;
}

void Server::HandleConnection(int fd) {
  // Get Name of Incoming Connection
  char buff[100];
  int size = recv(fd, buff, 100, 0);

  if ((size == 0) || (size == -1)) {
    std::cout << "An error has occurred " << std::endl;
    close(fd);
    return;
  }

  std::string name = std::string(buff);

  if (!name.empty() && name[name.length() - 1] == '\n') {
    name.erase(name.length() - 1);
  }

  bool associated = false;

  for (auto pair : name_fd_pairs) {
    if (pair.second == name) {
      name_fd_pairs.erase(pair.first);
      close(pair.first);
      // threads.at(pair.first).detach();
      name_fd_pairs.emplace(fd, name);
      std::cout << "User " << name << " being reassociated with fd " << fd
                << std::endl;
      std::cout << "Closing fd " << pair.first << std::endl;
      associated = true;
    }
  }

  if (associated == false) {
    std::cout << "User " << name << " associated with fd " << fd << std::endl;
    name_fd_pairs.emplace(fd, name);
  }

  if (known_pairs.find(name) == known_pairs.end()) {
    std::cout << "User " << name << " not known" << std::endl;
    close(fd);
    return;
  } else {
    std::cout << name << " has pair " << known_pairs.at(name) << std::endl;
  }

  HandleCommunication(fd);
}

void Server::HandleCommunication(int fd) {
  // Get Name of Incoming Connection

  while (true) {
    char buff[3];
    int size = recv(fd, buff, 3, 0);

    std::string sender;
    std::string receiver;

    try {
      sender = name_fd_pairs.at(fd);
      receiver = known_pairs.at(sender);
    } catch (std::out_of_range exeption) {
      std::cout << "Fd " << fd << " is dying" << std::endl;
      close(fd);
      return;
    }

    if ((size == 0) || (size == -1)) {
      std::cout << sender << " has disconnected " << std::endl;

      if (name_fd_pairs.find(fd) != name_fd_pairs.end()) {
        name_fd_pairs.erase(fd);
      }

      close(fd);
      return;
    }

    if (strcmp(buff, "hi\n") == 0) {
      std::cout << sender << " is trying to say hi to " << receiver
                << std::endl;

      auto partner_fd = IsPartnerOnline(receiver);
      if (partner_fd != -1) {
        std::string message = sender + " says hi\n";
        send(partner_fd, message.c_str(), message.size(), 0);
        // std::cout << receiver << " is online\n";
      } else {
        std::cout << receiver << " is offline\n";
      }
    } else {
      std::cout << "Unknown command" << std::endl;
    }
  }
}

int Server::IsPartnerOnline(std::string name) {
  for (auto connected : name_fd_pairs) {
    if (connected.second == name) {
      return connected.first;
    }
  }
  return -1;
}