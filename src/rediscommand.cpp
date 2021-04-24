#include "rediscommand.h"

RedisCommand::RedisCommand(std::string name, int arity, std::vector<std::string> flags, unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount, std::vector<std::string> tags) : m_name(name), m_arity(arity), m_flags(flags), m_firstKeyPosition(firstKeyPosition), m_lastKeyPosition(lastKeyPosition), m_stepCount(stepCount), m_tags(tags) {}

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
