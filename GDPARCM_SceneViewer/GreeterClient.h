#pragma once
#include <string>
#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GreeterClient
{
public:
	GreeterClient(std::shared_ptr<grpc::ChannelInterface> channel);
	std::string SayHello(const std::string& user);
	std::string SayHelloAgain(const std::string& user);

	static void runClient(const std::string& clientNo, const std::string& user, const std::string& user2);

private:
	std::unique_ptr<Greeter::Stub> stub_;
};