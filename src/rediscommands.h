#ifndef RedisCommandsH
#define RedisCommandsH

#include <functional>
#include <string>
#include <vector>

class RedisCommand {
public:
    [[nodiscard]] RedisCommand(std::string_view name, int arity,
        std::vector<std::string_view> flags, unsigned int firstKeyPosition, int lastKeyPosition,
        unsigned int stepCount, std::vector<std::string_view> tags,
        std::function<std::string(const std::vector<std::string>&)> callback);
    [[nodiscard]] auto toString() const noexcept;
    [[nodiscard]] auto getName() const noexcept;
    [[nodiscard]] auto getCallback() const noexcept;
private:
    std::string_view m_name;
    int m_arity;
    std::vector<std::string_view> m_flags;
    unsigned int m_firstKeyPosition;
    int m_lastKeyPosition;
    unsigned int m_stepCount;
    std::vector<std::string_view> m_tags;
    std::function<std::string(const std::vector<std::string>&)> m_callback;
};

class RedisCommands {
public:
    [[nodiscard]] static auto getCommand(std::string_view str) noexcept;
    [[nodiscard]] static auto getResponse(const std::vector<std::string>& parsedRequest) noexcept
        -> std::string;
    [[nodiscard]] constexpr static auto size() noexcept;
    [[nodiscard]] static auto toString() noexcept;
private:
    static std::array<RedisCommand, 3> m_commands;
};

#endif
