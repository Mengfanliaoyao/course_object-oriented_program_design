#include <iostream>

#include <memory>

#include <vector>
#include <map>
#include <queue>

#include <algorithm>
#include <climits>


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

  inline const std::vector<uint>& getNeighbors() const { return neighbors; }
  inline const std::vector<uint>& getAdjLinks() const { return adjLinks; }
  inline const std::map<uint, uint>& getNextHopMap() const { return nextHopMap; }
  inline const std::map<uint, uint>& getNextLinkMap() const { return nextLinkMap; }
  
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
  inline std::vector<uint>& getCapacities() { return capacities; }
  
  void findAllPairShortestPath();
  void showNavigationMap();
  
  std::unique_ptr<std::vector<std::pair<uint, uint>>> widestPathDijkstra(uint src, uint dst, uint cap_need, bool& success);

  Graph(uint n_link)
  {
      capacities.assign(n_link, 0);
  }
private:
  
  void bfs(uint src);
private:
  std::vector<SwitchNode> nodes;
  std::vector<Link> links;
  std::vector<uint> capacities; // capacities that each link already used
  
  std::vector<std::vector<uint>> hops; // in navigation map, from src to dst cost how many hop
};


struct TaskFlow
{
  uint id, src, dst, flowSize;
  double weight;

  std::unique_ptr<std::vector<std::pair<uint, uint>>> labels = nullptr;

  TaskFlow(uint id, uint src, uint dst, uint flow)
  :id(id), src(src), dst(dst), flowSize(flow), weight(0){}
};

void show_labels(const std::unique_ptr<std::vector<std::pair<uint, uint>>>& labels);


int main(void)
{
  uint n_node, n_link, n_task, label_limit;
  std::vector<TaskFlow> tasks;
  
  // input
  std::cin >> n_node >> n_link >> n_task >> label_limit;
  
  Graph g(n_link);
  
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
  std::vector<uint> selected_tasks;
  uint final_flow = 0;
  
//  auto flow_max_comp = [](const TaskFlow& a, const TaskFlow& b) { return a.flowSize > b.flowSize; };
//  auto flow_min_comp = [](const TaskFlow& a, const TaskFlow& b) { return a.flowSize < b.flowSize; };
//  std::sort(tasks.begin(), tasks.end(), flow_min_comp);

  for (uint i = 0; i < n_task; i++)
  {
      uint task_id = tasks[i].id;
      uint src = tasks[i].src, dst = tasks[i].dst, task_flow = tasks[i].flowSize;

      bool successed = false;
      std::unique_ptr<std::vector<std::pair<uint, uint>>> labels;

      if (label_limit == 1) // can't use any SR
      {
      TRY:
          uint curr = src;
          labels = std::make_unique<std::vector<std::pair<uint, uint>> >();
          while (curr != dst)
          {
#if DEBUG
              std::cout << curr << ' ';
#endif
              auto& node_src = g.getNodes()[curr];
              uint next_hop = node_src.getNextHop(dst);
              uint lid = node_src.getNextLinkMap()[dst];

              Link& link = g.getLinks()[lid];

              uint link_cap = link.capacity;

              if (g.getCapacities()[lid] + task_flow > link_cap) // short of capacity
              {
                  successed = false;
                  labels.reset();
                  break;
              }
              curr = next_hop;
              (*labels).emplace_back(UINT_MAX, lid); // invalid node id to means the node is not label
              if (next_hop == dst)
                  successed = true;
              
#if DEBUG
          std::cout << '\n';
#endif
          }
      }
      else
      {
         labels = g.widestPathDijkstra(src, dst, tasks[i].flowSize, successed);

         if (successed && labels && labels->size() > label_limit)
         {
             successed = false;
             // cut label until match limit
         }
         if (!successed || !labels)
            goto TRY;
      }

      // commit and select
      if (successed && labels)
      {
          // commit actual capacity cost
          auto& capacities = g.getCapacities();
          for (uint j = 0; j < labels->size(); j++) // for each link passed
          {
              uint lid = (*labels)[j].second;
              capacities[lid] += tasks[i].flowSize;
          }
          selected_tasks.push_back(i);
          tasks[i].labels = std::move(labels);
          final_flow += tasks[i].flowSize;
#if DEBUG
          show_labels(tasks[i].labels);
          std::cout << "Task " << task_id << " completed: " << src << " -> " << dst << '\n';
#endif
      }
      else
      {
#if DEBUG
          std::cout << "Task " << task_id << " rejected: insufficient capacity\n";
#endif  
      }    
  }
  
  // output
  g.showNavigationMap();

  uint n_selected = selected_tasks.size();
  std::cout << n_selected << '\n';
  for (uint i = 0; i < n_selected; i++)
  {
      uint task_id = selected_tasks[i];
      std::cout << tasks[task_id].id << '\n';
      std::cout << tasks[task_id].src << ' ';
#if DEBUG
      show_labels(tasks[task_id].labels);
#endif
      for (uint j = 0; j < tasks[task_id].labels->size(); j++)
      {
          uint label = tasks[task_id].labels->at(j).first;
          if (label < n_node) // valid label
            std::cout << label << ' ';
      }
      std::cout << '\n';
  }
  std::cout << final_flow;
  return 0;
}


