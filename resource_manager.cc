#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>

#include "resource_manager.h"

std::string BoolToReadable(bool val) {
    if (val == 1 or val == true) {
        return "yes";
    } else {
        return "no";
    }
}

std::vector<std::string> SplitString(std::string line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens{
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };
    return tokens;
}

class Node {
public:
    std::string name;
    std::list<Node *> links;
    std::list<std::string> dependencies;
    bool deleted = false;
    bool visited = false;

    Node(std::string input_name) {
        name = input_name;
    }

    void Link(Node* link) {
        Node* existing_link = FindLink(link->name);
        if (existing_link == NULL) {

            AddLink(link);

            // If the dependency is already listed, ignore.
            for (std::string dependency : dependencies) {
                if (link->name == dependency) {
                    return;
                }
            }

            // Otherwise, add the dependency to the list.
            AddDependency(link->name);

        } else {
            std::cout << "Link: " << name << " to " << link->name << " already exists. " << std::endl;
        }

    }

    bool Usable() {
        bool usable = true;
        for (std::string dependency : dependencies) {
            if (FindLink(dependency) == nullptr) {
                usable = false;
            }
        }
        for (Node* link : links) {
            if (link->Usable() == false) {
                usable = false;
            }
        }
        return usable;
    }

    void Print() {
        std::cout << "resource: " << name << std::endl;
        std::cout << "dependencies: ";
        for (std::string dependency : dependencies) {
            std::cout << dependency << " ";
        }

        std::cout << std::endl << "dependencies linked: ";
        for (Node* node : links) {
            std::cout << node->name << " ";
        }

        std::cout << std::endl << "usable? " << BoolToReadable(Usable());
        std::cout << '\n' << std::endl;
    }

    Node* FindLink(std::string name) {
        Node* existing_link = NULL;
        for (Node* link : links) {
            if (link->name == name) {
                existing_link = link;
            }
        }
        return existing_link;
    }
private:
    void AddDependency(std::string name) {
        dependencies.push_back(name);
    }

    void AddLink(Node* link) {
        links.push_back(link);
    }
};

Node* FindNode(std::string name, std::list<Node*> nodes) {
    Node* existing_node = NULL;
        for (Node* node : nodes) {
            if (node->name == name) {
                existing_node = node;
            }
        }
    return existing_node;
}

class ResourceManager {
public:
    std::list<Node *> resources;

    ResourceManager(std::string input_filename){
        std::ifstream input_data (input_filename);

        if (input_data.is_open()) {
            for (std::string line; getline(input_data, line); ) {
                std::vector<std::string> tokens = SplitString(line);
                if (!AddResource(tokens[0], tokens[1])) {
                    PrintError("file load error: cycle detected.");
                    return;
                }
            }
            input_data.close();
        }
        std::cout << "file loaded successfully." << std::endl;
    }

    Node* FindResource(std::string name) {
        Node* existing_resource = NULL;

        for (Node* resource : resources) {
            if (resource->name == name) {
                existing_resource = resource;
            }
        }

        return existing_resource;
    }

    /*
        Note: Nodes are stored in the order they're added.
        @ensure The root node is always added first.
            This may change later if I add rebalancing.
    */
    bool AddResource(std::string source_node_name, std::string destination_node_name) {
        Node* source_resource = FindResource(source_node_name);
        Node* destination_resource = FindResource(destination_node_name);

        if (source_resource == NULL) {
            source_resource = new Node(source_node_name);
            resources.push_back(source_resource);
        }

        if (destination_resource == NULL) {
            destination_resource = new Node(destination_node_name);
            resources.push_back(destination_resource);
        }

        source_resource->Link(destination_resource);

        if (CycleDetected()) {
            resources.pop_back();
            return false;
        }

        return true;
    }

    void DeleteResource(std::string name) {
        // remove linked dependencies
        for (Node* resource : resources) {
            Node* link = resource->FindLink(name);
            if (link != nullptr) {
                link->deleted = true;
            }
            resource->links.remove_if([](Node* node) { return node->deleted; });
        }
        // remove resource
        Node* node = FindResource(name);
        if (node != nullptr) {
            node->deleted = true;
            resources.remove_if([](Node* node) { return node->deleted; });
        }
    }

    void PrintResources() {
        for (Node* resource : resources) {
            resource->Print();
        }
    }

    bool CycleDetected(){
        for (Node* node : resources) {
            std::list<Node*> recursion_stack;
            if (CheckCycles(node, recursion_stack, 0) > 0) {
                return true;
            }
            recursion_stack.clear();
        }
        return false;
    }

