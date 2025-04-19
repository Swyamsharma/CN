#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>
#include <string>

using namespace std;

using DistanceVector = unordered_map<string, int>;
using Links = unordered_map<string, unordered_map<string, int>>;

DistanceVector initialize_distance_vector(const vector<string>& nodes, const Links& links, const string& source) {
    const int inf = numeric_limits<int>::max();
    DistanceVector distance_vector;

    for (const auto& node : nodes) {
        distance_vector[node] = inf;
    }
    distance_vector[source] = 0;

    for (const auto& neighbor : links.at(source)) {
        distance_vector[neighbor.first] = neighbor.second;
    }

    return distance_vector;
}

unordered_map<string, DistanceVector> exchange_and_update(const vector<string>& nodes, const Links& links, const unordered_map<string, DistanceVector>& distance_vectors) {
    unordered_map<string, DistanceVector> new_vectors = distance_vectors;

    for (const auto& node : nodes) {
        for (const auto& neighbor : links.at(node)) {
            for (const auto& target : nodes) {
                // Check if the distance to the target node is not infinity
                if (distance_vectors.at(node).at(target) != numeric_limits<int>::max()) {
                    int new_cost = distance_vectors.at(node).at(target) + neighbor.second;
                    if (new_cost < new_vectors[neighbor.first][target]) {
                        new_vectors[neighbor.first][target] = new_cost;
                    }
                }
            }
        }
    }

    return new_vectors;
}

void print_distance_vector(const DistanceVector& dv, const vector<string>& nodes) {
    cout << "{";
    for (size_t i = 0; i < nodes.size(); ++i) {
        const string& node = nodes[i];
        cout << "'" << node << "': " << (dv.at(node) == numeric_limits<int>::max() ? "inf" : to_string(dv.at(node)));
        if (i < nodes.size() - 1) {
            cout << ", ";
        }
    }
    cout << "}";
}

void simulate_distance_vector_protocol(const vector<string>& nodes, const Links& links, const string& source) {
    unordered_map<string, DistanceVector> distance_vectors;

    for (const auto& node : nodes) {
        distance_vectors[node] = initialize_distance_vector(nodes, links, node);
    }

    cout << "Initial distance vector of " << source << ": ";
    print_distance_vector(distance_vectors[source], nodes);
    cout << endl;

    int iterations = 0;
    while (true) {
        auto new_vectors = exchange_and_update(nodes, links, distance_vectors);
        bool converged = true;

        for (const auto& node : nodes) {
            if (new_vectors[node] != distance_vectors[node]) {
                converged = false;
                break;
            }
        }

        if (converged) {
            break;
        }

        distance_vectors = new_vectors;
        iterations++;

        cout << "After " << iterations << " iteration(s), Distance vector of " << source << ": ";
        print_distance_vector(distance_vectors[source], nodes);
        cout << endl;
    }

    cout << "Final distance vector at " << source << ": ";
    print_distance_vector(distance_vectors[source], nodes);
    cout << endl;
}

int main() {
    vector<string> nodes = {"A", "B", "C", "D", "E", "F", "G"};

    Links links = {
        {"A", {{"B", 1}, {"C", 1}, {"E", 1}}},
        {"B", {{"A", 1}, {"C", 1}}},
        {"C", {{"A", 1}, {"B", 1}, {"D", 1}}},
        {"D", {{"C", 1}, {"G", 1}}},
        {"E", {{"A", 1}}},
        {"F", {{"G", 1}}},
        {"G", {{"D", 1}, {"F", 1}}}
    };

    simulate_distance_vector_protocol(nodes, links, "A");

    return 0;
}
