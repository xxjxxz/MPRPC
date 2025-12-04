#include "mprpcconfig.h"
#include <cstdio>
#include <cstdlib>
#include <mymuduo/Logger.h>
#include <string>

void MprpcConfig::LoadConfigFile(const char *config_file) {
  FILE *pf = fopen(config_file, "r");
  if (pf == nullptr) {
    LOG_ERROR("path not exist !");
    exit(EXIT_FAILURE);
  }
  while (!feof(pf)) {
    char buf[512] = {0};
    fgets(buf, 512, pf);
    std::string src_buf(buf);

    trim(src_buf); // 去空格

    if (src_buf[0] == '#' || src_buf.empty())
      continue;

    int idx = src_buf.find('=');
    if (idx == -1) {
      continue;
    }

    std::string key;
    std::string value;
    key = src_buf.substr(0, idx);
    trim(key);

    int endIdx = src_buf.find('\n', idx);

    value = src_buf.substr(idx + 1, endIdx - idx - 1);
    trim((value));

    m_configMap_.insert({key, value});
  }
}
std::string MprpcConfig::Load(const std::string &key) {
  auto it = m_configMap_.find(key);
  if (it == m_configMap_.end()) {
    return "";
  }
  return it->second;
}

void MprpcConfig::trim(std::string &s) {
  int idx = s.find_first_not_of(' ');
  if (idx != -1) {
    s = s.substr(idx, s.size() - idx);
  }
  idx = s.find_last_not_of(' ');
  if (idx != -1) {
    s = s.substr(0, idx + 1);
  }
}
