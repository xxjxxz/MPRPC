#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include <cstdlib>
#include <getopt.h>
#include <mymuduo/Logger.h>
#include <unistd.h>

MprpcConfig MprpcApplication::m_config_;

void MprpcApplication::Init(int argc, char **argv) {
  if (argc < 2) {
    LOG_ERROR("format: command -i <configfile>");
    exit(EXIT_FAILURE);
  }
  int c = 0;
  std::string config_file;
  while ((c = getopt(argc, argv, "i:")) != -1) {
    switch (c) {
    case 'i':
      config_file = optarg;
      break;
    case '?':
      //   LOG_ERROR("invalid args!");
      exit(EXIT_FAILURE);
    case ':':
      //   LOG_ERROR("need <configfile> !");
      exit(EXIT_FAILURE);
    default:
      break;
    }
  }
  m_config_.LoadConfigFile(config_file.c_str());
  LOG_INFO("load config file: rpcserver ip: %s \n"
           "rpcserver port: %s \n zookeeper ip: %s \n zookeeper port: %s",
           m_config_.Load("rpcserverip").c_str(),
           m_config_.Load("rpcserverport").c_str(),
           m_config_.Load("zookeeperip").c_str(),
           m_config_.Load("zookeeperport").c_str());
}

MprpcApplication &MprpcApplication::GetInstance() {
  static MprpcApplication app;
  return app;
}