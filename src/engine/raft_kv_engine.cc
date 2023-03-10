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

#include "engine/raft_kv_engine.h"

#include <memory>

#include "braft/raft.h"
#include "butil/endpoint.h"
#include "common/helper.h"
#include "common/synchronization.h"
#include "config/config_manager.h"
#include "engine/write_data.h"
#include "proto/common.pb.h"
#include "proto/coordinator_internal.pb.h"
#include "proto/error.pb.h"
#include "proto/raft.pb.h"
#include "raft/meta_state_machine.h"
#include "raft/store_state_machine.h"
#include "server/server.h"

namespace dingodb {

RaftKvEngine::RaftKvEngine(std::shared_ptr<RawEngine> engine)
    : engine_(engine), raft_node_manager_(std::move(std::make_unique<RaftNodeManager>())) {}

RaftKvEngine::~RaftKvEngine() = default;

bool RaftKvEngine::Init(std::shared_ptr<Config> config) {
  LOG(INFO) << "Now=> Int Raft Kv Engine with config[" << config->ToString();
  return true;
}

// Recover raft node from region meta data.
// Invoke when server starting.
bool RaftKvEngine::Recover() {
  auto store_meta = Server::GetInstance()->GetStoreMetaManager();
  auto regions = store_meta->GetAllRegion();

  auto ctx = std::make_shared<Context>();
  for (auto& it : regions) {
    AddRegion(ctx, it.second);
  }

  return true;
}

std::string RaftKvEngine::GetName() { return pb::common::Engine_Name(pb::common::ENG_RAFT_STORE); }

pb::common::Engine RaftKvEngine::GetID() { return pb::common::ENG_RAFT_STORE; }

butil::Status RaftKvEngine::AddRegion(std::shared_ptr<Context> ctx, const std::shared_ptr<pb::common::Region> region) {
  LOG(INFO) << "RaftkvEngine add region, region_id " << region->id();

  // construct StoreStateMachine
  braft::StateMachine* state_machine = nullptr;
  state_machine = new StoreStateMachine(engine_);

  std::shared_ptr<RaftNode> node = std::make_shared<RaftNode>(
      ctx->ClusterRole(), region->id(), braft::PeerId(Server::GetInstance()->RaftEndpoint()), state_machine);

  if (node->Init(Helper::FormatPeers(Helper::ExtractLocations(region->peers()))) != 0) {
    node->Destroy();
    return butil::Status(pb::error::ERAFT_INIT, "Raft init failed");
  }

  raft_node_manager_->AddNode(region->id(), node);
  return butil::Status();
}

butil::Status RaftKvEngine::ChangeRegion([[maybe_unused]] std::shared_ptr<Context> ctx, uint64_t region_id,
                                         std::vector<pb::common::Peer> peers) {
  raft_node_manager_->GetNode(region_id)->ChangePeers(peers, nullptr);
  return butil::Status();
}

butil::Status RaftKvEngine::DestroyRegion([[maybe_unused]] std::shared_ptr<Context> ctx, uint64_t region_id) {
  auto node = raft_node_manager_->GetNode(region_id);
  if (node == nullptr) {
    return butil::Status(pb::error::ERAFT_NOTNODE, "Raft not node");
  }
  node->Shutdown(nullptr);
  node->Join();

  raft_node_manager_->DeleteNode(region_id);
  return butil::Status();
}

std::shared_ptr<pb::raft::RaftCmdRequest> genRaftCmdRequest(const std::shared_ptr<Context> ctx,
                                                            const WriteData& write_data) {
  std::shared_ptr<pb::raft::RaftCmdRequest> raft_cmd = std::make_shared<pb::raft::RaftCmdRequest>();

  pb::raft::RequestHeader* header = raft_cmd->mutable_header();
  header->set_region_id(ctx->RegionId());

  for (auto datum : write_data.Datums()) {
    auto requests = raft_cmd->mutable_requests();

    requests->AddAllocated(datum->TransformToRaft());
  }

  return raft_cmd;
}

butil::Status RaftKvEngine::Write(std::shared_ptr<Context> ctx, const WriteData& write_data) {
  auto node = raft_node_manager_->GetNode(ctx->RegionId());
  if (node == nullptr) {
    LOG(ERROR) << "Not found raft node " << ctx->RegionId();
    return butil::Status(pb::error::ERAFT_NOTNODE, "Not found node");
  }

  auto s = node->Commit(ctx, genRaftCmdRequest(ctx, write_data));
  if (!s.ok()) {
    return s;
  }

  ctx->EnableSyncMode();
  ctx->Cond()->IncreaseWait();
  LOG(INFO) << "Wake up";
  if (!ctx->Status().ok()) {
    return ctx->Status();
  }
  return butil::Status();
}

butil::Status RaftKvEngine::AsyncWrite(std::shared_ptr<Context> ctx, const WriteData& write_data, WriteCb_t cb) {
  LOG(INFO) << "here 0001";
  auto node = raft_node_manager_->GetNode(ctx->RegionId());
  if (node == nullptr) {
    LOG(ERROR) << "Not found raft node " << ctx->RegionId();
    return butil::Status(pb::error::ERAFT_NOTNODE, "Not found node");
  }
  LOG(INFO) << "here 0002";

  ctx->SetWriteCb(cb);
  return node->Commit(ctx, genRaftCmdRequest(ctx, write_data));
}

std::shared_ptr<Engine::Reader> RaftKvEngine::NewReader(const std::string& cf_name) {
  return std::make_shared<RaftKvEngine::Reader>(engine_->NewReader(cf_name));
}

butil::Status RaftKvEngine::Reader::KvGet(std::shared_ptr<Context> ctx, const std::string& key, std::string& value) {
  return reader_->KvGet(key, value);
}

butil::Status RaftKvEngine::Reader::KvScan(std::shared_ptr<Context> ctx, const std::string& start_key,
                                           const std::string& end_key, std::vector<pb::common::KeyValue>& kvs) {
  return reader_->KvScan(start_key, end_key, kvs);
}

butil::Status RaftKvEngine::Reader::KvCount(std::shared_ptr<Context> ctx, const std::string& start_key,
                                            const std::string& end_key, int64_t& count) {
  return reader_->KvCount(start_key, end_key, count);
}

}  // namespace dingodb
