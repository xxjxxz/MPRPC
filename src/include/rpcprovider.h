#pragma once

#include <google/protobuf/service.h>

// 服务发布的网络对象类
class RpcProvider {
public:
  // 发布rpc服务
  void NotifyService(google::protobuf::Service *service);
  void Run();

private:
};