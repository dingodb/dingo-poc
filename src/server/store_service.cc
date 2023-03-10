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

#include "server/store_service.h"

#include "common/constant.h"
#include "common/context.h"
#include "common/helper.h"
#include "meta/store_meta_manager.h"
#include "proto/common.pb.h"
#include "server/server.h"

namespace dingodb {

StoreServiceImpl::StoreServiceImpl() = default;

void StoreServiceImpl::AddRegion(google::protobuf::RpcController* controller,
                                 const dingodb::pb::store::AddRegionRequest* request,
                                 dingodb::pb::store::AddRegionResponse* response, google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "AddRegion request...";

  auto store_control = Server::GetInstance()->GetStoreControl();

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done);
  auto status = store_control->AddRegion(ctx, std::make_shared<pb::common::Region>(request->region()));
  if (!status.ok()) {
    auto* mut_err = response->mutable_error();
    mut_err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    mut_err->set_errmsg(status.error_str());
  }
}

void StoreServiceImpl::ChangeRegion(google::protobuf::RpcController* controller,
                                    const pb::store::ChangeRegionRequest* request,
                                    pb::store::ChangeRegionResponse* response, google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "ChangeRegion request...";

  auto store_control = Server::GetInstance()->GetStoreControl();
  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done);
  auto status = store_control->ChangeRegion(ctx, std::make_shared<pb::common::Region>(request->region()));
  if (!status.ok()) {
    auto* mut_err = response->mutable_error();
    mut_err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    mut_err->set_errmsg(status.error_str());
  }
}

void StoreServiceImpl::DestroyRegion(google::protobuf::RpcController* controller,
                                     const dingodb::pb::store::DestroyRegionRequest* request,
                                     dingodb::pb::store::DestroyRegionResponse* response,
                                     google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "DestroyRegion request...";

  auto store_control = Server::GetInstance()->GetStoreControl();
  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done);
  auto status = store_control->DeleteRegion(ctx, request->region_id());
  if (!status.ok()) {
    auto* mut_err = response->mutable_error();
    mut_err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    mut_err->set_errmsg(status.error_str());
  }
}

butil::Status ValidateKvGetRequest(const dingodb::pb::store::KvGetRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  if (request->key().empty()) {
    return butil::Status(pb::error::EKEY_EMPTY, "Key is empty");
  }

  return butil::Status();
}

void StoreServiceImpl::KvGet(google::protobuf::RpcController* controller,
                             const dingodb::pb::store::KvGetRequest* request,
                             dingodb::pb::store::KvGetResponse* response, google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "KvGet request: " << request->key();

  butil::Status status = ValidateKvGetRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  LOG(INFO) << "address: " << (void*)request->key().data();

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);
  std::vector<std::string> keys;
  auto mut_request = const_cast<dingodb::pb::store::KvGetRequest*>(request);
  keys.emplace_back(std::move(*mut_request->release_key()));
  LOG(INFO) << "address: " << (void*)(keys.begin()->data());

  std::vector<pb::common::KeyValue> kvs;
  status = storage_->KvGet(ctx, keys, kvs);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }
  if (kvs.size() > 0) {
    response->set_value(kvs[0].value());
  }
}

butil::Status ValidateKvBatchGetRequest(const dingodb::pb::store::KvBatchGetRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  for (const auto& key : request->keys()) {
    if (key.empty()) {
      return butil::Status(pb::error::EKEY_EMPTY, "Key is empty");
    }
  }

  return butil::Status();
}

void StoreServiceImpl::KvBatchGet(google::protobuf::RpcController* controller,
                                  const pb::store::KvBatchGetRequest* request, pb::store::KvBatchGetResponse* response,
                                  google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "KvBatchGet request";

  butil::Status status = ValidateKvBatchGetRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);

  std::vector<pb::common::KeyValue> kvs;
  auto mut_request = const_cast<dingodb::pb::store::KvBatchGetRequest*>(request);
  status = storage_->KvGet(ctx, Helper::PbRepeatedToVector(mut_request->mutable_keys()), kvs);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  Helper::VectorToPbRepeated(kvs, response->mutable_kvs());
}

butil::Status ValidateKvPutRequest(const dingodb::pb::store::KvPutRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  if (request->kv().key().empty()) {
    return butil::Status(pb::error::EKEY_EMPTY, "Key is empty");
  }

  return butil::Status();
}

