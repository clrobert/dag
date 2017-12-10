std::string BoolToReadable(bool val);

std::vector<std::string> SplitString(std::string line);

class Node;

class ResourceManager;

void ExecuteCommand(std::string input, std::forward_list<Node *> resources);

void PrintMenu();

bool DepthFirstSearch(Node* node, std::forward_list<Node*> cycle_stack);

Node* FindNode(std::string name, std::forward_list<Node*> nodes);

void PrintNodeList(std::forward_list<Node*> nodes);

void PrintError(std::string error);