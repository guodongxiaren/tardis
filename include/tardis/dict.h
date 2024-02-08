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
#include <unordered_map>
#include <vector>

#include <google/protobuf/descriptor.h>

#include "common.h"
#include "tardis.pb.h"
#define DictName const char *

namespace tardis {

template <typename T, DictName N> class Dict {
private:
    Dict() {}

public:
    static Dict &get_instance() {
        static Dict inst;
        return inst;
    }

    // load dict file
    int load_file(const std::string &dict_file);

    template <typename... Args>
    std::shared_ptr<T> get_record_by_key(Args... keys);

    std::shared_ptr<T> get_record_by_index(int index);

    int get_record_num();

private:
    int read_line(const std::string &line);

    static int string_to_message(const std::string &line,
                                 google::protobuf::Message *message,
                                 std::string *p_key);

    static int make_entry(const std::string &col,
                          google::protobuf::Message *entry,
                          const google::protobuf::FieldDescriptor *field,
                          bool is_repeated = false);

private:
    std::unordered_map<std::string, int> _dict;
    std::vector<std::shared_ptr<T>> _record;
    std::string _dict_filename;
};

template <typename T, DictName N>
int Dict<T, N>::make_entry(const std::string &col,
                           google::protobuf::Message *entry,
                           const google::protobuf::FieldDescriptor *field,
                           bool is_repeated) {
    const google::protobuf::Reflection *reflection = entry->GetReflection();
    int ret = 0;
#define filed_type_case(X, Y, T)                                               \
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_##X: {            \
        T value = tardis::lexical_cast<T>(col);                                \
        if (is_repeated) {                                                     \
            reflection->Add##Y(entry, field, value);                           \
        } else {                                                               \
            reflection->Set##Y(entry, field, value);                           \
        }                                                                      \
        break;                                                                 \
    }

    switch (field->cpp_type()) {
        filed_type_case(BOOL, Bool, bool) 
        filed_type_case(ENUM, Int32, int32_t) 
        filed_type_case(INT32, Int32, int32_t) 
        filed_type_case(INT64, Int64, int64_t)
        filed_type_case(UINT32, UInt32, uint32_t)
        filed_type_case( UINT64, UInt64, uint64_t) 
        filed_type_case(DOUBLE, Double, double)
        filed_type_case(FLOAT, Float, float)

        case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING : {
            if (is_repeated) {
                reflection->AddString(entry, field, col);
            } else {
                reflection->SetString(entry, field, col);
            }
            break;
        }

        case google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE: {
            const std::string &mname = field->message_type()->name();
            google::protobuf::Message *msg = nullptr;

            if (is_repeated) {
                msg = reflection->AddMessage(entry, field);
            } else {
                msg = reflection->MutableMessage(entry, field);
            }

            ret = string_to_message(col, msg, nullptr);
            break;
        }

        default:
            // LOG
            break;
    }

    return ret;
}

template <typename T, DictName N>
template <typename... Args>
std::shared_ptr<T> Dict<T, N>::get_record_by_key(Args... keys) {
    if (_dict.size() == 0) {
        std::cerr << "dict is empty";
        return nullptr;
    }

    std::string key = join_param(keys...);

    if (_dict.find(key) != _dict.end()) {
        std::cerr << "found dict key:" << key;
        int record_index = _dict[key];
        return get_record_by_index(record_index);
    } else {
        return nullptr;
    }
}

template <typename T, DictName N>
int Dict<T, N>::load_file(const std::string &dict_file) {
    _dict_filename = dict_file;
    std::ifstream fin(_dict_filename);

    if (!fin) {
        std::cerr << "open file:" << _dict_filename << " failed";
        return 2;
    }

    std::string line;
    std::vector<std::string> lines;

    while (getline(fin, line)) {
        lines.emplace_back(line);
    }

    for (std::string &line : lines) {

        if (this->read_line(line) != 0) {
            std::cerr << "read_line:" << line;
            return -1;
        }
    }

    return 0;
}

template <typename T, DictName N>
int Dict<T, N>::read_line(const std::string &line) {
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

template <typename T, DictName N>
int Dict<T, N>::string_to_message(const std::string &line,
                                  google::protobuf::Message *message,
                                  std::string *p_key) {
    std::vector<std::string> cols;
    const google::protobuf::Descriptor *descriptor = message->GetDescriptor();

    auto &sep = descriptor->options().GetExtension(tardis::separator);

    if (split(cols, line, sep) <= 0) {
        std::cerr << "split failed:" << line;
        return -1;
    }

    int field_count = descriptor->field_count();

    if (field_count > cols.size()) {
        std::cerr << "field_count:" << field_count << "cols:" << cols.size();
        return -1;
    }

    // 记录组合键，也可以是单键
    std::stringstream composite_key;
    bool is_first = true;

    for (int i = 0; i < field_count; ++i) {
        const std::string &col = cols[i];
        auto field = descriptor->field(i);
        std::cout << "field_" << i << " col:" << col
                  << " cpp_type:" << field->cpp_type();

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
                std::cerr << "make field failed:" << col;
                return -1;
            }
        } else {
            auto &del = field->options().GetExtension(tardis::delimiter);
            std::vector<std::string> vs;

            if (split(vs, col, del) <= 0) {
                std::cerr << "split failed:" << vs[1];
                return -1;
            }

            for (int i = 0; i < vs.size(); ++i) {
                std::string &data = vs[i];
                std::cout << "repeated cpp_type:" << field->cpp_type()
                          << " array" << i << "=" << data;
                int ret = make_entry(data, message, field, true);

                if (ret != 0) {
                    std::cerr << "make_entry failed:" << data;
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

template <typename T, DictName N> int Dict<T, N>::get_record_num() {
    return _record.size();
}

template <typename T, DictName N>
std::shared_ptr<T> Dict<T, N>::get_record_by_index(int index) {
    if (index >= _record.size()) {
        return nullptr;
    } else {
        return _record[index];
    }
}

} // namespace tardis
