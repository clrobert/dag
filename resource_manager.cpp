#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

static std::string INPUT_FILENAME = "resource.txt";
class Node;
Node* FindResource(std::string name, std::vector<Node *> resources);

class Node {
    public:
        std::string name;
        std::vector<Node *> dependencies;

        Node(std::string input_name){
            name = input_name;
        }

        void AddDependency(Node* dependency){
            Node* existingDependency = FindResource(dependency->name, dependencies);
            if(existingDependency == NULL){
                dependencies.push_back(dependency);
            }
        }

        bool Usable(){
            return true;
        }

        void Print(){
            std::cout << "resource: " << name << std::endl;
            std::cout << "dependencies: ";
            for(Node* node : dependencies){
                std::cout << node->name << " ";
            }
            std::cout << std::endl << "usable?: " << Usable();
            std::cout << '\n' << std::endl;
        }
};

Node* FindResource(std::string name, std::vector<Node *> resources){
    Node* existing_resource = NULL;

    for(Node* resource : resources){
        if(resource->name == name){
            existing_resource = resource;
        }
    }

    return existing_resource;
}

void DisplayGraph();

void HandleInput();

void DeleteNode();

void InsertNode();

void InsertDependency();

void CheckCycles();

int main(){
    std::ifstream input_data (INPUT_FILENAME);
    std::vector<Node *> resources;

    if(input_data.is_open()){
        for(std::string line; getline( input_data, line ); )
        {
            std::istringstream iss(line);
            std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

            std::string source_node_name = tokens[0];
            std::string destination_node_name = tokens[1];

            Node* source_resource = FindResource(source_node_name, resources);
            Node* destination_resource = FindResource(destination_node_name, resources);

            if (source_resource == NULL){
                source_resource = new Node(source_node_name);
                resources.push_back(source_resource);
            }

            if (destination_resource == NULL){
                destination_resource = new Node(destination_node_name);
                resources.push_back(destination_resource);
            }

            source_resource->AddDependency(destination_resource);
        }
        input_data.close();
    }

    for(Node* node : resources){
        node->Print();
    }

    return 0;
}
