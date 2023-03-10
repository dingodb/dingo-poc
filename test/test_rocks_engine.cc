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

#include <dirent.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common/context.h"
#include "config/config_manager.h"
#include "engine/engine.h"
#include "engine/rocks_engine.h"
#include "proto/common.pb.h"
#include "server/server.h"

#if 0
void Getfilepath(const char *path, const char *filename, char *filepath) {
  strcpy(filepath, path);  // NOLINT

  if (filepath[strlen(path) - 1] != '/') strcat(filepath, "/");  // NOLINT

  strcat(filepath, filename);  // NOLINT
}

bool DeleteFile(const char *path) {
  DIR *dir;
  struct dirent *dirinfo;
  struct stat statbuf;
  char filepath[256] = {0};
  lstat(path, &statbuf);

  if (S_ISREG(statbuf.st_mode)) {
    remove(path);
  } else if (S_ISDIR(statbuf.st_mode)) {
    if ((dir = opendir(path)) == nullptr) return true;
    while ((dirinfo = readdir(dir)) != nullptr) {
      Getfilepath(path, dirinfo->d_name, filepath);
      if (strcmp(dirinfo->d_name, ".") == 0 ||
          strcmp(dirinfo->d_name, "..") == 0)
        continue;
      DeleteFile(filepath);
      rmdir(filepath);
    }
    closedir(dir);
  }
  rmdir(path);
  return true;
}
#endif

// static const std::string &kDefautCf = "default";
// // static const std::string &kDefautCf = "meta";

// class RocksEngineTest {
//  public:
//   std::shared_ptr<dingodb::Config> GetConfig() { return config_; }

//   dingodb::RocksEngine &GetRocksEngine() { return rocks_engine_; }

//   void SetUp() {
//     std::cout << "RocksEngineTest::SetUp()" << std::endl;
//     server_ = dingodb::Server::GetInstance();
//     filename_ = "../../conf/store.yaml";
//     server_->SetRole(dingodb::pb::common::ClusterRole::STORE);
//     server_->InitConfig(filename_);
//     config_manager_ = dingodb::ConfigManager::GetInstance();
//     config_ = config_manager_->GetConfig(dingodb::pb::common::ClusterRole::STORE);
//   }
//   void TearDown() {}

//  private:
//   dingodb::Server *server_;
//   std::string filename_ = "../../conf/store.yaml";
//   dingodb::ConfigManager *config_manager_;
//   std::shared_ptr<dingodb::Config> config_;
//   dingodb::RocksEngine rocks_engine_;
// };

// static RocksEngineTest rocks_engine_test;

// TEST(RocksEngineTest, BeforeInit) { rocks_engine_test.SetUp(); }

// TEST(RocksEngineTest, Init) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   bool ret = rocks_engine.Init({});
//   EXPECT_FALSE(ret);

//   std::shared_ptr<dingodb::Config> config = rocks_engine_test.GetConfig();
// #if 0
//   std::string store_db_path_value = config->GetString("store.dbPath");
//   if (!store_db_path_value.empty()) {
//     struct stat statbuf;
//     lstat(store_db_path_value.c_str(), &statbuf);
//     if (S_ISDIR(statbuf.st_mode)) {
//       std::cout << "path : " << store_db_path_value << "need to delete [Y/N]"
//                 << std::endl;
//       std::string s;
//       std::cin >> s;
//       if (s == "Y" || s == "y" || s == "yes" || s == "Yes") {
//         DeleteFile(store_db_path_value.c_str());
//       }
//     }
// #endif

//   // Test for various configuration file exceptions
//   ret = rocks_engine.Init(config);
//   EXPECT_TRUE(ret);
// }

// TEST(RocksEngineTest, KvPut) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     dingodb::pb::common::KeyValue kv;

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut({}, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     dingodb::pb::common::KeyValue kv;

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // value empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key2");
//     kv.set_value("value2");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key3");
//     kv.set_value("value3");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPut(ctx, kv);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvBatchPut) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut({}, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.emplace_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     dingodb::pb::common::KeyValue kv;
//     ctx->set_cf_name("dummy");
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.emplace_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // key empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     kv.set_key("");
//     kv.set_value("value1");
//     kvs.emplace_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.emplace_back(kv);

//     kv.set_key("key2");
//     kv.set_value("value2");
//     kvs.emplace_back(kv);

//     kv.set_key("");
//     kv.set_value("value3");
//     kvs.emplace_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // ok
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.emplace_back(kv);

//     kv.set_key("key2");
//     kv.set_value("value2");
//     kvs.emplace_back(kv);

//     kv.set_key("key3");
//     kv.set_value("value3");
//     kvs.emplace_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvGet) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::string key;

//     std::string value;
//     dingodb::pb::error::Errno ok = rocks_engine.KvGet({}, key, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::string key = "key";
//     std::string value;

//     dingodb::pb::error::Errno ok = rocks_engine.KvGet(ctx, key, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::string key = "key";
//     std::string value;

//     dingodb::pb::error::Errno ok = rocks_engine.KvGet(ctx, key, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     std::string key;
//     std::string value;

//     dingodb::pb::error::Errno ok = rocks_engine.KvGet(ctx, key, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   {
//     const std::string &key = "key1";
//     std::string value;

//     dingodb::pb::error::Errno ok = rocks_engine.KvGet(ctx, key, value);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvCompareAndSet) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::string value;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet({}, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");
//     std::string value = "value123456";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");
//     std::string value = "value123456";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     std::string value = "value123456";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // value empty . key exist failed
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     std::string value = "value";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // value empty . key exist failed
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     std::string value = "value123456";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EINTERNAL);
//   }

