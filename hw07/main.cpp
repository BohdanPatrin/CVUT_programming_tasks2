#ifndef __PROGTEST__
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <array>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <compare>
#include <algorithm>
#include <memory>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <optional>
using namespace std::literals;

class CDumbString
{
  public:
    CDumbString             ( std::string v )
      : m_Data ( std::move ( v ) )
    {
    }
    bool        operator == ( const CDumbString & rhs ) const = default;
  private:
    std::string m_Data;
};
#endif /* __PROGTEST__ */


//checking of type for bonus test 1
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

//type alias for index mapping and graph, selected based on type capabilities
template <typename T_>
using IndexMap = std::conditional_t<Hashable<T_>, std::unordered_map<T_, int>,
        std::conditional_t<Comparable<T_>, std::map<T_, int>,
                std::vector<T_>>>;

template <typename T_>
using GraphMap = std::conditional_t<Hashable<T_>, std::unordered_map<T_, std::vector<std::pair<T_, int>>>,
        std::conditional_t<Comparable<T_>, std::map<T_, std::vector<std::pair<T_, int>>>,
                std::vector<std::pair<T_, std::vector<std::pair<T_, int>>>>>>;

template <typename T_>
class CNet {
public:
    CNet() = default;



    void addEdge(const T_& a, const T_& b, int cost) {
        if constexpr (Hashable<T_> || Comparable<T_>) {
            m_graph[a].emplace_back(b, cost);
        } else {
            //fallback for vector of pairs
            auto it = std::find_if(m_graph.begin(), m_graph.end(), [&](const auto& pair) { return pair.first == a; });
            if (it != m_graph.end())
                it->second.emplace_back(b, cost);
            else
                m_graph.emplace_back(a, std::vector<std::pair<T_, int>>{{b, cost}});
        }
    }

    CNet& add(const T_& a, const T_& b, int cost) {
        //add two connections because cooperation is always bi-directional
        addEdge(a, b, cost);
        addEdge(b, a, cost);
        return *this;
    }


    void buildIndexing() { //adding indexes for clerks
        if constexpr (Hashable<T_> || Comparable<T_>) {
            int idx = 0;
            for (const auto& [key, _] : m_graph) {
                m_index[key] = idx++;
                m_revIndex.push_back(key);
            }
        }
        else {
            int idx = 0;
            for (const auto& [key, _] : m_graph) {
                m_revIndex.push_back(key);
                ++idx;
            }
        }
    }

    int getIndex(const T_& x) const {
        if constexpr (Hashable<T_> || Comparable<T_>) {
            auto it = m_index.find(x);
            return (it != m_index.end()) ? it->second : -1;
        }
        else {
            auto it = std::find(m_revIndex.begin(), m_revIndex.end(), x);
            return (it != m_revIndex.end()) ? static_cast<int>(it - m_revIndex.begin()) : -1;
        }
    }

    void rec_dfs(int current) { //recursive fill the m_depth, m_parent[0] and costToParet[0]
        for (auto [neighbor, cost] : m_adj[current])
            if (m_depth[neighbor] == -1) {
                m_depth[neighbor] = m_depth[current] + 1;
                m_parent[0][neighbor] = current;
                m_costToParent[0][neighbor] = cost;
                rec_dfs(neighbor);
            }
    }

    void dfsBuildTree() {
        //filling the adjacency table
        if constexpr (Hashable<T_> || Comparable<T_>)
            for (const auto& [node, neighbors] : m_graph) {
                int a = m_index[node];
                for (const auto& [neighbor, cost] : neighbors) {
                    int b = m_index[neighbor];
                    m_adj[a].emplace_back(b, cost);
                }
            }
        else
            for (size_t i = 0; i < m_graph.size(); ++i) {
                const T_& node = m_graph[i].first;
                int a = getIndex(node);
                for (const auto& [neighbor, cost] : m_graph[i].second) {
                    int b = getIndex(neighbor);
                    m_adj[a].emplace_back(b, cost);
                }
            }

        //start dfs from root
        m_depth[0] = 0;
        rec_dfs(0);
    }

    void buildBinaryLifting() {
        size_t n = m_revIndex.size();

        //filling in all the 2^k-th ancestors
        for (size_t k = 1; k < (size_t)std::ceil(std::log2(n)); ++k)
            for (size_t p = 0; p < n; ++p) {
                int mid = m_parent[k - 1][p]; //go up by 2^(k-1) steps
                if (mid != -1)
                    if(m_parent[k - 1][mid] !=-1) {
                        m_parent[k][p] = m_parent[k - 1][mid]; //2^k-th ancestor = 2^(k-1)-th ancestor of mid
                        m_costToParent[k][p] = m_costToParent[k - 1][p] + m_costToParent[k - 1][mid]; //same with the cost
                    }
            }
    }

    CNet& optimize() {
        buildIndexing();

        //resizing and initializing all needed containers
        int n = m_revIndex.size();
        m_adj.resize(n);
        m_depth.resize(n, -1);
        int L = std::ceil(std::log2(n));
        m_parent.assign(L, std::vector<int>(n, -1));
        m_costToParent.assign(L, std::vector<int>(n, -1));

        dfsBuildTree();
        buildBinaryLifting();//i use binary lifting to compute cost faster in future
        return *this;
    }


