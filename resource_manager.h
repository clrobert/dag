
class Node;

std::string BoolToReadable(bool val);

Node* FindResource(std::string name, std::vector<Node *> resources);

bool Deleted(Node* node);

std::vector<Node *> DeleteNode(std::string name, std::vector<Node *> resources);

void InsertResource(std::string name, std::string dependency);

void RemoveResource(std::string name);

void CheckCycles();

void DisplayGraph();

void HandleInput();

std::vector<std::string> SplitString(std::string line);
