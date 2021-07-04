#ifndef RedisCommandsH
#define RedisCommandsH

#include <functional>
#include <string>
#include <vector>

class RedisCommand {
public:
    [[nodiscard]] RedisCommand(std::string_view name, int arity,
        std::vector<std::string_view> flags, unsigned int first_key_position, int last_key_position,
        unsigned int step_count, std::vector<std::string_view> tags,
        std::function<std::string(const std::vector<std::string>&)> callback);
    [[nodiscard]] auto to_string() const noexcept;
    [[nodiscard]] auto get_name() const noexcept;
    [[nodiscard]] auto get_arity() const noexcept;
    [[nodiscard]] auto get_callback() const noexcept;
private:
    std::string_view m_name;
    int m_arity;
    std::vector<std::string_view> m_flags;
    unsigned int m_first_key_position;
    int m_last_key_position;
    unsigned int m_step_count;
    std::vector<std::string_view> m_tags;
    std::function<std::string(const std::vector<std::string>&)> m_callback;
};

class RedisCommands {
public:
    [[nodiscard]] static auto get_command(std::string_view str) noexcept;
    [[nodiscard]] static auto get_response(const std::vector<std::string>& parsed_request) noexcept
        -> std::string;
    [[nodiscard]] constexpr static auto size() noexcept;
    [[nodiscard]] static auto to_string() noexcept;
private:
    static std::array<RedisCommand, 3> m_commands;
    static std::pair<bool, std::string> validate_request(
        const std::vector<std::string>& parsed_request);
};

#endif
