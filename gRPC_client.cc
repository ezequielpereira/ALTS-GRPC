#include <iostream>
#include <grpc++/grpc++.h>
#include "grpc/gcp/handshaker.pb.h"
#include "grpc/gcp/handshaker.grpc.pb.h"

int main() {
  auto creds = grpc::InsecureChannelCredentials();

  grpc::ChannelArguments ch_args;
  ch_args.SetMaxReceiveMessageSize(-1);

  auto channel = grpc::CreateCustomChannel("169.254.169.254:8080", creds, ch_args);

  auto stub = grpc::gcp::HandshakerService::NewStub(channel);

  grpc::gcp::HandshakerReq request;
  grpc::gcp::StartClientHandshakeReq *start_req = new grpc::gcp::StartClientHandshakeReq;
  start_req->set_handshake_security_protocol(grpc::gcp::HandshakeProtocol::ALTS);
  start_req->add_application_protocols("grpc");
  start_req->add_record_protocols("ALTSRP_GCM_AES128");
  request.set_allocated_client_start(start_req);

  grpc::gcp::HandshakerResp reply;

  grpc::ClientContext context;

  std::shared_ptr<grpc::ClientReaderWriter<grpc::gcp::HandshakerReq, grpc::gcp::HandshakerResp> > stream(stub->DoHandshake(&context));

  stream->Write(request);
  stream->WritesDone();

  while (stream->Read(&reply)) {
    std::cout << reply.DebugString() << std::endl;
  }

  grpc::Status status = stream->Finish();

  std::cout << std::endl;
  if (!status.ok()) {
    std::cout << "Error code " << status.error_code() << ": " << status.error_message() << std::endl;
    return -1;
  }

  return 0;
}
