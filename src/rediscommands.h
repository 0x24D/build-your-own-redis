#ifndef RedisCommandsH
#define RedisCommandsH

#include <functional>
#include <string>
#include <vector>

class RedisCommand {
public:
    RedisCommand(std::string name, int arity, std::vector<std::string> flags, unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount, std::vector<std::string> tags, std::function<std::string(const std::vector<std::string>&)> response);
    const std::string toString() const;
    const std::string getName() const;
    const std::function<std::string(const std::vector<std::string>&)> getResponse() const;
private:    
    std::string m_name;
    int m_arity;
    std::vector<std::string> m_flags;
    unsigned int m_firstKeyPosition;
    int m_lastKeyPosition;
    unsigned int m_stepCount;
    std::vector<std::string> m_tags;
    std::function<std::string(const std::vector<std::string>&)> m_response;
};

class RedisCommands {
public:
    static const std::string getResponse(const std::vector<std::string>& parsedRequest);
    static const std::string toString();
private:
    static std::array<RedisCommand, 2> m_commands;
};

#endif
