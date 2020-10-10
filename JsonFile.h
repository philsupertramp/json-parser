#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cmath>

/**
 * Broad implementation of a JSON file parser, no layout it enforced yet it is highly
 * suggested to follow regular Json syntax.
 *
 * Current feature support:
 * - int/float/string
 * - Array/Object
 * - Array[Object]
 *
 *
 * Type map:
 * [Json -> Internal]
 *
 * string -> std::string
 * float -> float
 * int -> int
 * Object -> std::map<string, string>
 * Array -> std::vector<>
 * Array[Object] -> std::vector<std::map<std::string, std::string>>
 *
 */
class JsonFile
{
    std::string m_Content;
public:
    JsonFile(const std::string &fileContent)
    : m_Content(fileContent){ }

    const std::string& GetContent() { return m_Content; }

    template<class T>
    T Get(const std::string &name){}

    /**
     * Returns an value string if existing, in case of not existing value empty string
     * @param name key to search for
     * @param in input string
     * @param skipElements for arrays only, this lets you retrieve strictly sized arrays
     * @return string containing the value or empty string.
     */
    std::string GetValStringFromString(const std::string& name, const std::string& in, int skipElements = 0){
        std::string key;
        std::string contentStr;
        if(IsNestedValue(name)){
            contentStr = GetObjString(name.substr(0, name.find('.') - 1));
            key = name.substr(name.find('.') + 1, name.size() - name.find('.'));
        } else {
            key = name;
            contentStr = in;
        }
        auto index = contentStr.find(("\"%s\"", key));
        if (index == std::string::npos) {
            return "";
        }
        // we assume a clean structure and don't validate (for now)
        auto valStart = contentStr.find(':', index) + 1;
        size_t valEnd = valStart;
        auto ind = valStart;
        auto c = contentStr[ind];
        while(c == ' '){
            ind++;
            c = contentStr[ind];
        }
        std::string endChar;
        switch (int(c)) {
            case int('{'):
                endChar = "}";
                break;
            case int('['):
                endChar = "]";
                break;
            case int('\"'):
                endChar = "\"";
                break;
            default:
                endChar = ",";
                break;
        }
        if(skipElements >= 0) {
            for (int i = 0; i < skipElements + 1; i++) {
                valEnd = contentStr.find(',', valEnd + 1);
            }
        }
        else {
            valEnd = contentStr.find(endChar, valEnd + 1);
        }
        // if last value in obj
        if (valEnd == std::string::npos) {
            valEnd = contentStr.find('\n', valStart + 1);
            // if compressed/reduced json file
            if (valEnd == std::string::npos) {
                valEnd = contentStr.find(endChar, valStart + 1);
            }
        }
        return contentStr.substr(valStart+1, valEnd - valStart - 1);
    }

    /**
     * Searches an object by key within the content string
     * @param key: key to search for
     * @param in: input string to search in
     * @return: value in case of existing object
     */
    std::string GetObjStringFromString(const std::string& key, const std::string& in){
        auto index = in.find(("\"%s\"", key));
        if (index == std::string::npos) {
            return "";
        }
        int i;
        // we assume a clean structure and don't validate (for now)
        auto objStart = in.find(':', index) + 1;
        // Starting point
        objStart = in.find('{', objStart);

        // First closing brackets
        auto objEnd = in.find('}', objStart);
        int tempEnd = objEnd;
        int tempStart = objStart;
        while(1){
            tempStart = in.find('{', tempStart + 1);
            if(tempStart != std::string::npos){
                if(tempStart < tempEnd) {
                    tempEnd = in.find('}', tempEnd + 1);
                } else { break; }
            } else { break; }
        }
        objEnd = tempEnd != std::string::npos ? tempEnd : objStart;
        return in.substr(objStart, objEnd - objStart + 1);
    }

    /**
     * Getter for array type values
     * @param key: to search for
     * @return : array object string
     */
    std::string GetArrayString(const std::string& key){
        return GetArrayStringFromString(key, GetContent());
    }
    std::string GetArrayStringFromString(const std::string& key, const std::string& in){
        auto index = GetContent().find(("\"%s\"", key));
        if (index == std::string::npos) {
            return "";
        }
        // we assume a clean structure and don't validate (for now)
        auto arrayStart = in.find(':', index) + 1;
        // Starting point
        arrayStart = in.find('[', arrayStart);

        // First closing brackets
        auto arrayEnd = in.find(']', arrayStart);
        int tempEnd = arrayEnd;
        int tempStart = arrayStart;
        while(1){
            tempStart = in.find('[', tempStart + 1);
            if(tempStart != std::string::npos){
                if(tempStart < tempEnd) {
                    tempEnd = in.find(']', tempEnd + 1);
                } else { break; }
            } else { break; }
        }

        arrayEnd = tempEnd != std::string::npos ? tempEnd : arrayStart - 1;
        return in.substr(arrayStart, arrayEnd - arrayStart + 1);
    }

    /**
     * Shortcut for GetObjStringFromString on attached content
     */
    std::string GetObjString(const std::string& key){
        return GetObjStringFromString(key, GetContent());
    }

private:

    /**
     * test for nested attribute access, since nested attributes will be accessed as
     * "parent.child.attribute"
     * @param in input key name
     * @return
     */
    bool IsNestedValue(const std::string& in){
        return in.find(".") != std::string::npos;
    }

    /**
     * Getter for special character delimited objects, i.e. Strings/Objects/Arrays, but is not strictly bound to
     * these types.
     * @param in input string
     * @param start start delimiter
     * @param end end delimiter
     * @return value string
     */
    std::string getValByDelimiter(const std::string &in, const std::string &start, const std::string &end) {
        auto valStart = in.find(start);
        if (valStart == std::string::npos) {
            printf("%s", ("JsonFile::Get(): Can't find start delimiter " + start).c_str());
        }
        auto valEnd = in.find(end, valStart + 1);
        if (valEnd == std::string::npos) {
            printf("%s", ("JsonFile::Get(): Can't find end delimiter " + end).c_str());
        }
        return in.substr(valStart + 1, valEnd - valStart - 1);
    }


    /**
     * shortcut for GetValStringFromString on `content`
     * @param name
     * @param skipElements
     * @return
     */
    std::string getValString(const std::string& name, int skipElements = 0){
        return GetValStringFromString(name, GetContent(), skipElements);
    }
};

/**
 * Getter for boolean values, defaults to false
 * @param name
 */
template<>bool JsonFile::Get<bool>(const std::string& name);

/**
 * Getter for arrays
 * @param name key to search for
 */
template<>std::vector<std::string> JsonFile::Get<std::vector<std::string>>(const std::string& name);

/**
 * Getter for string values
 * @param name key to search for
 */
template<>std::string JsonFile::Get<std::string>(const std::string &name);

/**
 * Getter for float values
 * @param name key to search for
 */
template<>float JsonFile::Get<float>(const std::string& name);

/**
 * getter for int values
 * @param name key to search for
 */
template<>int JsonFile::Get<int>(const std::string& name);

/**
 * Getter for objects, constructs a map<str, str> object containing key value pairs of the object
 * @param name key to search for
 */
template<>std::vector<std::map<std::string, std::string>> JsonFile::Get<std::vector<std::map<std::string, std::string>>>(const std::string& name);
