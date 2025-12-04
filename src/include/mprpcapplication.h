#pragma once
#include "mprpcconfig.h"
// mprpc 框架基础类
class MprpcApplication {
public:
  static void Init(int argc, char **argv);
  static MprpcApplication &GetInstance();

private:
  static MprpcConfig m_config_;
  MprpcApplication() {};
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;
};