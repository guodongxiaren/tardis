// @filename:   common_dict.h
// @author:     guodongxiaren
// @biref:      parse data
// @date:       2021-07-18

#pragma once

#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <new>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

#include <glog/logging.h>
#include <google/protobuf/descriptor.h>

#include "tardis.pb.h"
#include "common.h"
#define DictName const char*

namespace tardis {

template<typename T, DictName N>
class Dict {
private:
    // @brief:     构造函数
    Dict() {}

public:
    static Dict& get_instance() {
        static Dict inst;
        return inst;
    }

    // load dict file
    int load_file(const std::string& dict_file);

    template<typename ...Args>
    std::shared_ptr<T> get_record_by_key(Args... keys);

    std::shared_ptr<T> get_record_by_index(int index);

    int get_record_num();

private:
    int read_line(const std::string& line);

    static int string_to_message(const std::string& line, google::protobuf::Message* message, std::string* p_key);

    static int make_entry(const std::string& col, google::protobuf::Message* entry, 
                          const google::protobuf::FieldDescriptor* field, bool is_repeated = false);

private:
    std::unordered_map<std::string, int> _dict;
    std::vector<std::shared_ptr<T>> _record;
    std::string _dict_filename;

};

template <typename T, DictName N>
int Dict<T, N>::make_entry(const std::string& col, google::protobuf::Message* entry,
                              const google::protobuf::FieldDescriptor* field, bool is_repeated) {
    const google::protobuf::Reflection* reflection = entry->GetReflection();
    int ret = 0;

    switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL: {
        bool value = (stoi(col) == 1);

        if (is_repeated) {
            reflection->AddBool(entry, field, value);
        } else {
            reflection->SetBool(entry, field, value);
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM: {
        if (is_repeated) {
            reflection->AddInt32(entry, field, stol(col));
        } else {
            reflection->SetInt32(entry, field, stol(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32: {
        if (is_repeated) {
            reflection->AddInt32(entry, field, stol(col));
        } else {
            reflection->SetInt32(entry, field, stol(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64: {
        if (is_repeated) {
            reflection->AddInt64(entry, field, stoll(col));
        } else {
            reflection->SetInt64(entry, field, stoll(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32: {
        if (is_repeated) {
            reflection->AddUInt32(entry, field, stoul(col));
        } else {
            reflection->SetUInt32(entry, field, stoul(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64: {
        if (is_repeated) {
            reflection->AddUInt64(entry, field, stoul(col));
        } else {
            reflection->SetUInt64(entry, field, stoul(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_DOUBLE: {
        if (is_repeated) {
            reflection->AddDouble(entry, field, stod(col));
        } else {
            reflection->SetDouble(entry, field, stod(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_FLOAT: {
        if (is_repeated) {
            reflection->AddFloat(entry, field, stof(col));
        } else {
            reflection->SetFloat(entry, field, stof(col));
        }
        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING: {
        if (is_repeated) {
            reflection->AddString(entry, field, col);
        } else {
            reflection->SetString(entry, field, col);
        }

        break;
    }

    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
        const std::string& mname = field->message_type()->name();
        google::protobuf::Message* msg = nullptr;

        if (is_repeated) {
            msg = reflection->AddMessage(entry, field);
        } else {
            msg = reflection->MutableMessage(entry, field);
        }

        ret = string_to_message(col, msg, nullptr);
        break;
    }

    default:
        break;
    }

    return ret;
}

template<typename T, DictName N>
template<typename ...Args>
std::shared_ptr<T> Dict<T, N>::get_record_by_key(Args... keys) {
    if (_dict.size() == 0) {
        LOG(ERROR)<< "dict is empty";
        return nullptr;
    }

    std::string key = join_param(keys...);

    if (_dict.find(key) != _dict.end()) {
        LOG(ERROR)<< "found dict key:" << key;
        int record_index = _dict[key];
        return get_record_by_index(record_index);
    } else {
        return nullptr;
    }
}

template<typename T, DictName N>
int Dict<T, N>::load_file(const std::string& dict_file) {
    _dict_filename = dict_file;
    std::ifstream fin(_dict_filename);

    if (!fin) {
        LOG(ERROR)<< "open file:" << _dict_filename << " failed";
        return 2;
    }

    std::string line;
    std::vector<std::string> lines;

    while (getline(fin, line)) {
        lines.emplace_back(line);
    }

    for (std::string& line : lines) {

        if (this->read_line(line) != 0) {
            LOG(ERROR)<< "read_line:"<< line;
            return -1;
        }
    }

    return 0;
}

template<typename T, DictName N>
int Dict<T, N>::read_line(const std::string& line) {
    std::string key;

    std::shared_ptr<T> entry = std::make_shared<T>();
    int ret = string_to_message(line, entry.get(), &key);
    if (ret != 0) {
        return ret;
    }

    int next_record_index = _record.size();
    _record.emplace_back(entry);

    _dict[key] = next_record_index;

    return 0;
}

template<typename T, DictName N>
int Dict<T, N>::string_to_message(const std::string& line,
                                     google::protobuf::Message* message,
                                     std::string* p_key) {
    std::vector<std::string> cols;
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

    auto& sep = descriptor->options().GetExtension(tardis::separator);

    if (split(cols, line, sep) <= 0) {
        LOG(ERROR)<< "split failed:"<< line;
        return -1;
    }

    int field_count = descriptor->field_count();

    if (field_count > cols.size()) {
        LOG(ERROR)<< "field_count:" << field_count << "cols:"<< cols.size();
        return -1;
    }

    // 记录组合键，也可以是单键
    std::stringstream composite_key;
    bool is_first = true;

    for (int i = 0; i < field_count; ++i) {
        const std::string& col = cols[i];
        auto field = descriptor->field(i);
        LOG(INFO)<< "field_" << i <<" col:"<< col << " cpp_type:" << field->cpp_type();

        bool key_opt = field->options().GetExtension(tardis::key);

        if (key_opt) {
            if (is_first) {
                composite_key << col;
                is_first = false;
            } else {
                composite_key << '\t' << col;
            }
        }

        if (!field->is_repeated()) {
            int ret = make_entry(col, message, field);

            if (ret != 0) {
                LOG(ERROR)<< "make field failed:"<< col;
                return -1;
            }
        } else {
            auto& del = field->options().GetExtension(tardis::delimiter);
            std::vector<std::string> vs;

            if (split(vs, col, del) <= 0) {
                LOG(ERROR)<< "split failed:"<< vs[1];
                return -1;
            }

            for (int i = 0; i < vs.size(); ++i) {
                std::string& data = vs[i];
                LOG(INFO)<< "repeated cpp_type:" << field->cpp_type() <<" array" << i << "=" << data;
                int ret = make_entry(data, message, field, true);

                if (ret != 0) {
                    LOG(ERROR)<< "make_entry failed:"<< data;
                    return -1;
                }
            }
        }
    }
    if (p_key) {
        // 如果词表没设置键，则行记录做键
        if (is_first) {
            *p_key = line;
        } else {
            *p_key = composite_key.str();
        }
    }
    return 0;
}

template<typename T, DictName N>
int Dict<T, N>::get_record_num() {
    return _record.size();
}

template<typename T, DictName N>
std::shared_ptr<T> Dict<T, N>::get_record_by_index(int index) {
    if (index >= _record.size()) {
        return nullptr;
    } else {
        return _record[index];
    }
}

} // namespace tardis
