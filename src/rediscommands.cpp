#include "rediscommands.h"
#include <algorithm>

RedisCommand::RedisCommand(std::string name, int arity, std::vector<std::string> flags, unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount, std::vector<std::string> tags, std::function<std::string(const std::vector<std::string>&)> response) : m_name(name), m_arity(arity), m_flags(flags), m_firstKeyPosition(firstKeyPosition), m_lastKeyPosition(lastKeyPosition), m_stepCount(stepCount), m_tags(tags), m_response(response) {}

const std::string RedisCommand::toString() const {
    std::string str {"*7\r\n"};
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

const std::string RedisCommand::getName() const {
    return m_name;
}

const std::function<std::string(const std::vector<std::string>&)> RedisCommand::getResponse() const {
    return m_response;
}

std::array<RedisCommand, 2> RedisCommands::m_commands = [](){
    const auto commandCmdResponse = [](const std::vector<std::string>&) { return RedisCommands::toString(); }; // TODO: Handle COMMAND with additional args.
    const auto pingCmdResponse = [](const std::vector<std::string>& parsedRequest) {
        std::string str {};
        if (parsedRequest.size() == 1) {
            str = "+PONG\r\n";
        } else {
            str = '$';
            str += std::to_string(parsedRequest[1].size());
            str += "\r\n";
            str += parsedRequest[1];
            str += "\r\n";
        }
        return str;
    };

    const RedisCommand commandCmd {"command", -1, {"random", "loading", "stale"}, 0, 0, 0, {"@slow", "@connection"}, commandCmdResponse};
    const RedisCommand pingCmd {"ping", -1, {"stale", "fast"}, 0, 0, 0, {"@fast", "@connection"}, pingCmdResponse};
    return std::array{commandCmd, pingCmd};
}();

const std::string RedisCommands::getResponse(const std::vector<std::string>& parsedRequest) {
    std::string response {};
    for (const auto& command : m_commands) {
        std::string request {};
        std::ranges::transform(parsedRequest[0], std::back_inserter(request), [](unsigned char c) { return std::tolower(c); });
        if (request == command.getName()) {
            response = command.getResponse()(parsedRequest);
            break;
        }
    }
    return response;
}

const std::string RedisCommands::toString() {
    std::string str {'*'};
    str += std::to_string(m_commands.size());
    str += "\r\n";
    for (const auto& command : m_commands) {
        str += command.toString();
    }
    return str;
}
