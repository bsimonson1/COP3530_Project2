#include <iostream>
#include <string>
#include<vector>
#include <map>
#include <iomanip>
#include <set>

class AdjacencyList {
private:
    // idea taken from my RTOS class, struct for nodes in the linked list implementation I have
    struct Node {
        // to value passed in by callee
        std::string to;
        Node* next;
        // constructor for the linked list
        Node(const std::string& passed_in_to) : to(passed_in_to), next(nullptr) {}
    };
    // Nodes I will use to set, or iterate, through my linked list
    Node* head;
    Node* current;

public:
    // insertion method to build the linked list
    void insertNode(const std::string& to);
    // function to return a vector of all the urls that are being pointed to by a particular key
    std::vector<std::string> getNodes() const;
};

class Graph {
private:
    // ordered container of all unique nodes and what url they point to
    std::map<std::string, AdjacencyList> adjListMap;
    std::set<std::string> allNodes;
public:
    // function to insert the to value into a singly linked list based on the from value
    void AddEdge(const std::string& from, const std::string& to);
    // public page rank function to perform p power iterations on the graph object
    void PageRank(int n);
};


void AdjacencyList::insertNode(const std::string& to)
{
    Node* newNode = new Node(to);
    // if the from url does not have a linked list then the head will not exist, thus set head to the node containing the passed in string
    if (!head)
    {
        head = newNode;
    }
        // if there is a linked list, iterate to the end of it, once at the end, the next node value will be the passed in to value
    else
    {
        // use current to iterate from the beginning node of the list
        current = head;
        // iterate until current does not have a next pointer
        while (current->next)
        {
            // update the current value
            current = current->next;
        }
        // here if the current value does not have a next pointer, thus, set it to the newNode containing the passed in to url
        current->next = newNode;
    }
}

std::vector<std::string> AdjacencyList::getNodes() const
{
    // initialize a vector to store the urls of the nodes
    std::vector<std::string> nodes_of_key;
    // start at the beginning of the from key's linked list
    Node* first = head;
    // iterate through all of the nodes of a linked list, also make sure that a node even exists
    while (first != nullptr)
    {
        // push the corresponding node url value into the vector
        nodes_of_key.push_back(first->to);
        // move to the next node (or try to)
        first = first->next;
    }
    // if here then we can return the vector
    return nodes_of_key;
}

void Graph::AddEdge(const std::string& from, const std::string& to)
{
    // build the map and the corresponding keys list
    adjListMap[from].insertNode(to);
    // this is used to keep track of all the nodes being used (to and from nodes)  for proper sizing (to get |n|)
    allNodes.insert(from);
    allNodes.insert(to);
}

/* prints the PageRank of all pages after p powerIterations in
 * ascending alphabetical order of webpages and rounding rank to two
 * decimal places
 */
void Graph::PageRank(int n)
{
    for (const auto& val : adjListMap)
    {
        for (const auto& toNode : val.second.getNodes())
        {
            // need to create entries in the adjacency list of the dangling nodes (to)
            if (adjListMap.find(toNode) == adjListMap.end())
            {
                // this is an empty key value of the dangling node (it will later be initialized to 0)
                adjListMap[toNode];
            }
        }
    }
    // number of key value pairs (basically number of keys which is the number of unique nodes)
    // this is from the set i populated in the addedge function
    size_t numberOfNodes = allNodes.size();
    // initial rank for all nodes is 1 / (total number of unique nodes)
    double initialRank = 1.0 / numberOfNodes;
    // map of the unique nodes in the adjacency list with ranks initialized to 1 / |n| (number of unique nodes)
    std::map<std::string, double> oldRank;
    for (const auto& val : adjListMap)
    {
        // unique string now has an initial rank value
        oldRank[val.first] = initialRank;
    }
    // initialize a map for the newRank values to 0
    std::map<std::string, double> newRank;
    for (const auto& pair : adjListMap)
    {
        // unique string now has an initial rank value
        newRank[pair.first] = 0.0;
    }
    // find the inDegree of each node
    std::map<std::string, int> outDegree;
    // Initialize outDegree for all nodes to 0
    for (const auto&val : adjListMap)
    {
        int outDegreeCount = val.second.getNodes().size();
        outDegree[val.first] = outDegreeCount;
    }
    // rank(i) = rank(j) / out_degree(j) + rank(k) / out_degree(k) where j and k point to i
    // rank(node N) = summation(rank(node i) / outdegree(node i)) where i is a node pointing to a given node n

    // now i need a for loop that will find all of the keys that point to node(i), then do the summations of all of those keys rank / keys outDegree
    // Construct the reverse adjacency list
    std::map<std::string, std::vector<std::string>> reverseAdjList;
    for (const auto& val : adjListMap)
    {
        for (const auto& toNode : val.second.getNodes())
        {
            // reverses the adjacency list by getting a key, and then a specific node that is being pointed to by that key, and add that node as a key to the
            // reversed list, then the node that was originally pointing to it as one of the values in the vector. This reverses the linked list so that
            // rather than having a map of keys that point to values, it has a map of keys that are being pointed at by values (essentially) this makes it easier
            // to do the page rank calc (outDegree)
            reverseAdjList[toNode].push_back(val.first);
        }
    }

    // made a mistake here, need to keep track of the to urls as well and treat them as dangling nodes.
    for (const auto& val : adjListMap)
    {
        if (oldRank.find(val.first) == oldRank.end())
        {
            // if here then it needs to be initialized to 0
            oldRank[val.first] = 0;
        }
    }

    // power iterations calculator
    for (int p_iteration_count = 0; p_iteration_count < n-1; p_iteration_count++)
    {
        // once the calc is done and oldRank has the updated values, reset newRank to 0
        for (const auto& val : adjListMap)
        {
            newRank[val.first] = 0.0;
        }

        // calculate the rank for each node
        for (const auto& val : adjListMap)
        {
            std::string fromVal = val.first;
            double sum = 0.0;

            auto it = reverseAdjList.find(fromVal);
            // the node needs to exist in the adjlist and have corresponding vals
            if (it != reverseAdjList.end())
            {
                for (const std::string& pointingNode : it->second)
                {
                    sum += oldRank[pointingNode] / outDegree[pointingNode];
                }
            }
            // update the keys corresponding pagerank value
            newRank[fromVal] = sum;
        }
        // update the oldRank values (which will be used at the end) and prepare to reset newRank back to 0.0
        oldRank = newRank;
    }

    // set output to round to two decimal places
    std::cout << std::fixed << std::setprecision(2);
    // iterate through the oldRank map and print the from url and the corresponding to double value
    for (const auto& val : oldRank)
    {
        std::cout << val.first << " " << val.second << std::endl;
    }
}

int main()
{
    Graph webGraph;
    int no_of_lines, power_iterations;
    std::string from, to;
    std::cin >> no_of_lines;
    std::cin >> power_iterations;
    for(int i = 0; i < no_of_lines; i++)
    {
        // take input from user
        std::cin >> from;
        std::cin >> to;
        // create the directed graph by passing in the initial url and where it points to
        webGraph.AddEdge(from, to);
    }

    // if here then the adjacency list has been built
    webGraph.PageRank(power_iterations);

    return 0;
}