//   // normal
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     const std::string &value = "value1_modify";

//     dingodb::pb::error::Errno ok = rocks_engine.KvCompareAndSet(ctx, kv, value);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvBatchGet) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet({}, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::vector<std::string> keys{"key", "key1"};
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::vector<std::string> keys{"key", "key1"};
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key all empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // key some empty
//   {
//     std::vector<std::string> keys{"key1", "", "key"};
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key not exist
//   {
//     std::vector<std::string> keys{"key1", "key2", "key", "key4"};
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EINTERNAL);
//   }

//   // normal
//   {
//     std::vector<std::string> keys{"key1", "key"};
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchGet(ctx, keys, kvs);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvPutIfAbsent) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     dingodb::pb::common::KeyValue kv;

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent({}, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");
//     kv.set_value("value");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     dingodb::pb::common::KeyValue kv;

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // value empty . key exist failed
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EINTERNAL);
//   }

//   // value empty . key exist failed
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EINTERNAL);
//   }

//   // normal
//   {
//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key10");
//     kv.set_value("value10");

//     dingodb::pb::error::Errno ok = rocks_engine.KvPutIfAbsent(ctx, kv);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvBatchPutIfAbsentAtomic) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic({}, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");

//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key all empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key not exist
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     kv.set_key("");
//     kv.set_value("value2");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key exist failed
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;

//     kv.set_key("key111");
//     kv.set_value("value111");
//     kvs.push_back(kv);

//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     kv.set_key("key2");
//     kv.set_value("value2");
//     kvs.push_back(kv);

//     kv.set_key("key");
//     kv.set_value("value");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EINTERNAL);

//     std::string value;
//     ok = rocks_engine.KvGet(ctx, "key111", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_NOTFOUND);
//   }

//   // normal key all not exist
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;

//     kv.set_key("key101");
//     kv.set_value("value101");
//     kvs.push_back(kv);

//     kv.set_key("key102");
//     kv.set_value("value102");
//     kvs.push_back(kv);

//     kv.set_key("key103");
//     kv.set_value("value103");
//     kvs.push_back(kv);

//     kv.set_key("key104");
//     kv.set_value("value104");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string value;
//     ok = rocks_engine.KvGet(ctx, "key101", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key102", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key103", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key104", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvBatchPutIfAbsentNonAtomic) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic({}, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");

//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key all empty
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key not exist
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;
//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     kv.set_key("");
//     kv.set_value("value2");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // some key exist failed
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;

//     kv.set_key("key1111");
//     kv.set_value("value111");
//     kvs.push_back(kv);

//     kv.set_key("key1");
//     kv.set_value("value1");
//     kvs.push_back(kv);

//     kv.set_key("key2");
//     kv.set_value("value2");
//     kvs.push_back(kv);

//     kv.set_key("key");
//     kv.set_value("value");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string value;
//     ok = rocks_engine.KvGet(ctx, "key1111", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // normal key all not exist
//   {
//     std::vector<std::string> keys;
//     std::vector<dingodb::pb::common::KeyValue> kvs;
//     std::vector<std::string> put_keys;

//     dingodb::pb::common::KeyValue kv;

//     kv.set_key("key201");
//     kv.set_value("value201");
//     kvs.push_back(kv);

//     kv.set_key("key202");
//     kv.set_value("value202");
//     kvs.push_back(kv);

//     kv.set_key("key203");
//     kv.set_value("value203");
//     kvs.push_back(kv);

//     kv.set_key("key204");
//     kv.set_value("value204");
//     kvs.push_back(kv);

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPutIfAbsentNonAtomic(ctx, kvs, put_keys);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string value;
//     ok = rocks_engine.KvGet(ctx, "key201", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key202", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key203", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     ok = rocks_engine.KvGet(ctx, "key204", value);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvScan) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::shared_ptr<dingodb::Context> ctx;
//     std::string begin_key;
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan({}, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::string begin_key = "key01";
//     std::string end_key = "key1000";
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::string begin_key = "key01";
//     std::string end_key = "key1000";
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // ok
//   {
//     std::string begin_key;
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }

