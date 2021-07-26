#ifndef IBINARYSERIALIZE_H
#define IBINARYSERIALIZE_H

#include <fstream>

class IBinarySerialize {
private:
    bool isNeeded;

protected:
    explicit IBinarySerialize(bool binaryNeeded=false) : isNeeded(binaryNeeded) {}

public:
    [[nodiscard]] bool IsBinaryNeeded() const noexcept { return isNeeded; }
    virtual void SerializeBin(std::ofstream &file) {}
    virtual void UnSerializeBin(std::ifstream &file) {}
};

#endif
