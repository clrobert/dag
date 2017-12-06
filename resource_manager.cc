#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>

#include "resource_manager.h"

std::string BoolToReadable(bool val) {
    if (val == 1) {
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

    Node(std::string input_name) {
        name = input_name;
    }

    void Link(Node* link) {
        Node* existing_link = Find(link->name);
        if (existing_link == NULL) {
            AddLink(link);
        }
        for (std::string dependency : dependencies) {
            if (link->name == dependency) {
                return;
            }
        }
        AddDependency(link->name);
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

class ResourceManager {
public:
    std::vector<Node *> resources;

    ResourceManager(std::string input_filename){
        std::ifstream input_data (input_filename);

        if (input_data.is_open()) {
            for (std::string line; getline(input_data, line); ) {
                std::vector<std::string> tokens = SplitString(line);
                AddResource(tokens[0], tokens[1]);
            }
            input_data.close();
        }
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

    void AddResource(std::string source_node_name, std::string destination_node_name) {
        if (Cycles()) {
            return;
        }
        Node* source_resource = Find(source_node_name);
        Node* destination_resource = Find(destination_node_name);

        if (source_resource == NULL) {
            source_resource = new Node(source_node_name);
            resources.push_back(source_resource);
        }
        if (destination_resource == NULL) {
            destination_resource = new Node(destination_node_name);
            resources.push_back(destination_resource);
        }
        source_resource->Link(destination_resource);
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

    bool Cycles(){
        return false;
    }

    void Show(){

    }
};

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
        "   displays the graph.\n",
        "help",
        "   displays this menu.\n",
        "q",
        "   quit gracefully.\n",
    };

    for(std::string item : menu){
        std::cout << item << std::endl;
    }
}

int main() {
    ResourceManager resource_manager = ResourceManager("resource.txt");
    std::string sentinel = "";

    PrintMenu();

    while (sentinel != "q") {
        std::cin >> sentinel;
        ExecuteCommand(sentinel, resource_manager);
        std::cout << std::endl;
    }

    return 0;
}
