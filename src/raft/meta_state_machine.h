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

#ifndef DINGODB_META_STATE_MACHINE_H_
#define DINGODB_META_STATE_MACHINE_H_

#include "braft/raft.h"
#include "brpc/controller.h"
#include "common/context.h"
#include "engine/engine.h"
#include "proto/raft.pb.h"

namespace dingodb {

class MetaClosure : public braft::Closure {
 public:
  MetaClosure(std::shared_ptr<Context> ctx) : ctx_(ctx) {}
  ~MetaClosure() {}

  void Run();

  std::shared_ptr<Context> get_ctx() { return ctx_; }

 private:
  std::shared_ptr<Context> ctx_;
};

class MetaStateMachine : public braft::StateMachine {
 public:
  MetaStateMachine(std::shared_ptr<Engine> engine);

  void on_apply(braft::Iterator& iter);
  void on_shutdown();
  void on_snapshot_save(braft::SnapshotWriter* writer, braft::Closure* done);
  int on_snapshot_load(braft::SnapshotReader* reader);
  void on_leader_start();
  void on_leader_start(int64_t term);
  void on_leader_stop(const butil::Status& status);
  void on_error(const ::braft::Error& e);
  void on_configuration_committed(const ::braft::Configuration& conf);
  void on_start_following(const ::braft::LeaderChangeContext& ctx);
  void on_stop_following(const ::braft::LeaderChangeContext& ctx);

 private:
  void DispatchRequest(MetaClosure* done, const pb::raft::RaftCmdRequest& raft_cmd);
  void HandleCreateSchema(MetaClosure* done, const pb::raft::RaftCreateSchemaRequest& request);

 private:
  std::shared_ptr<Engine> engine_;
};

}  // namespace dingodb

#endif  // DINGODB_META_STATE_MACHINE_H_
