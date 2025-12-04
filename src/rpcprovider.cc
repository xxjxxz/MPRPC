#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "mprpcconfig.h"
#include "rpcheader.pb.h"
#include <cstdint>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <mymuduo/InetAddress.h>
#include <mymuduo/Logger.h>
#include <mymuduo/TcpServer.h>

/**
 * @brief 框架对外发布服务的接口
 * @param service 用户提供的服务对象
 *
 * 该方法将用户的服务对象进行注册，通过存储服务名称到服务的映射，
 * 实现服务的查找和调用。内部会提取服务的所有方法并建立映射关系。
 */
void RpcProvider::NotifyService(google::protobuf::Service *service) {
  // 创建服务信息对象，用于存储服务相关的描述信息
  ServiceInfo serviceInfo;

  // 获取服务的描述符，包含服务的元数据信息
  const google::protobuf::ServiceDescriptor *serviceDescrip =
      service->GetDescriptor();

  // 获取服务名称
  std::string serviceName = serviceDescrip->name();
  LOG_INFO("service name: %s", serviceName.c_str());
  // 获取服务中方法的数量
  int methodCnt = serviceDescrip->method_count();

  // 遍历所有方法，建立方法名称到方法描述符的映射
  for (int i = 0; i < methodCnt; ++i) {
    const google::protobuf::MethodDescriptor *pmethodDecrip =
        serviceDescrip->method(i);
    serviceInfo.m_methodMap.insert({pmethodDecrip->name(), pmethodDecrip});
    LOG_INFO("method name: %s \n", pmethodDecrip->name().c_str());
  }

  // 保存服务对象指针
  serviceInfo.m_service = service;

  // 将服务名称和服务信息保存到映射中
  m_serviceMap.insert({serviceName, serviceInfo});
}

/**
 * @brief 启动RPC服务
 *
 * 该方法负责初始化并启动RPC服务器。它从配置文件中读取服务地址和端口，
 * 创建TCP服务器，设置回调函数，配置线程池，并开始事件循环。
 */
void RpcProvider::Run() {
  // 从全局配置中获取RPC服务器的IP地址
  std::string ip =
      MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");

  // 从全局配置中获取RPC服务器的端口号并转换为uint16_t类型
  uint16_t port =
      stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

  // 创建网络地址对象
  InetAddress addr(port, ip);

  // 创建TCP服务器对象（修复拼写错误：TcpServe -> TcpServer）
  TcpServe server(&m_eventloop_, addr, "RpcProvider");

  // 绑定连接回调函数，处理客户端连接事件
  server.setConnectionCallback(
      std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));

  // 绑定消息回调函数，处理客户端消息
  server.setMessageCallback(
      std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  // 设置线程数量，用于处理客户端请求
  server.setThreadNum(4);

  // 启动TCP服务器
  server.start();

  // 启动事件循环，阻塞等待客户端连接
  m_eventloop_.loop();
}

/**
 * @brief 连接回调函数
 * @param conn TCP连接对象
 *
 * 该函数在客户端连接或断开时被调用
 */
void RpcProvider::onConnection(const TcpConnectionPtr &conn) {
  // 此处可添加连接建立或断开时的处理逻辑
  if (!conn->connected()) {
    conn->shutdown();
  }
}

/**
 * @brief 消息回调函数
 * @param conn TCP连接对象
 * @param buffer 缓冲区对象，包含接收到的数据
 * @param time 时间戳，表示消息到达的时间
 *
 * 该函数在接收到客户端消息时被调用，当前实现为空。
 * 实际使用时需要实现RPC请求的解析、处理和响应逻辑。
* header_size(4字节) + header_str + args_str
header_str: service_name method_name args_size 防止粘包
 */
void RpcProvider::onMessage(const TcpConnectionPtr &conn, Buffer *buffer,
                            Timestamp *timestamp) {
  // 此处需要实现：
  // 1. 解析RPC请求协议
  // 2. 查找对应的服务和方法
  // 3. 执行远程调用
  // 4. 封装并返回响应结果
  // 从buffer中读取字符流
  std::string recv_buf = buffer->retrieveAllAsString();

  // 读取header_size（前4个字节）
  uint32_t header_size;
  recv_buf.copy((char *)&header_size, 4, 0);

  std::string rpc_header_str = recv_buf.substr(4, header_size);

  std::string serviceName;
  std::string methodName;
  uint32_t argsSize;

  // 反序列化得到数据
  mprpc::RpcHeader rpcHeader;
  if (rpcHeader.ParseFromString(rpc_header_str)) {
    LOG_INFO("rpc header str parse success !");
    serviceName = rpcHeader.service_name();
    methodName = rpcHeader.method_name();
    argsSize = rpcHeader.args_size();
  } else {
    LOG_ERROR("rpc header str parse failure !");
    return;
  }
  std::string args_str = recv_buf.substr(4 + header_size, argsSize);
  LOG_INFO("service name: %s\n method name: %s\n ars size:%d\n ars str: %s\n",
           serviceName.c_str(), methodName.c_str(), argsSize, args_str.c_str());

  auto it = m_serviceMap.find(serviceName);
  if (it == m_serviceMap.end()) {
    LOG_ERROR("service:  %s not exist !", serviceName.c_str());
    return;
  }

  auto methodIt = it->second.m_methodMap.find(methodName);
  if (methodIt == it->second.m_methodMap.end()) {
    LOG_ERROR("method: %s not exist !", methodName.c_str());
    return;
  }

  google::protobuf::Service *service = it->second.m_service; // 服务对象
  const google::protobuf::MethodDescriptor *method =
      methodIt->second; // 服务对象的方法

  // 生成args参数 reques对象 response 对象
  google::protobuf::Message *request =
      service->GetRequestPrototype(method).New();

  if (!request->ParseFromString(args_str)) {
    LOG_ERROR("request parse args str error !");
    return;
  }

  google::protobuf::Message *response =
      service->GetResponsePrototype(method).New();

  google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider, const TcpConnectionPtr &,
                                    google::protobuf::Message *>(
          this, &RpcProvider::sendRpcResponse, conn, response);

  service->CallMethod(method, nullptr, request, response, done);
}

// closure的回调，本地方法处理完的结果进行序列化，再发送给客户端
void RpcProvider::sendRpcResponse(const TcpConnectionPtr &conn,
                                  google::protobuf::Message *response) {
  std::string response_str;
  // response 进行序列化
  if (response->SerializeToString(&response_str)) {
    // 序列化过程，通过网络发送回
    conn->send(response_str);
  } else {
    LOG_ERROR("response serialize to string failure ...");
    return;
  }
  conn->shutdown(); // 模拟http短链接服务， 返回后服务端主动断开
}
