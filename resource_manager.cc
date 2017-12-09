#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

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
    std::vector<Node *> links;
    std::vector<std::string> dependencies;
    bool deleted = false;
    bool visited = false;

    Node(std::string input_name) {
        name = input_name;
    }

    void Link(Node* link) {
        Node* existing_link = Find(link->name);
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
        return links.size() == dependencies.size();
    }

    void Print() {
        std::cout << "resource: " << name << std::endl;
        std::cout << "dependencies: " << dependencies.size() << "; ";
        for (std::string dependency : dependencies) {
            std::cout << dependency << " ";
        }

        std::cout << std::endl << "dependencies linked: " << links.size() << "; ";
        for (Node* node : links) {
            std::cout << node->name << " ";
        }

        std::cout << std::endl << "usable? " << BoolToReadable(Usable());
        std::cout << '\n' << std::endl;
    }

    Node* Find(std::string name) {
        Node* existing_resource = NULL;

        for (Node* resource : links) {
            if (resource->name == name) {
                existing_resource = resource;
            }
        }

        return existing_resource;
    }
private:
    void AddDependency(std::string name) {
        dependencies.push_back(name);
    }

    void AddLink(Node* link) {
        links.push_back(link);
    }
};

Node* Find(std::string name, std::vector<Node*> nodes) {
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
    std::vector<Node *> resources;

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

    Node* Find(std::string name) {
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
        Node* source_resource = Find(source_node_name);
        Node* destination_resource = Find(destination_node_name);

        std::cout << "--pre-size: " << resources.size() << std::endl;

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

        std::cout << source_node_name << "//" << destination_node_name << std::endl;
        std::cout << source_resource << "//" << destination_resource << std::endl;
        std::cout << "size: " << resources.size() << std::endl;

        return true;
    }

    std::vector<Node *> DeleteResource(std::string name) {
        for (Node* resource : resources) {
            Node* link = resource->Find(name);
            if (link != nullptr) {
                link->deleted = true;
                resource->links.erase(
                    std::remove_if(resource->links.begin(),
                                resource->links.end(),
                                [](Node* node) { return node->deleted; }),
                    resource->links.end());
            }
        }
        for (Node* resource : resources) {
            if (resource->name == name) {
                resource->deleted = true;
                resources.erase(
                    std::remove_if(resources.begin(),
                                resources.end(),
                                [](Node* node) { return node->deleted; }),
                    resources.end());
                delete resource;
            }
        }
        return resources;
    }

    void PrintResources() {
        for (Node* resource : resources) {
            resource->Print();
        }
    }

    bool CycleDetected(){
        for (Node* node : resources) {
            std::vector<Node*> recursion_stack;
            if (CheckCycles(node, recursion_stack, 0) > 0) {
                return true;
            }
            recursion_stack.clear();
        }
        return false;
    }

    int CheckCycles(Node* node, std::vector<Node*> recursion_stack, int cycles){
        if (::Find(node->name, recursion_stack)) {
            recursion_stack.push_back(node);
            PrintError("resource creation error: cycle detected.");
            PrintError(node->name + " already exists.");
            PrintNodeList(recursion_stack);
            std::cerr << std::endl;
            cycles++;
        } else {
            recursion_stack.push_back(node);
            if (!node->links.empty()) {
                std::vector<Node*>::iterator iter;
                for (iter = node->links.begin(); iter < node->links.end(); iter++) {
                    cycles += CheckCycles(*iter, recursion_stack, cycles);
                }
            } else {
                recursion_stack.pop_back();
            }
        }
        return cycles;
    }

    void Show() {
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
            std::vector<Node*>::iterator iter;
            for (iter = node->links.begin(); iter < node->links.end(); iter++) {
                PostOrderTraversal(*iter);
            }
            std::cout << node->name << std::endl;
        } else {
            std::cout << node->name << std::endl;
        }
    }

    void PreOrderTraversal(Node* node){
        if (!node->links.empty()) {
            std::vector<Node*>::iterator iter;
            std::cout << node->name << std::endl;
            for (iter = node->links.begin(); iter < node->links.end(); iter++) {
                PreOrderTraversal(*iter);
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

void PrintNodeList(std::vector<Node*> nodes) {
    if (nodes.empty()) {
        return;
    }
    std::string node_list = "";
    Node* last = nodes.back();
    nodes.pop_back();
    for (Node* node : nodes) {
        node_list += node->name + " -> ";
    }
    std::cout << node_list + last->name << std::endl;
}

void ExecuteCommand(std::string input, ResourceManager resource_manager){
    std::vector<std::string> args = SplitString(input);
    if (args[0] == "del") {
        resource_manager.DeleteResource(args[1]);
    } else if (args[0] == "add") {
        resource_manager.AddResource(args[1], args[2]);
    } else if (args[0] == "show") {
        resource_manager.Show();
    } else if (args[0] == "list") {
        resource_manager.PrintResources();
    } else if (args[0] == "help") {
        PrintMenu();
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
        "   lists all nodes and their dependencies.\n",
        "show",
        "   traverses the graph.\n",
        "help",
        "   displays this menu.\n",
        "q",
        "   quit gracefully.\n",
    };

    for(std::string item : menu){
        std::cout << item << std::endl;
    }
}

void RunMenu(ResourceManager resource_manager) {
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
    ResourceManager resource_manager = ResourceManager("resources/no_cycle_2.txt");

    PrintMenu();
    RunMenu(resource_manager);

    return 0;
}

void test() {
    ResourceManager has_cycle_1 = ResourceManager("resources/has_cycle_1.txt");
    ResourceManager has_cycle_2 = ResourceManager("resources/has_cycle_2.txt");
    ResourceManager has_cycle_3 = ResourceManager("resources/has_cycle_3.txt");
}