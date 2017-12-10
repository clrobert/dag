std::string BoolToReadable(bool val);

std::vector<std::string> SplitString(std::string line);

class Node;

class ResourceManager;

void ExecuteCommand(std::string input, std::list<Node *> resources);

void PrintMenu();

bool DepthFirstSearch(Node* node, std::list<Node*> cycle_stack);

Node* FindNode(std::string name, std::list<Node*> nodes);

void PrintNodeList(std::list<Node*> nodes);

void PrintError(std::string error);