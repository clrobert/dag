#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>

#include "resource_manager.h"

static std::string INPUT_FILENAME = "resource.txt";

std::string BoolToReadable(bool val) {
    if (val == 1) {
        return "yes";
    } else {
        return "no";
    }
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

        void AddDependency(std::string name) {
            dependencies.push_back(name);
        }

        void AddLink(Node* link) {
            Node* existingLink = FindResource(link->name, links);
            if (existingLink == NULL) {
                links.push_back(link);
            }
        }

        void Link(Node* link) {
            AddLink(link);
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
};

Node* FindResource(std::string name, std::vector<Node *> resources) {
    Node* existing_resource = NULL;

    for (Node* resource : resources) {
        if (resource->name == name) {
            existing_resource = resource;
        }
    }

    return existing_resource;
}

bool Deleted(Node* node) {
    return node->deleted;
}

std::vector<Node *> DeleteNode(std::string name, std::vector<Node *> resources) {
    for (Node* resource : resources) {
        Node* link = FindResource(name, resource->links);
        if (link != nullptr) {
            link->deleted = true;
            resource->links.erase(
                std::remove_if(resource->links.begin(),
                            resource->links.end(),
                            Deleted),
                resource->links.end());
        }
    }
    for (Node* resource : resources) {
        if (resource->name == name) {
            resource->deleted = true;
            resources.erase(
                std::remove_if(resources.begin(),
                               resources.end(),
                               Deleted),
                resources.end());
            delete resource;
        }
    }
    return resources;
}

void runMenu(std::string input, std::vector<Node *> resources){
    std::vector<std::string> args = SplitString(input);
    std::cout << args[2];
    if (args[0] == "del") {
        DeleteNode(args[1], resources);
    } else if (args[0] == "add") {

    } else if (args[0] == "link") {

    } else if (args[0] == "info") {
        Node* node = FindResource(args[1], resources);
        node->Print();
    } else if (args[0] == "show") {
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

int main() {
    std::ifstream input_data (INPUT_FILENAME);
    std::vector<Node *> resources;

    if (input_data.is_open()) {
        for (std::string line; getline(input_data, line); ) {
            std::vector<std::string> tokens = SplitString(line);
            std::string source_node_name = tokens[0];
            std::string destination_node_name = tokens[1];

            Node* source_resource = FindResource(source_node_name, resources);
            Node* destination_resource = FindResource(destination_node_name, resources);

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
        input_data.close();
    }

    std::string sentinel = "";

    // resources = DeleteNode("ore", resources);
    /*
    for (Node* node : resources) {
        node->Print();
    }
    */
    unsigned int microseconds = 10000;

    std::string menu[15] = {
        "\n----Options----\n",
        "del <resource_name>",
        "   deletes a resource.\n",
        "add <resource_name> <dependency_name>",
        "   add a resource and associated dependency.",
        "   complains if the resource already exists.",
        "   adds the dependency as a resource if it does not exist.\n",
        "link <resource_name> <dependency_name>",
        "   create a resource dependency.\n",
        "info <resource_name>",
        "   describes a resource and associated dependencies.\n",
        "show",
        "   displays the graph. lists all nodes and their dependencies.\n",
        "q",
        "   quit gracefully.\n",
    };

    for(std::string item : menu){
        std::cout << item << std::endl;
    }
    std::cin >> sentinel;
    runMenu(sentinel, resources);
    std::cout << std::endl;

    return 0;
}
