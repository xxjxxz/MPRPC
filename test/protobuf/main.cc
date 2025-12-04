#include "test.pb.h"
#include <iostream>

int main1() {
  fixbug::LoginRequest req;
  req.set_name("a");
  req.set_pwd("123");
  std::string send_str;
  if (req.SerializeToString(&send_str)) {
    std::cout << send_str.c_str() << std::endl;
  }
  fixbug::LoginRequest req1;
  if (req1.ParseFromString(send_str)) {
    std::cout << req1.name() << std::endl;
    std::cout << req1.pwd() << std::endl;
  }
  return 0;
}

int main() {
  //   fixbug::LoginResponse rsp;
  //   fixbug::Result *rs = rsp.mutable_result();
  //   rs->set_errcode(1);
  //   rs->set_errmsg("登录错误");

  fixbug::GetFriendListsResponse rsp2;
  fixbug::Result *rs = rsp2.mutable_result();
  rs->set_errcode(0);
  //   rs->set_errmsg("");
  fixbug::Usr *usr1 = rsp2.add_friendlists();
  usr1->set_age(11);
  usr1->set_name("aaa");
  usr1->set_sex(fixbug::Usr_Sex_MAN);

  fixbug::Usr *usr2 = rsp2.add_friendlists();
  usr2->set_age(22);
  usr2->set_name("bbb");
  usr2->set_sex(fixbug::Usr_Sex_WOMAN);
  std::cout << rsp2.friendlists_size() << std::endl;
}