#ifndef RedisCommandsH
#define RedisCommandsH

#include <functional>
#include <string>
#include <vector>

class RedisCommand {
public:
    [[nodiscard]] RedisCommand(std::string name, int arity, std::vector<std::string> flags,
        unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount,
        std::vector<std::string> tags,
        std::function<std::string(const std::vector<std::string>&)> callback);
    [[nodiscard]] auto toString() const noexcept;
    [[nodiscard]] auto getName() const noexcept;
    [[nodiscard]] auto getCallback() const noexcept;
private:
    std::string m_name;
    int m_arity;
    std::vector<std::string> m_flags;
    unsigned int m_firstKeyPosition;
    int m_lastKeyPosition;
    unsigned int m_stepCount;
    std::vector<std::string> m_tags;
    std::function<std::string(const std::vector<std::string>&)> m_callback;
};

class RedisCommands {
public:
    [[nodiscard]] static auto getResponse(const std::vector<std::string>& parsedRequest) noexcept
        -> std::string;
    [[nodiscard]] static auto toString() noexcept;
private:
    static std::array<RedisCommand, 3> m_commands;
};

#endif
