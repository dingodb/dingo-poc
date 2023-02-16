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

#include "raft/raft_node.h"

#include "raft/state_machine.h"


namespace dingodb {

RaftNode::RaftNode(uint64_t node_id, braft::PeerId& peer_id, braft::StateMachine *fsm)
  : node_id_(node_id),
    node_(new braft::Node(std::to_string(node_id), peer_id)),
    fsm_(fsm) {
}

RaftNode::~RaftNode() {
  if (fsm_) {
    delete fsm_;
    fsm_ = nullptr;
  }
}

int RaftNode::Init() {
  braft::NodeOptions node_options;
  if (node_options.initial_conf.parse_from("127.0.0.1:8201:0,127.0.0.1:8202:0,127.0.0.1:8203:0") != 0) {
    LOG(ERROR) << "Fail to parse configuration";
    return -1;
  }
  node_options.election_timeout_ms = 10;
  node_options.fsm = fsm_;
  node_options.node_owns_fsm = false;
  node_options.snapshot_interval_s = 100000;
  std::string prefix = "local://prefix";
  node_options.log_uri = prefix + "/log";
  node_options.raft_meta_uri = prefix + "/raft_meta";
  node_options.snapshot_uri = prefix + "/snapshot";
  node_options.disable_cli = false;

  if (node_->init(node_options) != 0) {
    LOG(ERROR) << "Fail to init raft node";
    return -1;
  }

  return 0;
}

void RaftNode::Destroy() {

}

// Commit message to raft
void RaftNode::Commit(std::shared_ptr<Context> ctx, Message &msg) {
  StoreClosure* closure = new StoreClosure(ctx->get_cntl(), ctx->get_done());

  braft::Task task;
  task.data = msg.SerializeToIOBuf();
  task.done = closure;
  node_->apply(task);
}


} // namespace dingodb