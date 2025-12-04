#include "../user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <iostream>

// rpc服务发布端
class UserService : public fixbug::UserServiceRpc {
public:
  bool Login(std::string name, std::string pwd) {
    std::cout << "doing local service login" << std::endl;
    std::cout << "name: " << name << " pwd:" << pwd << std::endl;
    return true;
  }

  // 重写基类的虚函数，实现业务功能
  void Login(::google::protobuf::RpcController *controller,
             const ::fixbug::LoginRequest *request,
             ::fixbug::LoginResponse *response,
             ::google::protobuf::Closure *done) override {
    std::string name = request->name();
    std::string pwd = request->pwd();
    bool result = Login(name, pwd);
    fixbug::ResultCode *rs = response->mutable_result();
    rs->set_errcode(0);
    rs->set_msg("");
    response->set_success(result);

    done->Run();
  }
};

int main(int argc, char **argv) {

  MprpcApplication::Init(argc, argv);
  RpcProvider rpcprovider;
  rpcprovider.NotifyService(new UserService());
  rpcprovider.Run();
  return 0;
}