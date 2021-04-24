#include <string>
#include <vector>

class RedisCommand {
public:
    RedisCommand(std::string name, int arity, std::vector<std::string> flags, unsigned int firstKeyPosition, int lastKeyPosition, unsigned int stepCount, std::vector<std::string> tags);
    const std::string toString() const;
    const std::string getName() const;
private:    
    std::string m_name;
    int m_arity;
    std::vector<std::string> m_flags;
    unsigned int m_firstKeyPosition;
    int m_lastKeyPosition;
    unsigned int m_stepCount;
    std::vector<std::string> m_tags;
};