    int CheckCycles(Node* node, std::list<Node*> recursion_stack, int cycles){
        if (::FindNode(node->name, recursion_stack)) {
            recursion_stack.push_back(node);
            PrintError("resource creation error: cycle detected.");
            PrintError(node->name + " already exists.");
            PrintNodeList(recursion_stack);
            std::cerr << std::endl;
            cycles++;
        } else {
            recursion_stack.push_back(node);
            if (!node->links.empty()) {
                std::list<Node*>::iterator iter;
                for (Node* node : node->links) {
                    cycles += CheckCycles(node, recursion_stack, cycles);
                }
            } else {
                recursion_stack.pop_back();
            }
        }
        return cycles;
    }

    void TraverseConnected() {
        std::cout << "Breadth first: " << std::endl;
        BreadthFirstTraversal();
        std::cout << std::endl;

        std::cout << "Preorder: " << std::endl;
        PreOrderTraversal(resources.front());
        std::cout << std::endl;

        std::cout << "Postorder: " << std::endl;
        PostOrderTraversal(resources.front());
        std::cout << std::endl;
    }

    void PostOrderTraversal(Node* node){
        if (!node->links.empty()) {
            for (Node* link : node->links)  {
                PostOrderTraversal(link);
            }
            std::cout << node->name << std::endl;
        } else {
            std::cout << node->name << std::endl;
        }
    }

    void PreOrderTraversal(Node* node){
        if (!node->links.empty()) {
            std::cout << node->name << std::endl;
            for (Node* link : node->links)  {
                PreOrderTraversal(link);
            }
        } else {
            std::cout << node->name << std::endl;
        }
    }

    void BreadthFirstTraversal() {
        std::cout << resources.front()->name << std::endl;
        BFT(resources.front());
    }

    void BFT(Node* root) {
        if (root->links.empty()) {
            return;
        }
        for (Node* node : root->links) {
            std::cout << node->name << " ";
        }
        std::cout << std::endl;
        for (Node* node : root->links) {
            BFT(node);
        }
    }
};

void PrintNodeList(std::list<Node*> nodes) {
    if (nodes.empty()) {
        return;
    }
    std::string node_list = "";
    for (Node* node : nodes) {
        node_list += node->name + " -> ";
    }
}

void ExecuteCommand(std::string input, ResourceManager* resource_manager){
    std::vector<std::string> args = SplitString(input);
    if (args.size() == 0){
        PrintMenu();
        return;
    }
    if (args[0] == "del" || args[0] == "delete") {
        resource_manager->DeleteResource(args[1]);
    } else if (args[0] == "add") {
        resource_manager->AddResource(args[1], args[2]);
    } else if (args[0] == "show") {
        resource_manager->TraverseConnected();
    } else if (args[0] == "list" || args[0] == "l") {
        resource_manager->PrintResources();
    }
}

void PrintMenu(){
    static std::string menu[17] = {
        "\n---- Menu ----\n",
        "add <resource_name> <dependency_name>",
        "   adds a SOURCE resource and DEPENDENT resource.",
        "   if one of the SOURCE or DEPENDENT resources do not exist, it will create them.",
        "   does not complain or duplicate if one of the resources already exist.",
        "   detects cycles in dependencies before adding new links.\n",
        "del <resource_name>",
        "   deletes a resource.\n",
        "list",
        "   lists all nodes in the graph (including orphans) and their dependencies.\n",
        "show",
        "   traverses all nodes connected to the root node (excluding orphans) in the graph.\n",
        "q",
        "   quit gracefully.\n",
    };

    for(std::string item : menu){
        std::cout << item << std::endl;
    }
}

void RunMenu(ResourceManager* resource_manager) {
    std::string sentinel = "";
    while (sentinel != "q") {
        std::getline(std::cin, sentinel);
        std::cout << std::endl;
        ExecuteCommand(sentinel, resource_manager);
        std::cout << std::endl;
    }
}

void PrintError(std::string error) {
    std::cerr << "\033[1;31m" << error << "\033[0m" << std::endl;
}

int main() {
    ResourceManager* resource_manager = new ResourceManager("resources/no_cycle_2.txt");

    PrintMenu();
    RunMenu(resource_manager);

    return 0;
}

void test() {
    ResourceManager* has_cycle_1 = new ResourceManager("resources/has_cycle_1.txt");
    ResourceManager* has_cycle_2 = new ResourceManager("resources/has_cycle_2.txt");
    ResourceManager* has_cycle_3 = new ResourceManager("resources/has_cycle_3.txt");
}