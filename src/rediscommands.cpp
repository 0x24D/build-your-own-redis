#include "rediscommands.h"
#include <algorithm>
#include <optional>
#include <utility>

RedisCommand::RedisCommand(std::string_view name, int arity, std::vector<std::string_view> flags,
    unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount,
    std::vector<std::string_view> tags,
    std::function<std::string(const std::vector<std::string>&)> callback)
    : m_name(name),
      m_arity(arity),
      m_flags(std::move(flags)),
      m_firstKeyPosition(firstKeyPosition),
      m_lastKeyPosition(lastKeyPosition),
      m_stepCount(stepCount),
      m_tags(std::move(tags)),
      m_callback(std::move(callback)) {}

auto RedisCommand::toString() const noexcept {
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
    str += std::to_string(m_firstKeyPosition);
    str += "\r\n";
    str += ':';
    str += std::to_string(m_lastKeyPosition);
    str += "\r\n";
    str += ':';
    str += std::to_string(m_stepCount);
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

auto RedisCommand::getName() const noexcept {
    return m_name;
}

auto RedisCommand::getArity() const noexcept {
    return m_arity;
}

auto RedisCommand::getCallback() const noexcept {
    return m_callback;
}

// Free functions
namespace {
    auto toLower(std::string_view view) {
        std::string str;
        std::ranges::transform(view, std::back_inserter(str), [](auto c) {
            return std::tolower(c);
        });
        return str;
    }
    auto getArgumentAsBulkString(const std::vector<std::string>& parsedRequest) {
        std::string str{'$'};
        str += std::to_string(parsedRequest[1].size());
        str += "\r\n";
        str += parsedRequest[1];
        str += "\r\n";
        return str;
    }
}

auto RedisCommands::getCommand(std::string_view str) noexcept {
    const auto found = std::ranges::find_if(m_commands, [str](const auto& cmd) {
        return cmd.getName() == toLower(str);
    });
    return found != m_commands.end() ? std::optional<RedisCommand>(*found) : std::nullopt;
}

auto RedisCommands::getResponse(const std::vector<std::string>& parsedRequest) noexcept
    -> std::string {
    const auto [validRequest, errorMessage] = validateRequest(parsedRequest);
    if (validRequest) {
        // TODO: Return response from validateRequest?
        //  We currently have to getCommand twice if the request is valid.
        const auto command = RedisCommands::getCommand(parsedRequest[0]).value();
        std::string response = command.getCallback()(parsedRequest);
        return response;
    } else {
        return errorMessage;
    }
}

constexpr auto RedisCommands::size() noexcept {
    return m_commands.size();
}

auto RedisCommands::toString() noexcept {
    std::string str{'*'};
    str += std::to_string(m_commands.size());
    str += "\r\n";
    for (const auto& command : m_commands) {
        str += command.toString();
    }
    return str;
}

auto RedisCommands::validateRequest(const std::vector<std::string>& parsedRequest)
    -> std::pair<bool, std::string> {
    // TODO: Currently assuming parsedRequest isn't empty, can it be?
    const auto optional = RedisCommands::getCommand(parsedRequest[0]);
    if (!optional.has_value()) {
        return {false, "-ERR unknown command '" + parsedRequest[0] + "'\r\n"};
    }
    const auto& command = optional.value();

    // Check number of arguments
    const auto arity = command.getArity();
    const auto numberOfArguments = parsedRequest.size();
    // Eithet too many arguments provided, or
    if ((arity >= 0 && static_cast<std::size_t>(arity) < numberOfArguments) ||
        // not enough arguments provided
        static_cast<std::size_t>(std::abs(arity)) > numberOfArguments) {
        return {false, "-ERR wrong number of arguments for '" + std::string{command.getName()} +
                           "' command\r\n"};
    }

    return {true, ""};
}

// Command response callbacks
namespace ResponseCallbacks {
    // TODO: Handle COMMAND GETKEYS.
    auto command(const std::vector<std::string>& parsedRequest) {
        if (parsedRequest.size() == 1) {
            return RedisCommands::toString();
        } else {
            const auto arg = toLower(parsedRequest[1]);
            if (arg == "count") {
                std::string str{':'};
                str += std::to_string(RedisCommands::size());
                str += "\r\n";
                return str;
            } else if (arg == "info") {
                const auto numArgs = parsedRequest.size() - 2;
                std::string str{'*'};
                str += std::to_string(numArgs);
                str += "\r\n";
                for (std::size_t i = 2; i < parsedRequest.size(); ++i) {
                    const auto cmd = RedisCommands::getCommand(parsedRequest[i]);
                    str += cmd ? cmd.value().toString() : "*0\r\n";
                }
                return str;
            }

            return "-ERR Unknown subcommand or wrong number of arguments for '" + parsedRequest[1] +
                   "'\r\n";
        }
    }

    auto echo(const std::vector<std::string>& parsedRequest) {
        return getArgumentAsBulkString(parsedRequest);
    }

    auto ping(const std::vector<std::string>& parsedRequest) {
        // TODO: In theory this error handling should be in validateRequest, but the command
        // properties for ping can't be used in a way to generically check for too many arguments -
        // e.g. arity (from Redis docs) is -1 so it can have 1 or more arguments (including 'ping'
        // key), but in reality either 1 or 2 arguments are required, more than 2 is an error.
        switch (parsedRequest.size()) {
            case 1:
                return std::string{"+PONG\r\n"};
            case 2:
                return getArgumentAsBulkString(parsedRequest);
        }
        return "-ERR wrong number of arguments for '" + toLower(parsedRequest[0]) + "' command\r\n";
    }
}

// Instantiate const m_commands member variable.
std::array<RedisCommand, 3> RedisCommands::m_commands = []() {
    const RedisCommand commandCmd{"command", -1, {"random", "loading", "stale"}, 0, 0, 0,
        {"@slow", "@connection"}, ResponseCallbacks::command};
    const RedisCommand echoCmd{
        "echo", 2, {"fast"}, 0, 0, 0, {"@fast", "@connection"}, ResponseCallbacks::echo};
    const RedisCommand pingCmd{
        "ping", -1, {"stale", "fast"}, 0, 0, 0, {"@fast", "@connection"}, ResponseCallbacks::ping};
    return std::array{commandCmd, echoCmd, pingCmd};
}();