    int computeLCA(int a, int b) const {
        int cost = 0;

        //make a is deeper
        if (m_depth[a] < m_depth[b])
            std::swap(a, b);

        //making a the same depth as b
        int diff = m_depth[a] - m_depth[b];
        for (int k = 0; (1 << k) <= diff; ++k)
            if (diff & (1 << k)) {
                cost += m_costToParent[k][a];
                a = m_parent[k][a];
            }

        if (a == b) return cost; //if b is a parent of a

        //jumping by binary lifting
        for (int k = m_parent.size() - 1; k >= 0; --k)
            if (m_parent[k][a] != -1 && m_parent[k][b] != -1 && m_parent[k][a] != m_parent[k][b]) {
                cost += m_costToParent[k][a];
                cost += m_costToParent[k][b];
                a = m_parent[k][a];
                b = m_parent[k][b];
            }


        //add the last step to common parent
        cost += m_costToParent[0][a];
        cost += m_costToParent[0][b];

        return cost;
    }

    int totalCost(const T_& a, const T_& b) const {
        int idxA = getIndex(a);
        int idxB = getIndex(b);
        if (idxA == -1 || idxB == -1) return -1;
        return computeLCA(idxA, idxB); //i use LCA to compute the cost
    }

private:
    //graph storage
    GraphMap<T_> m_graph;

    //indexing for clerks -> int
    IndexMap<T_> m_index; //is used only for hashable/comparable types
    std::vector<T_> m_revIndex;

    //tree + LCA structures
    std::vector<std::vector<std::pair<int, int>>> m_adj; // int-indexed adjacency list
    std::vector<int> m_depth;
    std::vector<std::vector<int>> m_parent;         // binary lifting table
    std::vector<std::vector<int>> m_costToParent;   // cost to go to parent at every level 2^k
};
#ifndef __PROGTEST__
int main ()
{
  CNet<std::string> a;
  a . add ( "Adam", "Bob", 100 )
    . add ( "Bob", "Carol", 200 )
    . add ( "Dave", "Adam", 300 )
    . add ( "Eve", "Fiona", 120 )
    . add ( "Kate", "Larry", 270 )
    . add ( "Ivan", "John", 70 )
    . add ( "Kate", "Ivan", 300 )
    . add ( "George", "Henry", 10 )
    . add ( "Eve", "George", 42 )
    . add ( "Adam", "Eve", 75 )
    . add ( "Ivan", "George", 38 )
    . optimize ();
  assert ( a . totalCost ( "Adam", "Bob" ) == 100 );
  assert ( a . totalCost ( "John", "Eve" ) == 150 );
  assert ( a . totalCost ( "Dave", "Henry" ) == 427 );
  assert ( a . totalCost ( "Carol", "Larry" ) == 1025 );
  assert ( a . totalCost ( "George", "George" ) == 0 );
  assert ( a . totalCost ( "Alice", "Bob" ) == -1 );
  assert ( a . totalCost ( "Thomas", "Thomas" ) == -1 );

  CNet<int> b;
  b . add ( 0, 1, 100 )
    . add ( 1, 2, 200 )
    . add ( 3, 0, 300 )
    . add ( 4, 5, 120 )
    . add ( 10, 11, 270 )
    . add ( 8, 9, 70 )
    . add ( 10, 8, 300 )
    . add ( 6, 7, 10 )
    . add ( 4, 6, 42 )
    . add ( 0, 4, 75 )
    . add ( 8, 6, 38 )
    . optimize ();
  assert ( b . totalCost ( 0, 1 ) == 100 );
  assert ( b . totalCost ( 9, 4 ) == 150 );
  assert ( b . totalCost ( 3, 7 ) == 427 );
  assert ( b . totalCost ( 2, 11 ) == 1025 );
  assert ( b . totalCost ( 6, 6 ) == 0 );
  assert ( b . totalCost ( 0, 1 ) == 100 );
  assert ( b . totalCost ( 19, 19 ) == -1 );

  CNet<CDumbString> c;
  c . add ( "Adam"s, "Bob"s, 100 )
    . add ( "Bob"s, "Carol"s, 200 )
    . add ( "Dave"s, "Adam"s, 300 )
    . add ( "Eve"s, "Fiona"s, 120 )
    . add ( "Kate"s, "Larry"s, 270 )
    . add ( "Ivan"s, "John"s, 70 )
    . add ( "Kate"s, "Ivan"s, 300 )
    . add ( "George"s, "Henry"s, 10 )
    . add ( "Eve"s, "George"s, 42 )
    . add ( "Adam"s, "Eve"s, 75 )
    . add ( "Ivan"s, "George"s, 38 )
    . optimize ();
  assert ( c . totalCost ( "Adam"s, "Bob"s ) == 100 );
  assert ( c . totalCost ( "John"s, "Eve"s ) == 150 );
  assert ( c . totalCost ( "Dave"s, "Henry"s ) == 427 );
  assert ( c . totalCost ( "Carol"s, "Larry"s ) == 1025 );
  assert ( c . totalCost ( "George"s, "George"s ) == 0 );
  assert ( c . totalCost ( "Alice"s, "Bob"s ) == -1 );
  assert ( c . totalCost ( "Thomas"s, "Thomas"s ) == -1 );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

