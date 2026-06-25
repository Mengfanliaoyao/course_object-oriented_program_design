#include <iostream>

#include <vector>
#include <map>
#include <queue>

#include <algorithm>
#include <limits>


#define MAX_CAPACITY 0xffffffffU

#define DEBUG 0

namespace {
#if DEBUG
  int nothing;
#endif
  typedef unsigned int uint;
}


class SwitchNode
{
public:
  
  inline std::vector<uint>& getNeighbors() { return neighbors; }
  inline std::vector<uint>& getAdjLinks() { return adjLinks; }
  inline std::map<uint, uint>& getNextHopMap() { return nextHopMap; }
  inline std::map<uint, uint>& getNextLinkMap() { return nextLinkMap; }
  
  inline int getNextHop(uint label) { return nextHopMap[label]; }
  
  inline void setNeighbor(uint id) { neighbors.push_back(id); }
  inline void setAdjLink(uint id) { adjLinks.push_back(id); }
  
  SwitchNode(uint id)
    :id(id){}

#if DEBUG
  void showNeighbors();
  void ShowAdjLinks();
#endif
private:
  uint id;
  std::vector<uint> neighbors;
  std::vector<uint> adjLinks;
  std::map<uint, uint> nextHopMap;
  std::map<uint, uint> nextLinkMap;
};

struct Link
{
  uint id, v1, v2, capacity;
  Link(uint id, uint v1, uint v2, uint capacity)
    :id(id), v1(v1), v2(v2), capacity(capacity){}
};

class Graph
{
public:
  
  inline std::vector<SwitchNode>& getNodes() { return nodes; }
  inline std::vector<Link>& getLinks() { return links; }
  
  void findAllPairShortestPath();
  void showNavigationMap();
  
private:
  
  void dijkstra(uint src);
  void bfs(uint src);
private:
  std::vector<SwitchNode> nodes;
  std::vector<Link> links;
  
  std::vector<std::vector<uint>> hops;
  std::vector<std::vector<uint>> capacities;
};


struct TaskFlow
{
  uint id, src, dst, flowSize;
  double weight;
  TaskFlow(uint id, uint src, uint dst, uint flow)
  :id(id), src(src), dst(dst), flowSize(flow), weight(0){}
};


int main(void)
{
  uint n_node, n_link, n_task, label_limit;
  std::vector<TaskFlow> tasks;
  
  // input
  std::cin >> n_node >> n_link >> n_task >> label_limit;
  
  Graph g;
  
  for(uint i = 0; i < n_node; i++)
    g.getNodes().emplace_back(i);
  
  for(uint i = 0; i < n_link; i++)
  {
    uint id, v1, v2, c;
    std::cin >> id >> v1 >> v2 >> c;
    g.getLinks().emplace_back(id, v1, v2, c);
    g.getNodes()[v1].setNeighbor(v2);
    g.getNodes()[v1].setAdjLink(id);
    g.getNodes()[v2].setNeighbor(v1);
    g.getNodes()[v2].setAdjLink(id);
  }
  
  for(uint i = 0; i < n_task; i++)
  {
    uint id, src, dst, flow;
    std::cin >> id >> src >> dst >> flow;
    tasks.emplace_back(id, src, dst, flow);
  }
  
#if DEBUG
  for (auto node : g.getNodes())
  {
      node.showNeighbors();
      node.ShowAdjLinks();
  }
#endif

  // find APSP
  g.findAllPairShortestPath();
  
  // select
//  std::sort(tasks.begin(), tasks.end(), [&](){});
  
  std::vector<uint> selected_tasks;
  uint final_flow = 0;
  
  std::vector<uint> current_flow;
  current_flow.assign(n_link, 0); // set initial all zero
  
  for(uint i = 0; i < n_task; i++) {
    uint src = tasks[i].src, dst = tasks[i].dst;
    uint task_flow = tasks[i].flowSize;
    
    std::vector<uint> flow_cost;  // link_id
    
    bool reachable = true;
    uint curr = src;
#if DEBUG
    std::cout << "src: " << src << ", dst: " << dst << '\n';
#endif
    while(reachable && curr != dst)
    {
#if DEBUG
      std::cout << curr << ' ';
#endif
      auto& node_src = g.getNodes()[curr];
      uint next_hop = node_src.getNextHop(dst);
      uint lid = node_src.getNextLinkMap()[dst];
      
      Link& link = g.getLinks()[lid];
      
      uint link_cap = link.capacity;
      
      if(current_flow[lid] + task_flow > link_cap) // short of capacity
      {
        reachable = false;
        break;
      }
      
      flow_cost.emplace_back(lid); // record template cost
      curr = next_hop;
    }
#if DEBUG
    std::cout << '\n';
#endif
    // commit actual cost to current_flow
    if(reachable) 
    {
      for(uint l : flow_cost)
      {
        current_flow[l] += task_flow;
      }
      selected_tasks.push_back(i);
      final_flow += task_flow;
#if DEBUG
      std::cout << "Task " << i << " completed: " << src << " -> " << dst << '\n';
#endif
    }
    else
    {
#if DEBUG
      std::cout << "Task " << i << " rejected: insufficient capacity\n";
#endif  
    }
  }
  
  // output
  g.showNavigationMap();
  
  uint n_selected = selected_tasks.size();
  std::cout << n_selected << '\n';
  for(uint i = 0; i < n_selected; i++)
  {
    uint task_id = selected_tasks[i];
    std::cout << task_id << '\n';
    std::cout << tasks[task_id].src << ' ' << tasks[task_id].dst << '\n';
  }
  std::cout << final_flow;
  
  return 0;
}


