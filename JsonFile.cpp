#include "JsonFile.h"


/**
 * Getter for boolean values, defaults to false
 * @param name
 * @param out
 */
template<>bool JsonFile::Get<bool>(const std::string& name){
    bool out;
    auto valStr = getValString(name);
    std::istringstream(valStr) >> std::boolalpha >> out;
    return out;
}

/**
 * Getter for string values
 * @param name key to search for
 * @param out variable to save result to
 */
template<>std::string JsonFile::Get<std::string>(const std::string &name) {
    std::string out;
    auto subStr = this->getValString(name);
    out = this->getValByDelimiter(subStr, "\"", "\"");
    return out;
}

/**
 * Getter for float values
 * @param name key to search for
 * @param out variable to save the result in
 */
template<>float JsonFile::Get<float>(const std::string& name){
    float out;
    auto valStr = this->getValString(name);
    auto is = std::istringstream(valStr);
    std::string val;
    std::getline(is, val);
    out = std::atof(val.c_str());
    if(std::isnan(out)) out = 0.0f;
    return out;
}

/**
 * getter for int values
 * @param name key to search for
 * @param out variable to save the result in
 */
template<>int JsonFile::Get<int>(const std::string& name){
    int out;
    auto valStr = this->getValString(name);
    auto is = std::istringstream(valStr);
    std::string val;
    std::getline(is, val);
    out = std::atoi(val.c_str());
    if(std::isnan(out)) out = 0;
    return out;
}

/**
 * Getter for objects, constructs a map<str, str> object containing key value pairs of the object
 * @param name key to search for
 * @param out variable to save result in
 */
template<>std::vector<std::map<std::string, std::string>> JsonFile::Get<std::vector<std::map<std::string, std::string>>>(const std::string& name){
    std::vector<std::map<std::string, std::string>> out;
    auto valStr = this->GetArrayString(name);
    auto is = std::istringstream(valStr);
    std::string val;

    out.clear();
    std::map<std::string, std::string> obj;
    bool flush = false;
    while(std::getline(is, val)){
        if(val.find('"') != std::string::npos) {
            std::string key;
            std::string value;
            auto keyStart = val.find('"') + 1;
            auto keyEnd = val.find('"', keyStart);
            key = val.substr(keyStart, keyEnd - keyStart);
            value = GetValStringFromString(key, val);
            obj.insert_or_assign(key, value);
            flush = true;
        } else if(flush) {
            out.push_back(obj);
            obj.clear();
            flush = false;
        }
    }
    return out;
}
template<>std::vector<std::string> JsonFile::Get<std::vector<std::string>>(const std::string& name){
    std::vector<std::string> array;
    auto arrayString = GetArrayString(name);

    char prefix = '{';
    char postfix = '}';
    auto objStart = arrayString.find(prefix);
    if(objStart == std::string::npos){
        prefix = '\"';
        objStart = arrayString.find(prefix);
        if(objStart != std::string::npos){
            postfix = '\"';
        } else {
            return array;
        }
    }
    auto objEnd = objStart + 1;

    while(objStart != std::string::npos && objEnd != std::string::npos){
        // First closing brackets
        objEnd = arrayString.find(postfix, objStart + 1);
        if(objEnd == std::string::npos) break;
        long tempEnd = objEnd;
        long tempStart = objStart;
        while(1){
            tempStart = arrayString.find(prefix, tempStart + 1);
            if(tempStart != std::string::npos){
                if(tempStart < tempEnd) {
                    tempEnd = arrayString.find(postfix, tempEnd + 1);
                } else { break; }
            } else {
                break;
            }
        }
        objEnd = tempEnd != std::string::npos ? tempEnd : objStart - 1;
        array.push_back(arrayString.substr(objStart, objEnd - objStart+1));
        objStart = arrayString.find(prefix, objEnd + 1);
    }
    return array;
}

