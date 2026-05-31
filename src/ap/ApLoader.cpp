#include "ap/ApLoader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace apex {

using json = nlohmann::json;

// ── public ────────────────────────────────────────────────────

ApLoader& ApLoader::with_shapes_yaml(const std::string& path) {
    shapes_yaml_path_ = path;
    return *this;
}

std::vector<std::unique_ptr<ApNode>>
ApLoader::load_json_string(const std::string& src) const {
    const ShapeMap shapes = load_shapes_yaml();
    const json j = json::parse(src);
    std::vector<std::unique_ptr<ApNode>> result;
    for (const auto& item : j)
        result.push_back(parse_node(&item, shapes));
    return result;
}

std::vector<std::unique_ptr<ApNode>>
ApLoader::load_json_file(const std::string& path) const {
    std::ifstream f(path);
    if (!f)
        throw std::runtime_error("ApLoader: cannot open " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return load_json_string(ss.str());
}

// ── private ───────────────────────────────────────────────────

ApLoader::ShapeMap ApLoader::load_shapes_yaml() const {
    ShapeMap map;
    if (shapes_yaml_path_.empty())
        return map;

    const YAML::Node root = YAML::LoadFile(shapes_yaml_path_);
    const YAML::Node shapes = root["shapes"];
    if (!shapes)
        return map;

    for (const auto& kv : shapes) {
        const std::string name = kv.first.as<std::string>();
        std::vector<int64_t> dims;
        for (const auto& d : kv.second)
            dims.push_back(d.as<int64_t>());
        map[name] = std::move(dims);
    }
    return map;
}

std::unique_ptr<ApNode>
ApLoader::parse_node(const void* raw, const ShapeMap& shapes) const {
    const json& j = *static_cast<const json*>(raw);
    const std::string type = j.at("type").get<std::string>();

    if (type == "Scalar") {
        auto n    = std::make_unique<ScalarNode>();
        n->name   = j.at("name").get<std::string>();
        n->op     = j.value("op", "");
        return n;
    }

    if (type == "Array") {
        auto n       = std::make_unique<ArrayNode>();
        n->name      = j.at("name").get<std::string>();
        n->op        = j.value("op", "");
        n->elem_size = j.value("elem_size", int64_t{4});
        for (const auto& idx : j.at("indices"))
            n->indices.push_back(idx.get<std::string>());

        if (j.contains("shape") && !j["shape"].is_null()) {
            for (const auto& d : j["shape"])
                n->shape.push_back(d.get<int64_t>());
        } else {
            auto it = shapes.find(n->name);
            if (it == shapes.end())
                throw std::runtime_error(
                    "ApLoader: no shape for array '" + n->name +
                    "' — add it to shapes.yaml");
            n->shape = it->second;
        }
        return n;
    }

    if (type == "Call") {
        auto n    = std::make_unique<CallNode>();
        n->callee = j.at("callee").get<std::string>();
        for (const auto& a : j.at("args"))
            n->args.push_back(a.get<std::string>());
        return n;
    }

    if (type == "Loop") {
        auto n    = std::make_unique<LoopNode>();
        n->var    = j.at("var").get<std::string>();
        n->start  = j.at("start").get<int64_t>();
        n->bound  = j.at("bound").get<int64_t>();
        n->depth  = j.value("depth", int64_t{1});
        for (const auto& child : j.at("body"))
            n->body.push_back(parse_node(&child, shapes));
        return n;
    }

    throw std::runtime_error("ApLoader: unknown node type '" + type + "'");
}

}  // namespace apex