//   // ok
//   {
//     std::string begin_key = "key101";
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }

//   // ok
//   {
//     std::string begin_key = "key201";
//     std::string end_key = "key204";
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     dingodb::pb::error::Errno ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }
// }

// TEST(RocksEngineTest, KvCount) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::shared_ptr<dingodb::Context> ctx;
//     std::string begin_key;
//     std::string end_key;
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount({}, begin_key, end_key, count);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::string begin_key = "key01";
//     std::string end_key = "key1000";
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount(ctx, begin_key, end_key, count);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::string begin_key = "key01";
//     std::string end_key = "key1000";
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount(ctx, begin_key, end_key, count);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // ok
//   {
//     std::string begin_key;
//     std::string end_key;
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount(ctx, begin_key, end_key, count);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << " count : " << count << std::endl;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     EXPECT_EQ(count, kvs.size());
//   }

//   // ok
//   {
//     std::string begin_key = "key101";
//     std::string end_key;
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount(ctx, begin_key, end_key, count);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << " count : " << count << std::endl;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     EXPECT_EQ(count, kvs.size());
//   }

//   // ok
//   {
//     std::string begin_key = "key201";
//     std::string end_key = "key204";
//     int64_t count = 0;

//     dingodb::pb::error::Errno ok = rocks_engine.KvCount(ctx, begin_key, end_key, count);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << " count : " << count << std::endl;

//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//     EXPECT_EQ(count, kvs.size());
//   }
// }

// TEST(RocksEngineTest, CreateReader) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::shared_ptr<dingodb::Context> ctx;

//     std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader({});
//     EXPECT_EQ(reader.get(), nullptr);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader(ctx);
//     EXPECT_EQ(reader.get(), nullptr);
//   }

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");

//     std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader(ctx);
//     EXPECT_EQ(reader.get(), nullptr);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // ok
//   {
//     std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader(ctx);
//     EXPECT_NE(reader.get(), nullptr);
//   }
// }

// TEST(RocksEngineTest, EngineReader) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();
//   const std::string &cf_name = kDefautCf;
//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();
//   ctx->set_cf_name(cf_name);

//   std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader(ctx);

//   // GetSelf
//   {
//     std::shared_ptr<dingodb::EngineReader> reader_another = reader->GetSelf();
//     EXPECT_EQ(2, reader_another.use_count());
//   }

//   // test in another unit. ingore
//   { std::shared_ptr<dingodb::EngineIterator> iter = reader->Scan("", ""); }

//   {
//     const std::string &key = "key";
//     auto value = reader->KvGet(key);
//     EXPECT_EQ("value", *value);
//   }

//   {
//     const std::string &key = "keykey";
//     auto value = reader->KvGet(key);
//     EXPECT_EQ(nullptr, value.get());
//   }

//   {
//     auto name = reader->GetName();
//     EXPECT_EQ("RocksReader", name);
//   }

//   {
//     uint32_t id = reader->GetID();
//     EXPECT_EQ(dingodb::EnumEngineReader::kRocksReader, static_cast<dingodb::EnumEngineReader>(id));
//   }
// }

// TEST(RocksEngineTest, EngineIterator) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();
//   const std::string &cf_name = kDefautCf;
//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();
//   ctx->set_cf_name(cf_name);

//   std::shared_ptr<dingodb::EngineReader> reader = rocks_engine.CreateReader(ctx);
//   std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan("", "");

//   // GetSelf
//   {
//     auto engine_iterator_another = engine_iterator->GetSelf();
//     EXPECT_EQ(2, engine_iterator_another.use_count());
//   }

//   {
//     auto name = engine_iterator->GetName();
//     EXPECT_EQ("RocksIterator", name);
//   }

//   {
//     uint32_t id = engine_iterator->GetID();
//     EXPECT_EQ(dingodb::EnumEngineIterator::kRocksIterator, static_cast<dingodb::EnumEngineIterator>(id));
//   }

//   {
//     std::string begin_key;
//     std::string end_key;

//     std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan(begin_key, end_key);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     while (engine_iterator->HasNext()) {
//       std::string key;
//       std::string value;
//       engine_iterator->GetKV(key, value);
//       std::cout << key << ":" << value << std::endl;
//       engine_iterator->Next();
//     }
//   }

//   {
//     std::string begin_key = "key101";
//     std::string end_key;

//     std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan(begin_key, end_key);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     while (engine_iterator->HasNext()) {
//       std::string key;
//       std::string value;
//       engine_iterator->GetKV(key, value);
//       std::cout << key << ":" << value << std::endl;
//       engine_iterator->Next();
//     }
//   }

