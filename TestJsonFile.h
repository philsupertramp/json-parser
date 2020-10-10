#pragma once

#include <string>
#include <assert.h>
#include "JsonFile.h"

const std::string smallExample = "{\"float\": 1.0, \"int\": 2, \"string\": \"qwertz\", \"bool\": false}";

bool TestJsonFileBuiltinTypes(){
    auto file = new JsonFile(smallExample);

    assert(file->Get<float>("float") == 1.0f);
    assert(file->Get<int>("int") == 2);
    assert(file->Get<std::string>("string") == "qwertz");
    assert(!file->Get<bool>("bool"));

    return true;
}

const std::string child1Str = \
    "{" \
        "\"config\": \"configs/planet/earth.json\"," \
        "\"radius\": 1," \
        "\"children\": []" \
    "}";

const std::string child2Str = \
    "{" \
        "\"foo\": 10," \
        "\"bar\": -10.09" \
    "}" ;
const std::string childrenStr = \
        "[" \
            + child1Str + ","
        + child2Str \
        + "]";
const std::string objAttrStr = \
    "{" \
        "\"config\": \"configs/scenes/test.json\"," \
        "\"radius\": 1," \
        "\"children\": " + childrenStr \
    + "}";

const std::string objectExample = \
"{" \
    "\"name\": \"Object1\"," \
    "\"type\": \"Fawxy::Scene::Node\"," \
    "\"attributes\":" + objAttrStr \
+ "}";

bool TestJsonFileGetObjString(){
    auto file = new JsonFile(objectExample);
    assert(file->Get<std::string>("name") == "Object1");
    assert(file->Get<std::string>("type") == "Fawxy::Scene::Node");
    auto attributes = file->GetObjString("attributes");
    assert(attributes == objAttrStr);
    return true;
}

bool TestJsonFileGetArrayString(){
    auto file = new JsonFile(objectExample);
    auto attributes = file->GetObjString("attributes");
    auto attrFile = new JsonFile(attributes);
    auto children = attrFile->GetArrayString("children");
    assert(children == childrenStr);
    return true;
}


bool TestJsonFile(){
    TestJsonFileBuiltinTypes();
    TestJsonFileGetObjString();
    TestJsonFileGetArrayString();
    return true;
}