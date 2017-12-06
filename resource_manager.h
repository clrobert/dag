std::string BoolToReadable(bool val);

std::vector<std::string> SplitString(std::string line);

class Node;

class ResourceManager;

void ExecuteCommand(std::string input, std::vector<Node *> resources);

void PrintMenu();
