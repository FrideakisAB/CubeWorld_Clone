#ifndef GRADIENT_H
#define GRADIENT_H

#include <list>
#include "Platform.h"
#include <glm/glm.hpp>
#include "Assets/ISerialize.h"

class Gradient : public ISerialize {
public:
    struct Mark {
        glm::vec4 color;
        f32 position;
    };

    static constexpr size_t CacheSize = 128;
    using MarkIterator = std::list<Mark>::iterator;

private:
    std::list<Mark> marks;
    glm::vec4 cachedValues[CacheSize]{};

    void computeColorAt(f32 position, glm::vec4 *color) const noexcept;

public:
    Gradient();

    [[nodiscard]] glm::vec4 GetColorAt(f32 position) const noexcept;

    void AddMark(f32 position, glm::vec4 color);
    void RemoveMark(MarkIterator element) noexcept;

    [[nodiscard]] const std::list<Mark> &GetMarks() const noexcept { return marks; }
    void RefreshCache();

    json SerializeObj() override;
    void UnSerializeObj(const json &j) override;
};

#endif
