#include <cmath>
#include <chrono>
#include <sstream>
#include <functional>
#include <google/protobuf/util/json_util.h>
#include <cstdint>
#include "ads_feature.h"

void FeatureResult::merge(std::shared_ptr<FeatureResult> ptr) {
    for (auto it : ptr->int_features) {
        int_features[it.first] = it.second;
    }
    for (auto it : ptr->sequence_features) {
        sequence_features[it.first] = it.second;
    }
    for (auto it : ptr->float_features) {
        float_features[it.first] = it.second;
    }
}

inline uint32_t time33(const char* str, size_t key_length) {
    if (key_length == 0) {
      return 0;
    }
    uint32_t hash = 5381;
    while (key_length--) {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

uint32_t hash(const std::string& t) {
    return time33(t.c_str(), t.length());
}

template<typename T, typename H>
uint32_t hash(const T& t, const H& h) {
    uint32_t id = hash(std::to_string(t) + "_" + std::to_string(h));
    return id;
}

template<typename T>
uint32_t log_int(T t) {
    if (t == 0) {
      return 0;
    }
    return uint32_t(std::log2(std::max(t, (T)2)) * 10);
}

template<typename T, typename H>
uint32_t log_div(T a, H b) {
    if (b == 0) {
        return log_int(a);
    } else {
        return log_int(double(a) / b);
    }
}


uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

std::string ModelFeature::extract_tf_example(const std::string& str) {
	Sample sample;
  google::protobuf::util::JsonParseOptions options;
  options.ignore_unknown_fields = true;
  auto status = google::protobuf::util::JsonStringToMessage(str, &sample, options);
    if (status.ok()) {
        auto result = extract_feature(sample.feature());
		tensorflow_def::Example example;
		auto features = example.mutable_features()->mutable_feature();
        for (auto it : result->int_features) {
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			int64->mutable_value()->Add(it.second);
			(*features)[it.first] = feature;
		}
        for (auto it : result->float_features) {
			tensorflow_def::Feature feature;
			auto floats = feature.mutable_float_list();
			floats->mutable_value()->Add(it.second);
			(*features)[it.first] = feature;
		}
        for (auto it : result->sequence_features) {
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			for (auto v : it.second) {
				int64->mutable_value()->Add(v);
			}
			(*features)[it.first] = feature;
		}
		{
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			int64->mutable_value()->Add(sample.label().imp());
			(*features)["is_imp"] = feature;
		}
		{
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			int64->mutable_value()->Add(sample.label().click());
			(*features)["is_click"] = feature;
		}
		{
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			int64->mutable_value()->Add(sample.label().install());
			(*features)["is_install"] = feature;
		}
		{
			tensorflow_def::Feature feature;
			auto int64 = feature.mutable_int64_list();
			int64->mutable_value()->Add(sample.label().attr_install());
			(*features)["is_attr_install"] = feature;
		}

		std::string ret;
		example.SerializeToString(&ret);
		return ret;
    } else {
        std::cout<<"error json\n";
        return "";
	}

}

std::string ModelFeature::extract_json(const std::string& str) {
    Sample sample;
    google::protobuf::util::JsonParseOptions options;
    options.ignore_unknown_fields = true;
    auto status = google::protobuf::util::JsonStringToMessage(str, &sample, options);
    if (status.ok()) {
        auto result = extract_feature(sample.feature());
        std::ostringstream oss;
        oss << "{\"label\": {"
            << "\"imp\":" <<sample.label().imp()
            << ",\"click\":" <<sample.label().click()
            << ",\"install\":" <<sample.label().install()
            << ",\"attr_install\":" <<sample.label().attr_install()
            <<"},\"int_features\": {";
        int index = 0;
        for (auto it : result->int_features) {
            if (index++ > 0) {
                oss <<",";
            }
            oss << "\"" << it.first <<"\": " << it.second;
        }

        oss << "},\"float_features\": {";
        index = 0;
        for (auto it : result->float_features) {
            if (index++ > 0) {
                oss <<",";
            }
            oss << "\"" << it.first <<"\": " << it.second;
        }

        oss<<"}, \"sequence_features\": {";
        index = 0;
        for (auto it : result->sequence_features) {
            if (index++ > 0) {
                oss <<",";
            }
            oss << "\"" << it.first <<"\": [";
            int _index = 0;
            for (auto x : it.second) {
                if (_index ++ > 0) {
                    oss<<",";
                }
                oss<<x;
            }
            oss<<"]";
        }
        oss<<"}}";
        return oss.str();
    } else {
        std::cout<<"error json\n";
        return "";
    }
}

FeatureResultPtr ModelFeature::extract_feature(const Feature& feature) {
    FeatureResultPtr feature_result(std::make_shared<FeatureResult>());
    feature_result_ = feature_result;
    extract_user_feature(feature.user_profile());
    extract_ad_feature(feature.ad_data());
    extract_user_ad_feature(feature.user_ad_feature());
    extract_ctx_feature(feature.context());
    return feature_result;
}

inline void ModelFeature::extract_user_feature(const UserProfile& up) {
    extract_user_profile("up", up);
}

inline void ModelFeature::extract_ad_feature(const AdData& ad) {
    extract_ad_data("ad", ad);
}

inline void ModelFeature::extract_user_ad_feature(const UserAdFeature& uaf) {
    extract_user_ad_feature("user_ad", uaf);
}

inline void ModelFeature::extract_ctx_feature(const Context& ctx) {
    extract_context("ctx", ctx);
}


void ModelFeature::extract_acf(const std::string& prefix, const BusinessCountFeature_ActionCountFeature& acf) {
    auto imp = log_int(acf.imp());
    auto click = log_int(acf.click());
    auto install_stats = acf.install();
    if (install_stats == 0) {
      install_stats = acf.attr_install();
    }
    auto install = log_int(install_stats);

    auto imp_click = log_div(acf.imp(), acf.click());
    auto imp_install = log_div(acf.imp(), install_stats);
    auto click_install = log_div(acf.click(), install_stats);

    append(prefix, "_imp", imp);
    append(prefix, "_clk", click);
    append(prefix, "_ainst", install);

    append(prefix, "_imp_clk", imp_click);
    append(prefix, "_imp_ainst", imp_install);
    append(prefix, "_clk_ainst", click_install);

    append(prefix, "_imp/clk", hash(imp, imp_click));
    append(prefix, "_imp/ainst", hash(imp, imp_install));
    append(prefix, "_clk/ainst", hash(click, click_install));
}

void ModelFeature::extract_bcf(const std::string& prefix, const BusinessCountFeature& bcf) {
    if (bcf.has_count_features_7d()) {
        extract_acf(prefix + "_7d", bcf.count_features_7d());
    }
    if (bcf.has_count_features_1d()) {
        extract_acf(prefix + "_1d", bcf.count_features_1d());
    }
    if (bcf.has_count_features_3d()) {
        extract_acf(prefix + "_3d", bcf.count_features_3d());
    }
}

void ModelFeature::extract_ad_info(const std::string& prefix, const AdInfo& ai) {
    //if (ai.has_ad_id()) {
        append(prefix, "_adid", hash(std::to_string(ai.ad_id())));
    //}
    //if (ai.has_app_id()) {
        append(prefix, "_aid", hash(ai.app_id()));
    //}
    //if (ai.has_category()) {
        append(prefix, "_cate", hash(ai.category()));
    //}
        append(prefix, "_cid", hash(ai.creative_id()));
}

void ModelFeature::extract_ad_data(const std::string& prefix, const AdData& ad) {
    //if (ad.has_ad_info()) {
        extract_ad_info(prefix + "_ai", ad.ad_info());
    //}
    //if (ad.has_ad_counter()) {
        extract_ad_count(prefix + "_ac", ad.ad_counter());
    //}
}

void ModelFeature::extract_context(const std::string& prefix, const Context& ctx) {
    //if (ctx.has_pos_id()) {
        append(prefix, "_pi1", hash(ctx.pos_id()));
    //}

    //if (ctx.has_network_type()) {
        append(prefix, "_ne3", hash(ctx.network_type()));
    //}
    //if (ctx.has_os_version()) {
        append(prefix, "_os4", hash(ctx.os_version()));
    //}
    //if (ctx.has_brand()) {
        append(prefix, "_brand", hash(ctx.brand()));
    //}
    //if (ctx.has_model()) {
        append(prefix, "_model", hash(ctx.model()));
    //}
        append(prefix, "_lang", hash(ctx.language()));
    //if (ctx.has_app_version_code()) {
        append(prefix, "_app_ver", hash(ctx.app_version_code()));
    //}

    // 取ip的前3段。如1.2.3.4取1.2.3
    auto it_ip = ctx.client_ip().rfind('.');
    append(prefix, "_ip", hash(ctx.client_ip().substr(0, it_ip)));

    append(prefix, "_nation", hash(ctx.nation()));
    //if (ctx.has_req_time()) {
        auto time = ctx.req_time();
        int32_t hour = ((time / 1000) % 86400) / 3600;
        int32_t week = (((time / 1000) % 604800) + 86400) / 86400;
        append(prefix, "_req_hour", hour);
        append(prefix, "_req_week", week);
    //}
}

void ModelFeature::extract_user_ad_count(const std::string& prefix, const UserAdCount& uac) {
    if (uac.has_user_id_ad_id()) {
        extract_bcf(prefix + "_bcf1", uac.user_id_ad_id());
    }
    if (uac.has_user_id_ad_package_name()) {
        extract_bcf(prefix + "_bcf2", uac.user_id_ad_package_name());
    }
    if (uac.has_user_id_ad_package_category()) {
        extract_bcf(prefix + "_bcf3", uac.user_id_ad_package_category());
    }
    if (uac.has_user_id_pos_id_ad_id()) {
        extract_bcf(prefix + "_bcf4", uac.user_id_pos_id_ad_id());
    }
    if (uac.has_user_id_pos_id_ad_package_name()) {
        extract_bcf(prefix + "_bcf5", uac.user_id_pos_id_ad_package_name());
    }
    if (uac.has_user_id_c_id()) {
        extract_bcf(prefix + "_bcf6", uac.user_id_c_id());
    }
    if (uac.has_user_id_pos_id_c_id()) {
        extract_bcf(prefix + "_bcf7", uac.user_id_pos_id_c_id());
    }
    if (uac.has_user_id_pos_id_ad_package_category()) {
        extract_bcf(prefix + "_bcf8", uac.user_id_pos_id_ad_package_category());
    }
}

void ModelFeature::extract_user_ad_feature(const std::string& prefix, const UserAdFeature& uaf) {
    if (uaf.has_user_ad_count()) {
        extract_user_ad_count(prefix + "_uac", uaf.user_ad_count());
    }
}

void ModelFeature::extract_ad_count(const std::string& prefix, const AdCount& ac) {
    if (ac.has_ad_id()) {
        extract_bcf(prefix + "_bcf1", ac.ad_id());
    }
    if (ac.has_ad_package_name()) {
        extract_bcf(prefix + "_bcf2", ac.ad_package_name());
    }
    if (ac.has_ad_package_category()) {
        extract_bcf(prefix + "_bcf3", ac.ad_package_category());
    }
    if (ac.has_pos_id_ad_id()) {
        extract_bcf(prefix + "_bcf4", ac.pos_id_ad_id());
    }
    if (ac.has_pos_id_ad_package_name()) {
        extract_bcf(prefix + "_bcf5", ac.pos_id_ad_package_name());
    }
    if (ac.has_c_id()) {
        extract_bcf(prefix + "_bcf6", ac.c_id());
    }
    if (ac.has_pos_id_c_id()) {
        extract_bcf(prefix + "_bcf7", ac.pos_id_c_id());
    }
    if (ac.has_pos_id_ad_package_category()) {
        extract_bcf(prefix + "_bcf8", ac.pos_id_ad_package_category());
    }
}

void ModelFeature::extract_user_count(const std::string& prefix, const  UserCount& uc) {
    if (uc.has_user_id()) {
        extract_bcf(prefix + "_uid", uc.user_id());
    }
}

void ModelFeature::extract_user_behavior(const std::string& prefix, const  UserBehavior& ub) {
    for (const auto& app : ub.click_ad_app_list()) {
        append(prefix, "_caal", hash(app), true);
    }
    for (const auto& app : ub.install_ad_app_list()) {
        append(prefix, "_iaal", hash(app), true);
    }
    for (const auto& app : ub.install_app_list()) {
        append(prefix, "_ial", hash(app), true);
    }
    for (const auto& app : ub.use_app_list()) {
        append(prefix, "_ual", hash(app), true);
    }
}

void ModelFeature::extract_user_base(const std::string& prefix, const UserBase& ub) {
    //if (ub.has_country()) {
        append(prefix, "_country", hash(ub.country()));
    //}
    //if (ub.has_province()) {
        append(prefix, "_province", hash(ub.province()));
    //}
    //if (ub.has_city()) {
        append(prefix, "_city", hash(ub.city()));
    //}
    //if (ub.has_gender()) {
        append(prefix, "_gender", hash(ub.gender()));
    //}
    //if (ub.has_age()) {
        append(prefix, "_age", hash(ub.age()));
    //}
}

void ModelFeature::extract_user_profile(const std::string& prefix, const UserProfile& up) {
    // append(prefix, "_uid", hash(up.user_id()));
    if (up.has_user_base()) {
        extract_user_base(prefix + "_ub2", up.user_base());
    }
    /*
    if (up.has_user_behavior()) {
        extract_user_behavior(prefix + "_ub3", up.user_behavior());
    }
    */
    if (up.has_user_counter()) {
        extract_user_count(prefix + "_uc", up.user_counter());
    }
}
