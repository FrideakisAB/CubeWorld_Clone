#ifndef ISERIALIZE_H
#define ISERIALIZE_H

#include <string>
#include "Utils/json.h"

class ISerialize {
public:
	virtual json SerializeObj() { return {}; }
	virtual void UnSerializeObj(const json& j) {}
};

#endif