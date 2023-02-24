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

#ifndef DINGODB_RAFT_NODE_H_
#define DINGODB_RAFT_NODE_H_

#include <braft/raft.h>
#include <braft/util.h>

#include <memory>

#include "common/context.h"
#include "proto/error.pb.h"
#include "proto/raft.pb.h"

namespace dingodb {

// Encapsulation braft node
class RaftNode {
 public:
  RaftNode(uint64_t node_id, braft::PeerId peer_id, braft::StateMachine* fsm);
  ~RaftNode();

  int Init(const std::string& init_conf);
  void Destroy();

  pb::error::Errno Commit(std::shared_ptr<Context> ctx,
                          std::shared_ptr<pb::raft::RaftCmdRequest> raft_cmd);

  bool IsLeader();
  bool IsLeaderLeaseValid();
  braft::PeerId GetLeaderId();

  void shutdown(braft::Closure* done);

  butil::Status ListPeers(std::vector<braft::PeerId>* peers);
  void AddPeer(const braft::PeerId& peer, braft::Closure* done);
  void RemovePeer(const braft::PeerId& peer, braft::Closure* done);
  void ChangePeers(const braft::Configuration& new_peers, braft::Closure* done);
  butil::Status ResetPeers(const braft::Configuration& new_peers);

 private:
  uint64_t node_id_;
  std::unique_ptr<braft::Node> node_;
  braft::StateMachine* fsm_;
};

}  // namespace dingodb

#endif  // DINGODB_RAFT_NODE_H_