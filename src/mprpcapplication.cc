#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include <cstdlib>
#include <getopt.h>
#include <mymuduo/Logger.h>
#include <unistd.h>

/**
 * @brief MprpcApplication类的静态成员变量定义
 *
 * 该静态成员变量用于存储全局配置信息，在整个应用程序生命周期内共享。
 */
MprpcConfig MprpcApplication::m_config_;

/**
 * @brief 初始化MPRPC应用程序
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 *
 * 该方法负责解析命令行参数，加载配置文件，并初始化应用程序的配置信息。
 * 使用getopt库进行命令行参数解析，支持-i选项指定配置文件路径。
 *
 * @note 如果参数不足或解析失败，程序将直接退出并显示错误信息。
 * @warning 该方法必须在应用程序启动时首先调用，否则配置信息无法正确加载。
 */
void MprpcApplication::Init(int argc, char **argv) {
  // 检查命令行参数数量，至少需要2个参数（程序名 + 选项）
  if (argc < 2) {
    LOG_ERROR("format: command -i <configfile>");
    exit(EXIT_FAILURE);
  }

  int c = 0;               // 存储当前解析的选项字符
  std::string config_file; // 配置文件路径

  // 使用getopt循环解析命令行参数，支持-i选项（需要参数）
  while ((c = getopt(argc, argv, "i:")) != -1) {
    switch (c) {
    case 'i':
      // 处理-i选项，获取配置文件路径
      config_file = optarg;
      break;
    case '?':
      // 处理未知选项错误
      //   LOG_ERROR("invalid args!");
      exit(EXIT_FAILURE);
    case ':':
      // 处理缺少选项参数错误
      //   LOG_ERROR("need <configfile> !");
      exit(EXIT_FAILURE);
    default:
      break;
    }
  }

  // 加载配置文件到配置对象中
  m_config_.LoadConfigFile(config_file.c_str());

  // 记录配置加载成功的日志信息，显示关键配置项
  LOG_INFO("load config file: rpcserver ip: %s \n"
           "rpcserver port: %s \n zookeeper ip: %s \n zookeeper port: %s",
           m_config_.Load("rpcserverip").c_str(),
           m_config_.Load("rpcserverport").c_str(),
           m_config_.Load("zookeeperip").c_str(),
           m_config_.Load("zookeeperport").c_str());
}

/**
 * @brief 获取MprpcApplication单例实例
 * @return MprpcApplication& 单例实例的引用
 *
 * 该方法实现单例模式，确保整个应用程序中只有一个MprpcApplication实例。
 * 使用局部静态变量实现线程安全的单例模式（C++11标准保证）。
 *
 * @note 该方法线程安全，可以在多线程环境下安全调用。
 * @see
 * https://en.cppreference.com/w/cpp/language/storage_duration#Static_local_variables
 */
MprpcApplication &MprpcApplication::GetInstance() {
  static MprpcApplication app; // 局部静态变量，首次调用时初始化
  return app;
}

/**
 * @brief 获取应用程序配置对象
 * @return MprpcConfig& 配置对象的引用
 *
 * 该方法提供对全局配置信息的访问接口，允许其他模块获取和使用配置数据。
 *
 * @note 返回的是配置对象的引用，可以直接修改配置信息（如果需要）。
 */
MprpcConfig &MprpcApplication::GetConfig() { return m_config_; }
