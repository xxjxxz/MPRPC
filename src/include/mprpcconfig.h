#pragma once

// 读取配置文件类

#include <string>
#include <unordered_map>
class MprpcConfig {
public:
  void LoadConfigFile(const char *config_file);
  std::string Load(const std::string &key);
  void trim(std::string &src_buf);

private:
  std::unordered_map<std::string, std::string> m_configMap_;
};