void Graph::findAllPairShortestPath()
{
  uint v = nodes.size();
//  capacities.resize(v);
//  for (auto& vec : capacities) vec.resize(v, 0);

  hops.resize(v);
  for (auto& vec : hops) vec.resize(v, MAX_CAPACITY);
  
  for(uint src = 0; src < v; src++) // iterate for all node id as src
  {
//    dijkstra(src);
      bfs(src);
  }
#if 0
  //      if(src==2)
  showNavigationMap();
#endif
}

void Graph::bfs(uint src)
{
    std::vector<uint>& hop = hops[src];
    hop[src] = 0;

    std::queue<uint> q;
    q.push(src);

    auto& m = nodes[src].getNextHopMap();
    auto& lm = nodes[src].getNextLinkMap();
    m.clear();
    lm.clear();

    // level order bfs
    while (!q.empty())
    {
        uint u = q.front();
        q.pop();

        const auto& neighbors = nodes[u].getNeighbors();
        const auto& adjlinks = nodes[u].getAdjLinks();


        for (uint i = 0; i < neighbors.size(); i++)
        {
            uint v = neighbors[i];
            uint l = adjlinks[i];

            if (u == src)
            {
                m[v] = v;
                lm[v] = l;
            }

            if (hop[v] == MAX_CAPACITY)
            {
                hop[v] = hop[u] + 1;
                if (u != src)
                {
                    m[v] = m[u];
                    lm[v] = lm[u];
                }
                
                q.push(v);
            }
        }
    }
}

void Graph::dijkstra(uint src)
{
  std::vector<uint>& capacity = capacities[src];
//  std::fill(capacity.begin(), capacity.end(), 0); // initialize with 0 capacity
  capacity[src] = MAX_CAPACITY;
  
  // max heap
  std::priority_queue<std::pair<uint, uint>> pq; // max_next_capacity, node_id
  pq.push({MAX_CAPACITY, src});
  
  auto& m = nodes[src].getNextHopMap();
  m.clear(); // avoid recall this function
  auto& lm = nodes[src].getNextLinkMap();
  lm.clear();
  
  while (!pq.empty()) {
    uint c = pq.top().first; // current max capacity
    uint u = pq.top().second; // current node_id
    pq.pop();
    
    if (c < capacity[u]) continue; // visted vertex
    
    const auto& neighbors = nodes[u].getNeighbors();
    const auto& adjlinks = nodes[u].getAdjLinks();
    
    for (uint i = 0; i < neighbors.size(); i++)
    {
      uint v = neighbors[i];
      uint l = adjlinks[i];
      uint next_c = links[l].capacity;
      
      if(u == src)
      {
        m[v] = v;
        lm[v] = l;
      }
        
      
      uint new_c = std::min(capacity[u], next_c);
      if (new_c > capacity[v]) // maximize capacity of all passed link
      {
        capacity[v] = new_c;
        if(u != src)
        {
          m[v] = m[u];
          lm[v] = lm[u];
        }
          
        pq.push({new_c, v});
      }
    }
  }
#if 0
//      if(src==2)
      showNavigationMap();
#endif
}


void Graph::showNavigationMap() 
{
  uint v = nodes.size();
  for (uint src = 0; src < v; src++)
  {
#if DEBUG
    std::cout << "Source: ";
#endif
    std::cout << src << '\n';

#if DEBUG
    const auto& m = nodes[src].getNextHopMap();
#endif
    for (uint dst = 0; dst < v; dst++)
    {
      if (src == dst) continue;
#if DEBUG
      auto it = m.find(dst);
      if (it != m.end())
      {
        std::cout << "  " << dst << " -> " << it->second << '\n';
      }
      else
      {
        std::cout << "  " << dst << " -> unreachable\n";
      }
#else
      std::cout << dst << ' ' << nodes[src].getNextHop(dst) << '\n';
#endif
    }
#if DEBUG
    std::cout << '\n';
#endif
  }
}
#if DEBUG
void SwitchNode::showNeighbors()
{
    std::cout << "neighbors of node: " << id << '\n';
    for (uint v : neighbors)
    {
        std::cout << v << ' ';
    }
    std::cout << '\n';
}
void SwitchNode::ShowAdjLinks()
{
    std::cout << "adjlinks of node: " << id << '\n';
    for (uint l : adjLinks)
    {
        std::cout << l << ' ';
    }
    std::cout << '\n';
}
#endif