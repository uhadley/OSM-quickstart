#include <iostream>
#include <map> // BST 
#include <unordered_map> // hash table
#include <vector> // dynamic array
#include <string> // 
#include <cmath>

const double EARTH_RADIUS_KM = 6371.0;

double toRadians(double degree) {
    return degree * M_PI / 180.0;
}

// Haversine formula to calculate distance between two coordinates
double haversine(double lat1, double lon1, double lat2, double lon2) {
    lat1 = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2 = toRadians(lat2);
    lon2 = toRadians(lon2);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_KM * c;
}


#include "tinyxml2.h"

using namespace tinyxml2;

struct Node {
    //long long id;
    double lat, lon;
};

struct Way {
    std::vector<long long> node_refs;
    bool is_highway = false;
    bool is_one_way = false;
    std::string name; // To store the road name if available
};

struct Edge {
    long long node1, node2;
    std::string label;
};

std::map<long long, Node> nodes;
std::vector<Way> ways;
struct EdgeInfo {
    std::string road_name;
    double distance_km;
};

std::unordered_map<long long, std::unordered_map<long long, EdgeInfo>> graph;

double min_lat = 1e9, max_lat = -1e9;
double min_lon = 1e9, max_lon = -1e9;

void parse_osm(const char* filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS)
        throw std::runtime_error("Failed to read OSM file");

    XMLElement* root = doc.RootElement();
    for (XMLElement* elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        std::string tagName = elem->Name();
        if (tagName == "node") {
            long long id = std::stoll(elem->Attribute("id"));
            double lat = std::stod(elem->Attribute("lat"));
            double lon = std::stod(elem->Attribute("lon"));
            nodes[id] = {lat, lon};
            min_lat = std::min(min_lat, lat);
            max_lat = std::max(max_lat, lat);
            min_lon = std::min(min_lon, lon);
            max_lon = std::max(max_lon, lon);
        } else if (tagName == "way") {
            Way way;
            bool is_hw = false;
            for (XMLElement* child = elem->FirstChildElement(); child; child = child->NextSiblingElement()) {
                std::string childName = child->Name();
                if (childName == "nd") {
                    long long ref = std::stoll(child->Attribute("ref"));
                    way.node_refs.push_back(ref);
                } else if (childName == "tag") {
                    const char* k = child->Attribute("k");
                    const char* v = child->Attribute("v");
                    if (k && std::string(k) == "highway") {
                        is_hw = true;
                    }
                    if (k && std::string(k) == "name") {
                        way.name = v ? v : "";
                    }
                    if (k && std::string(k) == "oneway" && std::string(v)=="yes") {
                        way.is_one_way=true;
                    }
                    
                }
            }
            way.is_highway = is_hw;
            if(way.is_highway){
                ways.push_back(way);

            // Add edges to the graph (undirected)
            for (size_t i = 1; i < way.node_refs.size(); ++i) {
                long long node1 = way.node_refs[i - 1];
                long long node2 = way.node_refs[i];
                if (nodes.count(node1) && nodes.count(node2)) {
                    double lat1 = nodes[node1].lat;
                    double lon1 = nodes[node1].lon;
                    double lat2 = nodes[node2].lat;
                    double lon2 = nodes[node2].lon;
                
                    double dist = haversine(lat1, lon1, lat2, lon2);
                
                    graph[node1][node2] = {way.name, dist};
                    if (!way.is_one_way)
                        graph[node2][node1] = {way.name, dist};
                }
                
            }//for
           }// is_highway
        }//way
    } //main loop
}


void print_graph() {
    for (const auto& node : graph) {
        std::cout << "Node " << nodes[node.first].lat << " " << nodes[node.first].lon << "\n";
        for (const auto& edge : node.second) {
            std::cout << "Edge: " << node.first << " <-> " << edge.first 
                      << " (Road: " << edge.second.road_name 
                      << ", Distance: " << edge.second.distance_km << " km)\n";
        }
    }
}

void print_distance_between_nodes(long long node1, long long node2) {
    if (nodes.count(node1) && nodes.count(node2)) {
        double lat1 = nodes[node1].lat;
        double lon1 = nodes[node1].lon;
        double lat2 = nodes[node2].lat;
        double lon2 = nodes[node2].lon;
        double dist = haversine(lat1, lon1, lat2, lon2);
        std::cout << "Distance between node " << node1 << " and " << node2 << ": " << dist << " km\n";
    } else {
        std::cout << "One or both nodes not found.\n";
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.osm> \n";
        return 1;
    }

    const char* input_file = argv[1];

    parse_osm(input_file);

    // Print the graph with edge labels
    print_graph();

// Call this instead in main() for quick demo:
if (!graph.empty()) {
    auto first = *graph.begin();
    if (!first.second.empty()) {
        auto second = *first.second.begin();
        print_distance_between_nodes(first.first, second.first);
    }
}


    return 0;
}
