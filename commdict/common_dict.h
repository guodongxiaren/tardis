// @filename:   common_dict.h
// @author:     guodongxiaren
// @biref:      parse data
// @date:       2021-07-18

#pragma once

#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <glog/logging.h>
#include <google/protobuf/descriptor.h>

#include "commdict-key.pb.h"
#include "common.h"
#include "factory.h"

template<typename T>
class CommonDict {
private:
    // @brief:     构造函数
    CommonDict(const std::string& dict_filename): _dict_filename(dict_filename) {};

public:
    // @brief:     通过key查找记录
    // @param[in]  keys 词表的key字段，可变参数，支持多个字段做联合的key
    // @return     shared_ptr版本的值，即单行记录的对应的结构体
    template<typename ...Args>
    std::shared_ptr<T> get_record_by_key(Args... keys);

    // @brief:     通过行号下标获取记录
    // @param[in]  index 下标，即文件的行数，从0计数
    // @return     shared_ptr版本的值，即单行数据的对应的结构体
    std::shared_ptr<T> get_record_by_index(int index);

    // @brief:     获取词表行数
    // @return     词表的行数
    int get_record_num();

    // @brief:     指定词表文件路径，构造词表类实例
    // @param[in]  dict_filename词表路径
    // @return     shared_ptr版本的词表结构体指针
    static std::shared_ptr<CommonDict<T>> get_instance(const std::string& dict_filename);

private:
    // @breif:      解析单行数据
    // @param[in]:  line 数据字符串
    // @return:     是否成功 0:成功 -1失败
    int read_line(const std::string& line);

    // @breif:      加载并解析词表
    // @return:     是否成功 0:成功 -1失败
    int load_file();

    // @breif:      反射构造自定义类型对象
    // @param[in]:  col 表示列的字符串
    // @param[out]: entry 要构造的自定义类型对象
    // @param[in]:  field protobuf获取字段信息反射对象
    // @param[in]:  is_repeated 表示是否是数组
    // @param[in]:  i 如果是数组，表示是所位于的数组下标
    // @return:     是否成功 0:成功 -1失败
    static int make_entry(const std::string& col, std::shared_ptr<T>& sp_entry,
                          const google::protobuf::FieldDescriptor* field, bool is_repeated = false);

private:
    std::map<std::string, int> _dict; // key-value 词表, value是记录对应的行号下标
    std::vector<std::shared_ptr<T>> _record; // 存储每行记录
    std::string _dict_filename; // 词表文件路径

    static std::map<std::string, std::shared_ptr<CommonDict<T>>>
    _s_dict; // 单例所需的map，key为词表路径，一个词表文件只能实例化出一个CommonDict
    static std::mutex _s_lock; // 互斥锁
};
/* static */
template<typename T>
std::map<std::string, std::shared_ptr<CommonDict<T>>> CommonDict<T>::_s_dict;

template<typename T>
std::mutex CommonDict<T>::_s_lock;

template<typename T>
std::shared_ptr<CommonDict<T>> CommonDict<T>::get_instance(const std::string& dict_filename) {
    // 互斥锁
    std::lock_guard<std::mutex> lock(_s_lock);

    if (_s_dict.find(dict_filename) == _s_dict.end()) {
        CommonDict<T>* dict = new(std::nothrow)CommonDict<T>(dict_filename);

        if (dict == nullptr) {
            LOG(FATAL)<< "new Dict:"<< dict_filename;
        return nullptr;
    }

    if (dict->load_file() != 0) {
            LOG(FATAL)<< "dict:"<< dict_filename << " load failed";
            return nullptr;
        }

        _s_dict[dict_filename] = std::shared_ptr<CommonDict<T>>(dict);
    }

    return _s_dict[dict_filename];
}

template <typename T>
int CommonDict<T>::make_entry(const std::string& col, std::shared_ptr<T>& sp_entry,
                              const google::protobuf::FieldDescriptor* field, bool is_repeated) {
    T* entry = sp_entry.get();
    const google::protobuf::Reflection* reflection = entry->GetReflection();

    switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL: {
        bool value = (stoi(col) == 1 ? true : false);

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

        parse_function parse = Factory::get(mname);

        if (parse == nullptr) {
            LOG(ERROR)<< "get parse func failed:"<< mname;
            return -1;
        }

        if (parse(col, msg) != 0) {
            LOG(ERROR)<< "parse [" << mname << "][" << col << "%s] failed";
            return -1;
        }

        break;
    }

    default:
        break;
    }

    return 0;
}

/* non static */
template<typename T>
template<typename ...Args>
std::shared_ptr<T> CommonDict<T>::get_record_by_key(Args... keys) {
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

template<typename T>
int CommonDict<T>::load_file() {
    using std::string;
    using std::vector;
    using std::ifstream;

    ifstream fin(_dict_filename);

    if (!fin) {
        LOG(ERROR)<< "open file:" << _dict_filename << " failed";
        return 2;
    }

    string line;
    vector<string> lines;

    while (getline(fin, line)) {
        lines.emplace_back(line);
    }

    for (string & line : lines) {

        if (this->read_line(line) != 0) {
            LOG(ERROR)<< "read_line:"<< line;
            return -1;
        }
    }

    return 0;
}

template<typename T>
int CommonDict<T>::read_line(const std::string& line) {
    std::vector<std::string> cols;

    if (split(cols, line, "\t") <= 0) {
        LOG(ERROR)<< "split failed:"<< line;
        return -1;
    }

    std::shared_ptr<T> entry = std::make_shared<T>();
    const google::protobuf::Descriptor* descriptor = entry->GetDescriptor();

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
        LOG(INFO)<< "field_" << i <<" col:"<< col;
        auto field = descriptor->field(i);
        bool key_opt = field->options().GetExtension(commdict::key);

        if (key_opt) {
            if (is_first) {
                composite_key << col;
                is_first = false;
            } else {
                composite_key << '\t' << col;
            }
        }

        if (!field->is_repeated()) {
            LOG(INFO)<< "cpp_type:"<< field->cpp_type();
            int ret = make_entry(col, entry, field);

            if (ret != 0) {
                LOG(ERROR)<< "make_entry failed:"<< col;
                return -1;
            }
        } else {
            std::vector<std::string> vs;

            if (split(vs, col, ",") <= 0) {
                LOG(ERROR)<< "split failed:"<< vs[1];
                return -1;
            }

            for (int i = 0; i < vs.size(); ++i) {
                std::string& data = vs[i];
                LOG(INFO)<< "repeated cpp_type:" << field->cpp_type() <<" array" << i << "=" << data;
                int ret = make_entry(data, entry, field, true);

                if (ret != 0) {
                    LOG(ERROR)<< "make_entry failed:"<< data;
                    return -1;
                }
            }
        }
    }

    int next_record_index = _record.size();
    _record.emplace_back(entry);
    // 如果词表没设置键，则行记录做键
    std::string key = composite_key.str();

    if (is_first) {
        std::string key = line;
    }

    _dict[key] = next_record_index;

    return 0;
}

template<typename T>
int CommonDict<T>::get_record_num() {
    return _record.size();
}

template<typename T>
std::shared_ptr<T> CommonDict<T>::get_record_by_index(int index) {
    if (index >= _record.size()) {
        return nullptr;
    } else {
        return _record[index];
    }
}

