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

#ifndef DINGODB_ENGINE_RAFT_KV_ENGINE_H_
#define DINGODB_ENGINE_RAFT_KV_ENGINE_H_

#include <memory>

#include "engine/engine.h"
#include "proto/error.pb.h"
#include "proto/store.pb.h"
#include "raft/raft_node_manager.h"

namespace dingodb {

class RaftKvEngine : public Engine {
 public:
  RaftKvEngine(std::shared_ptr<Engine> engine);
  ~RaftKvEngine();

  bool Init(std::shared_ptr<Config> config);

  std::string GetName();
  pb::common::Engine GetID();

  pb::error::Errno AddRegion(std::shared_ptr<Context> ctx,
                const std::shared_ptr<pb::common::Region> region);
  pb::error::Errno ChangeRegion(std::shared_ptr<Context> ctx, uint64_t region_id,
                   std::vector<pb::common::Peer> peers);
  pb::error::Errno DestroyRegion(std::shared_ptr<Context> ctx, uint64_t region_id);

  pb::error::Errno KvGet(std::shared_ptr<Context> ctx, const std::string& key,
                         std::string& value) override;
  pb::error::Errno KvBatchGet(std::shared_ptr<Context> ctx,
                              const std::vector<std::string>& keys,
                              std::vector<pb::common::KeyValue>& kvs) override;

  pb::error::Errno KvPut(std::shared_ptr<Context> ctx,
                         const pb::common::KeyValue& kv) override;
  pb::error::Errno KvBatchPut(
      std::shared_ptr<Context> ctx,
      const std::vector<pb::common::KeyValue>& kvs) override;

  pb::error::Errno KvPutIfAbsent(std::shared_ptr<Context> ctx,
                                 const pb::common::KeyValue& kv) override;
  pb::error::Errno KvBatchPutIfAbsent(
      std::shared_ptr<Context> ctx,
      const std::vector<pb::common::KeyValue>& kvs,
      std::vector<std::string>& put_keys) override;

  pb::error::Errno KvDeleteRange(std::shared_ptr<Context> ctx,
                                 const pb::common::Range& range) override;

 private:
  std::shared_ptr<Engine> engine_;
  std::unique_ptr<RaftNodeManager> raft_node_manager_;
};

}  // namespace dingodb

#endif  // DINGODB_ENGINE_RAFT_KV_ENGINE_H_H  // NOLINT