void StoreServiceImpl::KvPut(google::protobuf::RpcController* controller,
                             const dingodb::pb::store::KvPutRequest* request,
                             dingodb::pb::store::KvPutResponse* response, google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "KvPut request: " << request->kv().key();

  butil::Status status = ValidateKvPutRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done_guard.release(), response);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);
  auto mut_request = const_cast<dingodb::pb::store::KvPutRequest*>(request);
  std::vector<pb::common::KeyValue> kvs;
  kvs.emplace_back(std::move(*mut_request->release_kv()));
  status = storage_->KvPut(ctx, kvs);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    brpc::ClosureGuard done_guard(done);
  }
}

butil::Status ValidateKvBatchPutRequest(const dingodb::pb::store::KvBatchPutRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  for (const auto& kv : request->kvs()) {
    if (kv.key().empty()) {
      return butil::Status(pb::error::EKEY_EMPTY, "key is empty");
    }
  }

  return butil::Status();
}

void StoreServiceImpl::KvBatchPut(google::protobuf::RpcController* controller,
                                  const pb::store::KvBatchPutRequest* request, pb::store::KvBatchPutResponse* response,
                                  google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);

  butil::Status status = ValidateKvBatchPutRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done_guard.release(), response);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);
  auto mut_request = const_cast<dingodb::pb::store::KvBatchPutRequest*>(request);
  status = storage_->KvPut(ctx, Helper::PbRepeatedToVector(mut_request->mutable_kvs()));
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    brpc::ClosureGuard done_guard(done);
  }
}

butil::Status ValidateKvPutIfAbsentRequest(const dingodb::pb::store::KvPutIfAbsentRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  if (request->kv().key().empty()) {
    return butil::Status(pb::error::EKEY_EMPTY, "Key is empty");
  }

  return butil::Status();
}

void StoreServiceImpl::KvPutIfAbsent(google::protobuf::RpcController* controller,
                                     const pb::store::KvPutIfAbsentRequest* request,
                                     pb::store::KvPutIfAbsentResponse* response, google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "KvPutIfAbsent request: ";
  butil::Status status = ValidateKvPutIfAbsentRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done_guard.release(), response);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);
  auto mut_request = const_cast<dingodb::pb::store::KvPutIfAbsentRequest*>(request);
  std::vector<pb::common::KeyValue> kvs;
  kvs.emplace_back(std::move(*mut_request->release_kv()));
  status = storage_->KvPutIfAbsent(ctx, kvs);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    brpc::ClosureGuard done_guard(done);
  }
}

butil::Status ValidateKvBatchPutIfAbsentRequest(const dingodb::pb::store::KvBatchPutIfAbsentRequest* request) {
  // Check is exist region.
  if (!Server::GetInstance()->GetStoreMetaManager()->IsExistRegion(request->region_id())) {
    return butil::Status(pb::error::EREGION_NOT_FOUND, "Not found region");
  }

  for (const auto& kv : request->kvs()) {
    if (kv.key().empty()) {
      return butil::Status(pb::error::EKEY_EMPTY, "Key is empty");
    }
  }

  return butil::Status();
}

void StoreServiceImpl::KvBatchPutIfAbsent(google::protobuf::RpcController* controller,
                                          const pb::store::KvBatchPutIfAbsentRequest* request,
                                          pb::store::KvBatchPutIfAbsentResponse* response,
                                          google::protobuf::Closure* done) {
  brpc::Controller* cntl = (brpc::Controller*)controller;
  brpc::ClosureGuard done_guard(done);
  LOG(INFO) << "KvBatchPutIfAbsent request: ";

  butil::Status status = ValidateKvBatchPutIfAbsentRequest(request);
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    return;
  }

  std::shared_ptr<Context> ctx = std::make_shared<Context>(cntl, done_guard.release(), response);
  ctx->SetRegionId(request->region_id()).SetCfName(Constant::kStoreDataCF);

  auto mut_request = const_cast<dingodb::pb::store::KvBatchPutIfAbsentRequest*>(request);
  status = storage_->KvPutIfAbsent(ctx, Helper::PbRepeatedToVector(mut_request->mutable_kvs()));
  if (!status.ok()) {
    auto* err = response->mutable_error();
    err->set_errcode(static_cast<pb::error::Errno>(status.error_code()));
    err->set_errmsg(status.error_str());
    brpc::ClosureGuard done_guard(done);
  }
}

void StoreServiceImpl::set_storage(std::shared_ptr<Storage> storage) { storage_ = storage; }

}  // namespace dingodb
