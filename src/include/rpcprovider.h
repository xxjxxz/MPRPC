#pragma once

#include "mymuduo/TcpServer.h"
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <mymuduo/EventLoop.h>
#include <unordered_map>

// 服务发布的网络对象类
class RpcProvider {
public:
  // 发布rpc服务
  void NotifyService(google::protobuf::Service *service);
  void Run();

private:
  // std::unique_ptr<TcpServe> m_tcpServerPtr_;
  void onConnection(const TcpConnectionPtr &);
  void onMessage(const TcpConnectionPtr &, Buffer *, Timestamp *);
  void sendRpcResponse(const TcpConnectionPtr &, google::protobuf::Message *);

  // 网络服务主线程
  EventLoop m_eventloop_;

  // 保存服务对象信息
  struct ServiceInfo {
    google::protobuf::Service *m_service; // 服务对象
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor *>
        m_methodMap; // 方法映射表
  };

  // 保存所有注册成功服务的映射信息
  std::unordered_map<std::string, ServiceInfo> m_serviceMap;
};