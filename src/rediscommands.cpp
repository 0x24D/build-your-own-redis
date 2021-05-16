#include "rediscommands.h"
#include <algorithm>

RedisCommand::RedisCommand(std::string name, int arity, std::vector<std::string> flags,
    unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount,
    std::vector<std::string> tags,
    std::function<std::string(const std::vector<std::string>&)> callback)
    : m_name(name),
      m_arity(arity),
      m_flags(flags),
      m_firstKeyPosition(firstKeyPosition),
      m_lastKeyPosition(lastKeyPosition),
      m_stepCount(stepCount),
      m_tags(tags),
      m_callback(callback) {}

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

auto RedisCommand::getCallback() const noexcept {
    return m_callback;
}

auto RedisCommands::getResponse(const std::vector<std::string>& parsedRequest) noexcept
    -> std::string {
    std::string response;
    for (const auto& command : m_commands) {
        std::string request;
        std::ranges::transform(parsedRequest[0], std::back_inserter(request), [](unsigned char c) {
            return std::tolower(c);
        });
        if (request == command.getName()) {
            response = command.getCallback()(parsedRequest);
            break;
        }
    }
    return response;
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

// Free functions
namespace {
    auto getArgumentAsBulkString(const std::vector<std::string>& parsedRequest) {
        std::string str = "$";
        str += std::to_string(parsedRequest[1].size());
        str += "\r\n";
        str += parsedRequest[1];
        str += "\r\n";
        return str;
    }
}

// Command response callbacks
namespace ResponseCallbacks {
    // TODO: Handle COMMAND with additional args.
    auto command(const std::vector<std::string>&) {
        return RedisCommands::toString();
    }

    auto echo(const std::vector<std::string>& parsedRequest) {
        return getArgumentAsBulkString(parsedRequest);
    }

    auto ping(const std::vector<std::string>& parsedRequest) {
        if (parsedRequest.size() == 1) {
            return std::string{"+PONG\r\n"};
        } else {
            return getArgumentAsBulkString(parsedRequest);
        }
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
