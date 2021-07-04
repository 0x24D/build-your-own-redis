#include "rediscommands.h"
#include <algorithm>
#include <optional>
#include <utility>

RedisCommand::RedisCommand(std::string_view name, int arity, std::vector<std::string_view> flags,
    unsigned int first_key_position, int last_key_position, unsigned int step_count,
    std::vector<std::string_view> tags,
    std::function<std::string(const std::vector<std::string>&)> callback)
    : m_name(name),
      m_arity(arity),
      m_flags(std::move(flags)),
      m_first_key_position(first_key_position),
      m_last_key_position(last_key_position),
      m_step_count(step_count),
      m_tags(std::move(tags)),
      m_callback(std::move(callback)) {}

auto RedisCommand::to_string() const noexcept {
    std::string str{"*7\r\n"};
    str += '$';
    str += std::to_string(m_name.size());
    str += "\r\n";
    str += m_name;
    str += "\r\n";
    str += ':';
    str += std::to_string(m_arity);
    str += "\r\n";
    str += '*';
    str += std::to_string(m_flags.size());
    str += "\r\n";
    for (const auto& flag : m_flags) {
        str += '+';
        str += flag;
        str += "\r\n";
    }
    str += ':';
    str += std::to_string(m_first_key_position);
    str += "\r\n";
    str += ':';
    str += std::to_string(m_last_key_position);
    str += "\r\n";
    str += ':';
    str += std::to_string(m_step_count);
    str += "\r\n";
    str += '*';
    str += std::to_string(m_tags.size());
    str += "\r\n";
    for (const auto& tag : m_tags) {
        str += '+';
        str += tag;
        str += "\r\n";
    }
    return str;
}

auto RedisCommand::get_name() const noexcept {
    return m_name;
}

auto RedisCommand::get_arity() const noexcept {
    return m_arity;
}

auto RedisCommand::get_callback() const noexcept {
    return m_callback;
}

// Free functions
namespace {
    auto to_lower(std::string_view view) {
        std::string str;
        std::ranges::transform(view, std::back_inserter(str), [](auto c) {
            return std::tolower(c);
        });
        return str;
    }
    auto get_argument_as_bulk_string(const std::vector<std::string>& parsed_request) {
        std::string str{'$'};
        str += std::to_string(parsed_request[1].size());
        str += "\r\n";
        str += parsed_request[1];
        str += "\r\n";
        return str;
    }
}

auto RedisCommands::get_command(std::string_view str) noexcept {
    const auto found = std::ranges::find_if(m_commands, [str](const auto& cmd) {
        return cmd.get_name() == to_lower(str);
    });
    return found != m_commands.end() ? std::optional<RedisCommand>(*found) : std::nullopt;
}

auto RedisCommands::get_response(const std::vector<std::string>& parsed_request) noexcept
    -> std::string {
    const auto [valid_request, error_message] = validate_request(parsed_request);
    if (valid_request) {
        // TODO: Return response from validate_request?
        //  We currently have to get_command twice if the request is valid.
        const auto command = RedisCommands::get_command(parsed_request[0]).value();
        std::string response = command.get_callback()(parsed_request);
        return response;
    } else {
        return error_message;
    }
}

constexpr auto RedisCommands::size() noexcept {
    return m_commands.size();
}

auto RedisCommands::to_string() noexcept {
    std::string str{'*'};
    str += std::to_string(m_commands.size());
    str += "\r\n";
    for (const auto& command : m_commands) {
        str += command.to_string();
    }
    return str;
}

auto RedisCommands::validate_request(const std::vector<std::string>& parsed_request)
    -> std::pair<bool, std::string> {
    // TODO: Currently assuming parsed_request isn't empty, can it be?
    const auto optional = RedisCommands::get_command(parsed_request[0]);
    if (!optional.has_value()) {
        return {false, "-ERR unknown command '" + parsed_request[0] + "'\r\n"};
    }
    const auto& command = optional.value();

    // Check number of arguments
    const auto arity = command.get_arity();
    const auto number_of_arguments = parsed_request.size();
    // Eithet too many arguments provided, or
    if ((arity >= 0 && static_cast<std::size_t>(arity) < number_of_arguments) ||
        // not enough arguments provided
        static_cast<std::size_t>(std::abs(arity)) > number_of_arguments) {
        return {false, "-ERR wrong number of arguments for '" + std::string{command.get_name()} +
                           "' command\r\n"};
    }

    return {true, ""};
}

// Command response callbacks
namespace ResponseCallbacks {
    // TODO: Handle COMMAND GETKEYS.
    auto command(const std::vector<std::string>& parsed_request) {
        if (parsed_request.size() == 1) {
            return RedisCommands::to_string();
        } else {
            const auto arg = to_lower(parsed_request[1]);
            if (arg == "count") {
                std::string str{':'};
                str += std::to_string(RedisCommands::size());
                str += "\r\n";
                return str;
            } else if (arg == "info") {
                const auto num_args = parsed_request.size() - 2;
                std::string str{'*'};
                str += std::to_string(num_args);
                str += "\r\n";
                for (std::size_t i = 2; i < parsed_request.size(); ++i) {
                    const auto cmd = RedisCommands::get_command(parsed_request[i]);
                    str += cmd ? cmd.value().to_string() : "*0\r\n";
                }
                return str;
            }

            return "-ERR Unknown subcommand or wrong number of arguments for '" +
                   parsed_request[1] + "'\r\n";
        }
    }

    auto echo(const std::vector<std::string>& parsed_request) {
        return get_argument_as_bulk_string(parsed_request);
    }

    auto ping(const std::vector<std::string>& parsed_request) {
        // TODO: In theory this error handling should be in validate_request, but the command
        // properties for ping can't be used in a way to generically check for too many arguments -
        // e.g. arity (from Redis docs) is -1 so it can have 1 or more arguments (including 'ping'
        // key), but in reality either 1 or 2 arguments are required, more than 2 is an error.
        switch (parsed_request.size()) {
            case 1:
                return std::string{"+PONG\r\n"};
            case 2:
                return get_argument_as_bulk_string(parsed_request);
        }
        return "-ERR wrong number of arguments for '" + to_lower(parsed_request[0]) +
               "' command\r\n";
    }
}

// Instantiate const m_commands member variable.
std::array<RedisCommand, 3> RedisCommands::m_commands = []() {
    const RedisCommand command_cmd{"command", -1, {"random", "loading", "stale"}, 0, 0, 0,
        {"@slow", "@connection"}, ResponseCallbacks::command};
    const RedisCommand echo_cmd{
        "echo", 2, {"fast"}, 0, 0, 0, {"@fast", "@connection"}, ResponseCallbacks::echo};
    const RedisCommand ping_cmd{
        "ping", -1, {"stale", "fast"}, 0, 0, 0, {"@fast", "@connection"}, ResponseCallbacks::ping};
    return std::array{command_cmd, echo_cmd, ping_cmd};
}();
