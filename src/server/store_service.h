// Copyright (c) 2023 dingodb.com, Inc. All Rights Reserved
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DINGODB_STORE_SERVICE_H_
#define DINGODB_STORE_SERVICE_H_

#include "brpc/controller.h"
#include "brpc/server.h"
#include "engine/storage.h"
#include "proto/store.pb.h"

namespace dingodb {

class StoreServiceImpl : public pb::store::StoreService {
 public:
  StoreServiceImpl();

  void AddRegion(google::protobuf::RpcController* controller, const pb::store::AddRegionRequest* request,
                 pb::store::AddRegionResponse* response, google::protobuf::Closure* done);

  void ChangeRegion(google::protobuf::RpcController* controller, const pb::store::ChangeRegionRequest* request,
                    pb::store::ChangeRegionResponse* response, google::protobuf::Closure* done);

  void DestroyRegion(google::protobuf::RpcController* controller, const pb::store::DestroyRegionRequest* request,
                     pb::store::DestroyRegionResponse* response, google::protobuf::Closure* done);

  void KvGet(google::protobuf::RpcController* controller, const pb::store::KvGetRequest* request,
             pb::store::KvGetResponse* response, google::protobuf::Closure* done);

  void KvBatchGet(google::protobuf::RpcController* controller, const pb::store::KvBatchGetRequest* request,
                  pb::store::KvBatchGetResponse* response, google::protobuf::Closure* done);

  void KvPut(google::protobuf::RpcController* controller, const pb::store::KvPutRequest* request,
             pb::store::KvPutResponse* response, google::protobuf::Closure* done);

  void KvBatchPut(google::protobuf::RpcController* controller, const pb::store::KvBatchPutRequest* request,
                  pb::store::KvBatchPutResponse* response, google::protobuf::Closure* done);

  void KvPutIfAbsent(google::protobuf::RpcController* controller, const pb::store::KvPutIfAbsentRequest* request,
                     pb::store::KvPutIfAbsentResponse* response, google::protobuf::Closure* done);

  void KvBatchPutIfAbsent(google::protobuf::RpcController* controller,
                          const pb::store::KvBatchPutIfAbsentRequest* request,
                          pb::store::KvBatchPutIfAbsentResponse* response, google::protobuf::Closure* done);

  void set_storage(std::shared_ptr<Storage> storage);

 private:
  std::shared_ptr<Storage> storage_;
};

}  // namespace dingodb

#endif  // DINGODB_STORE_SERVICE_H_
