//
//
// Author : huziang
// this is a cpp file complete mongodb interface in c++

#include "WZMongodbEngine.h"
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include "ToDocuments.h"
#include "DataParse.h"

using mongocxx::cursor;

DataEngine* MongodbEngine::getInstance() {
  if (instance == NULL) {
    MongodbEngine *mginstance = new MongodbEngine();
    mginstance->conn = new mongocxx::client(mongocxx::uri{});
    instance = mginstance;
  }
  return instance;
}

int MongodbEngine::insert_one(const map<string, string> &md) {
  // get document
  document doc {};
  toDocument(md, doc);

  // get collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  auto result = coll.insert_one(doc.view());
  return (bool)result;
}

int MongodbEngine::insert_many(const vector<map<string, string>> &mds) {
  // get document
  vector<bsoncxx::document::value> docvs;
  toDocument(mds, docvs);

  // get collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  auto result = coll.insert_many(docvs);
  return result->inserted_count();
}

int MongodbEngine::update_one(const KeyValue &filter, const vector<KeyValue> &update) {
  // get document
  document filterdoc {};
  document updatedoc {};
  toDocument(filter, update, filterdoc, updatedoc);

  // get collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  auto result = coll.update_one(filterdoc.view(), updatedoc.view());
  return (bool)result;
}

int MongodbEngine::update_many(const KeyValue &filter, const vector<KeyValue> &update) {
  // get document
  document filterdoc {};
  document updatedoc {};
  toDocument(filter, update, filterdoc, updatedoc);

  // get collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  auto result = coll.update_many(filterdoc.view(), updatedoc.view());
  return result->modified_count();
}

int MongodbEngine::find_one(map<string, string> &md, const vector<KeyValue> &condition, const char ID[20]) {
  // get document
  document doc {};
  toDocument(condition, ID, doc);

  // get one collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  auto result = coll.find_one(doc.view());
  if (result) {
    string json = bsoncxx::to_json(result->view());
    parseTo(md, json);
    return 1;
  }
  return 0;
}

int MongodbEngine::find_many(vector<map<string, string>> &mds, const vector<KeyValue> &condition, const char ID[20]) {
  // get document
  document doc {};
  toDocument(condition, ID, doc);

  // get many collection
  mongocxx::database db = conn->database(libname);
  mongocxx::collection coll = db[tablename];
  mongocxx::cursor cursor = coll.find(doc.view());

  // go through all answer
  int num = 0;
  map<string, string> result;
  string json;
  for(auto &cur : cursor){
    json = bsoncxx::to_json(cur);
    parseTo(result, json);
    mds.push_back(result);
    result.clear();
    num++;
  }

  return num;
}