void Graph::findAllPairShortestPath()
{
  uint v = nodes.size();

  hops.resize(v);
  for (auto& vec : hops) vec.resize(v, UINT_MAX);
  
  for(uint src = 0; src < v; src++) // iterate for all node id as src
  {
      bfs(src);
  }
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

            if (hop[v] == UINT_MAX)
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

std::unique_ptr<std::vector<std::pair<uint, uint>>> Graph::widestPathDijkstra(uint src, uint dst, uint cap_need, bool& success)
{
    uint n = nodes.size();
    std::vector<uint> remain_cap(n, 0);
    std::vector<uint> prev(n, UINT_MAX);
    std::vector<uint> prev_link(n, UINT_MAX);

    remain_cap[src] = UINT_MAX;

    // max heap
    struct MaxCapacityComp
    {
        bool operator()(const std::pair<uint, uint>& a, const std::pair<uint, uint>& b) const 
        { return a.first < b.first; }
    };
    std::priority_queue<std::pair<uint, uint>, std::vector<std::pair<uint, uint>>, MaxCapacityComp > pq;
    pq.push({ UINT_MAX, src });

    while (!pq.empty()) 
    {
        uint c = pq.top().first;
        uint u = pq.top().second;
        pq.pop();

        if (c < remain_cap[u]) continue;

        
        if (u == dst) 
        {
            if (remain_cap[dst] < cap_need) // no path to select
            {
                success = false;
                return nullptr;
            }
            // reconstruct path
            std::unique_ptr<std::vector<std::pair<uint, uint>>> path = std::make_unique<std::vector<std::pair<uint, uint>>>();
            uint curr = dst;
            while (curr != src) {
                path->emplace_back(curr, prev_link[curr]);
                curr = prev[curr];
                if (curr == UINT_MAX) // no path
                {
                    return nullptr;
                }
            }
            std::reverse(path->begin(), path->end());
            success = true;
            return path;  // exclude src (actual labels)
        }

        const auto& neighbors = nodes[u].getNeighbors();
        const auto& adjlinks = nodes[u].getAdjLinks();

        for (uint i = 0; i < neighbors.size(); i++) {
            uint v = neighbors[i];
            uint l = adjlinks[i];

            // remain = link.capacity - capacity
            uint edge_remain = links[l].capacity - capacities[l];
            uint new_remain = std::min(remain_cap[u], edge_remain);

            if (new_remain > remain_cap[v]) {
                remain_cap[v] = new_remain;
                prev[v] = u;  // pre node
                prev_link[v] = l;
                pq.push({ new_remain, v });
            }
        }
    }

    return nullptr;  // nothing
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

void show_labels(const std::unique_ptr<std::vector<std::pair<uint, uint>>>& labels)
{
    std::cout << "=== Label Debug ===\n";


    if (!labels) 
    {
        std::cout << "labels is nullptr\n";
        return;
    }


    if (!labels->empty()) // empty vector
    {
        std::cout << "Labels size: " << labels->size() << "\n";
        for (size_t i = 0; i < labels->size(); i++) {
            const auto& p = (*labels)[i];
            std::cout << p.first << " ";
        }
        std::cout << '\n';
    }
    else
    {
        std::cout << "labels vector is empty\n";
    }

    std::cout << "=== End ===\n";
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