std::string BoolToReadable(bool val);

std::vector<std::string> SplitString(std::string line);

class Node;

class ResourceManager;

void ExecuteCommand(std::string input, std::vector<Node *> resources);

void PrintMenu();

bool DepthFirstSearch(Node* node, std::vector<Node*> cycle_stack);

Node* Find(std::string name, std::vector<Node*> nodes);