//   {
//     std::string begin_key = "key201";
//     std::string end_key = "key204";

//     std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan(begin_key, end_key);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     while (engine_iterator->HasNext()) {
//       std::string key;
//       std::string value;
//       engine_iterator->GetKV(key, value);
//       std::cout << key << ":" << value << std::endl;
//       engine_iterator->Next();
//     }
//   }

//   {
//     std::string begin_key;
//     std::string end_key;

//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     kvs.reserve(300);

//     for (size_t i = 0; i < 300; i++) {
//       dingodb::pb::common::KeyValue kv;
//       kv.set_key("key" + std::to_string(i));
//       kv.set_value("value" + std::to_string(i));
//       kvs.push_back(kv);
//     }

//     dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan(begin_key, end_key);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     while (engine_iterator->HasNext()) {
//       std::string key;
//       std::string value;
//       engine_iterator->GetKV(key, value);
//       std::cout << key << ":" << value << std::endl;
//       engine_iterator->Next();
//     }
//   }

//   {
//     std::string begin_key;
//     std::string end_key;

//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     kvs.reserve(300);

//     for (size_t i = 0; i < 300; i++) {
//       dingodb::pb::common::KeyValue kv;
//       kv.set_key("key" + std::to_string(i));
//       kv.set_value("value" + std::to_string(i));
//       kvs.push_back(kv);
//     }

//     bool run_once = false;
//     std::shared_ptr<dingodb::EngineIterator> engine_iterator = reader->Scan(begin_key, end_key);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     while (engine_iterator->HasNext()) {
//       std::string key;
//       std::string value;
//       engine_iterator->GetKV(key, value);
//       std::cout << key << ":" << value << std::endl;
//       engine_iterator->Next();

//       if (!run_once) {
//         dingodb::pb::error::Errno ok = rocks_engine.KvBatchPut(ctx, kvs);
//         EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//         run_once = true;
//       }
//     }
//   }
// }

// TEST(RocksEngineTest, KvDelete) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     std::string key;

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete({}, key);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     std::string key = "key";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     std::string key = "key";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     std::string key;

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // ok
//   {
//     const std::string &key = "key1";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key1";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key2";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key3";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key10";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key1111";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key101";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key102";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key103";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key104";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key201";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key202";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key203";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }

//   // ok
//   {
//     const std::string &key = "key204";

//     dingodb::pb::error::Errno ok = rocks_engine.KvDelete(ctx, key);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);
//   }
// }

// TEST(RocksEngineTest, KvDeleteRange) {
//   dingodb::RocksEngine &rocks_engine = rocks_engine_test.GetRocksEngine();

//   // Context empty
//   {
//     dingodb::pb::common::Range range;

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange({}, range);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ECONTEXT);
//   }

//   std::shared_ptr<dingodb::Context> ctx = std::make_shared<dingodb::Context>();

//   // Context not empty, but Context name empty
//   {
//     dingodb::pb::common::Range range;
//     range.set_start_key("key1");
//     range.set_end_key("key10");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   const std::string &cf_name = kDefautCf;
//   ctx->set_cf_name(cf_name);

//   // Context not empty, but Context name not exist
//   {
//     ctx->set_cf_name("dummy");
//     dingodb::pb::common::Range range;
//     range.set_start_key("key1");
//     range.set_end_key("key10");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::ESTORE_INVALID_CF);
//   }

//   ctx->set_cf_name(cf_name);

//   // key empty
//   {
//     dingodb::pb::common::Range range;

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // key not empty but value empty
//   {
//     dingodb::pb::common::Range range;
//     range.set_start_key("key");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::EKEY_EMPTY);
//   }

//   // ok
//   {
//     dingodb::pb::common::Range range;
//     range.set_start_key("key");
//     range.set_end_key("key100");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string begin_key;
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }

//   // ok
//   {
//     dingodb::pb::common::Range range;
//     range.set_start_key("key100");
//     range.set_end_key("key200");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string begin_key;
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }

//   // ok
//   {
//     dingodb::pb::common::Range range;
//     range.set_start_key("key");
//     range.set_end_key("key99999");

//     dingodb::pb::error::Errno ok = rocks_engine.KvDeleteRange(ctx, range);

//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::string begin_key;
//     std::string end_key;
//     std::vector<dingodb::pb::common::KeyValue> kvs;

//     ok = rocks_engine.KvScan(ctx, begin_key, end_key, kvs);
//     EXPECT_EQ(ok, dingodb::pb::error::Errno::OK);

//     std::cout << "begin_key : " << begin_key << " "
//               << "end_key : " << end_key << std::endl;
//     for (const auto &kv : kvs) {
//       std::cout << kv.key() << ":" << kv.value() << std::endl;
//     }
//   }
// }
