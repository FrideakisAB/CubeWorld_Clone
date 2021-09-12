#ifndef ISERIALIZE_H
#define ISERIALIZE_H

#include <string>
#include "Utils/json.h"

class ISerialize {
public:
	[[nodiscard]] virtual json SerializeObj() const { return {}; }
	virtual void UnSerializeObj(const json &j) {}
};

#endif