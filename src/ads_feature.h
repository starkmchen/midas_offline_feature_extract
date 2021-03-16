#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <google/protobuf/util/json_util.h>
//#include "proto/model_feature.pb.h"
//#include "proto/tf_feature.pb.h"
#include "model_feature.pb.h"
#include "tf_feature.pb.h"


struct FeatureResult {
    // key: feature_name, value: val
    std::map<std::string, float> float_features;
    // key: feature_name, value: sparse_id
    std::map<std::string, uint32_t> int_features;
    // key: feature_name, value: sequence_id
    std::map<std::string, std::vector<uint32_t>> sequence_features;
    void merge(std::shared_ptr<FeatureResult> ptr);
};


typedef std::shared_ptr<FeatureResult> FeatureResultPtr;

class ModelFeature {
public:
    ModelFeature() : hash_enable(false) {}
    ModelFeature(const std::string &feature_index) {
      hash_enable = true;
      std::ifstream fin(feature_index);
      if (fin.is_open()) {
        std::string line;
        while(std::getline(fin, line)) {
          std::stringstream ssin(line);
          std::string fea_key, fea_value, tok_str;
          int32_t cnt = 0;
          while (std::getline(ssin, tok_str, '\t')) {
            if (cnt == 0) {
              fea_key = tok_str;
            }
            if (cnt == 2) {
              fea_value = tok_str;
              break;
            }
            cnt++;
          }
          feature_map_.insert({fea_key, atoi(fea_value.c_str())});
        }
      }
    }

    std::string extract_json(const std::string& str);
    std::string extract_tf_example(const std::string& str);
    FeatureResultPtr extract_feature(const Feature& feature);
    void extract_user_feature(const UserProfile& up);
    void extract_ad_feature(const AdData& ad);
    void extract_user_ad_feature(const UserAdFeature& uaf);
    void extract_ctx_feature(const Context& ctx);

private:
    void extract_user_profile(const std::string& prefix, const UserProfile& up);
    void extract_ad_data(const std::string& prefix, const AdData& ad);
    void extract_user_ad_feature(const std::string& prefix, const UserAdFeature& uaf);
    void extract_context(const std::string& prefix, const Context& ctx);

    void extract_user_base(const std::string& prefix, const UserBase& ub);
    void extract_user_behavior(const std::string& prefix, const UserBehavior& ub);
    void extract_user_count(const std::string& prefix, const UserCount& uc);

    void extract_bcf(const std::string& prefix, const BusinessCountFeature& bcf);
    void extract_acf(const std::string& prefix, const BusinessCountFeature_ActionCountFeature& acf);

    void extract_ad_info(const std::string& prefix, const AdInfo& ai);
    void extract_ad_count(const std::string& prefix, const AdCount& ac);

    void extract_user_ad_count(const std::string& prefix, const UserAdCount& uac);

    uint32_t hash_feature(const std::string &key, uint32_t id) {
      if (id == 0) {
        return 0;
      }
      auto it = feature_map_.find(key);
      if (it != feature_map_.end()) {
        return (id % (it->second - 1)) + 1;
      }
      return 0;
    }

    void append(const std::string& prefix, const std::string& name, uint32_t fid, bool is_seq=false) {
        std::string key = prefix + name;
        if (hash_enable) {
          fid = hash_feature(key, fid);
        }
        if (is_seq) {
            feature_result_->sequence_features[key].push_back(fid);
        } else {
            feature_result_->int_features.emplace(key, fid);
        }
    }
    FeatureResultPtr feature_result_;
    std::map<std::string, uint32_t> feature_map_;
    bool hash_enable;
};
