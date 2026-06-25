#include <iostream>

#include <memory>

#include <vector>
#include <set>
#include <map>
#include <queue>

#include <algorithm>


#define DEBUG 0


class SwitchNode
{
public:
  
  inline std::vector<size_t>& getNeighbors() { return neighbors; }
  inline std::vector<size_t>& getAdjLinks() { return adjLinks; }
  inline std::map<size_t, size_t>& getNextHopMap() { return nextHopMap; }
  inline std::map<size_t, size_t>& getNextLinkMap() { return nextLinkMap; }

  inline const std::vector<size_t>& getNeighbors() const { return neighbors; }
  inline const std::vector<size_t>& getAdjLinks() const { return adjLinks; }
  inline const std::map<size_t, size_t>& getNextHopMap() const { return nextHopMap; }
  inline const std::map<size_t, size_t>& getNextLinkMap() const { return nextLinkMap; }
  
  inline int getNextHop(unsigned int label) { return nextHopMap[label]; }
  
  inline void setNeighbor(size_t id) { neighbors.push_back(id); }
  inline void setAdjLink(size_t id) { adjLinks.push_back(id); }
  
  SwitchNode(size_t id)
    :id(id){}

#if DEBUG
  void showNeighbors();
  void ShowAdjLinks();
#endif
private:
  size_t id;
  std::vector<size_t> neighbors;
  std::vector<size_t> adjLinks;

  std::map<size_t, size_t> nextHopMap;
  std::map<size_t, size_t> nextLinkMap;
};

struct Link
{
  size_t id, v1, v2, capacity;

  Link(size_t id, size_t v1, size_t v2, size_t capacity)
    :id(id), v1(v1), v2(v2), capacity(capacity){}
};

class Graph
{
public:
  
  inline std::vector<SwitchNode>& getNodes() { return nodes; }
  inline std::vector<Link>& getLinks() { return links; }
  inline std::vector<size_t>& getCapacities() { return capacities; }
  inline std::vector<std::vector<size_t>>& getHops() { return hops; }
  
  void findAllPairShortestPath();
  void showNavigationMap();
  
  bool goAPSP(size_t src, size_t dst, size_t task_flow, std::vector<std::pair<size_t, size_t>>& path_out);
  bool checkAPSP(size_t src, size_t task_flow, const std::vector<std::pair<size_t, size_t>>& path);

  std::unique_ptr<std::vector<std::pair<size_t, size_t>>> widestPathBfs(size_t src, size_t dst, size_t task_flow, bool& success);
  std::unique_ptr<std::vector<std::pair<size_t, size_t>>> minLabelWidestPath(size_t src, size_t dst, size_t label_limit, size_t task_flow, bool& success);

#if 0
  std::unique_ptr<std::vector<std::pair<size_t, size_t>>> widestPathDijkstra(size_t src, size_t dst, size_t cap_need, bool& success);
#endif
  
  Graph(size_t n_link)
  {
      capacities.assign(n_link, 0);
  }
private:
  enum ShuffleStrategy { SEQUENTIAL= 0, REVERSE = 1};

  void buildMapBfs(size_t src);
private:
  std::vector<SwitchNode> nodes;
  std::vector<Link> links;

  std::vector<size_t> capacities; // capacities that each link already used
  std::vector<std::vector<size_t>> hops; // in navigation map, from src to dst cost how many hop
};


struct TaskFlow
{
  size_t id, src, dst, flowSize;
  double weight;

  std::unique_ptr<std::vector<std::pair<size_t, size_t>>> labels = nullptr;

  TaskFlow(size_t id, size_t src, size_t dst, size_t flow)
  :id(id), src(src), dst(dst), flowSize(flow), weight(0){}
};

#if DEBUG
void show_labels(const std::unique_ptr<std::vector<std::pair<size_t, size_t>>>& labels);
#endif

int main(void)
{
  size_t n_node, n_link, n_task, label_limit;
  std::vector<TaskFlow> tasks;
  
  // input
  std::cin >> n_node >> n_link >> n_task >> label_limit;
  
  Graph g(n_link);
  
  for(size_t i = 0; i < n_node; i++)
    g.getNodes().emplace_back(i);
  
  for(size_t i = 0; i < n_link; i++)
  {
    size_t id, v1, v2, c;
    std::cin >> id >> v1 >> v2 >> c;
    g.getLinks().emplace_back(id, v1, v2, c);
    g.getNodes()[v1].setNeighbor(v2);
    g.getNodes()[v1].setAdjLink(id);
    g.getNodes()[v2].setNeighbor(v1);
    g.getNodes()[v2].setAdjLink(id);
  }
  
  for(size_t i = 0; i < n_task; i++)
  {
    size_t id, src, dst, flow;
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
  std::vector<size_t> selected_tasks, remaining_tasks;
  size_t final_flow = 0;
  
#if 0
  auto flow_max_comp = [](const TaskFlow& a, const TaskFlow& b) { return a.flowSize > b.flowSize; };
  auto flow_min_comp = [](const TaskFlow& a, const TaskFlow& b) { return a.flowSize < b.flowSize; };
#endif
  
  // less hop prior
  auto hop_min_comp = [&](const TaskFlow& a, const TaskFlow& b)
    { return (g.getHops()[a.src][a.dst] < g.getHops()[b.src][b.dst]) ||
        ((g.getHops()[a.src][a.dst] == g.getHops()[b.src][b.dst]) && (a.flowSize > b.flowSize)); };

  std::sort(tasks.begin(), tasks.end(), hop_min_comp);

  // select fully label tasks
  for (size_t i = 0; i < n_task; i++)
  {
    size_t src = tasks[i].src, dst = tasks[i].dst, task_flow = tasks[i].flowSize;

    bool successed = false;

    std::unique_ptr<std::vector<std::pair<size_t, size_t>>> labels;

    labels = g.widestPathBfs(src, dst, task_flow, successed);

    // commit and select
    if (successed && labels && (*labels).size() <= label_limit)
    {
      // commit actual capacity cost
      for (const auto& [node_id, lid] : *labels)
        g.getCapacities()[lid] += task_flow;
      
      selected_tasks.push_back(i);
      tasks[i].labels = std::move(labels);
      final_flow += tasks[i].flowSize;
#if DEBUG
      show_labels(tasks[i].labels);
      std::cout << "Task " << task_id << " completed: " << src << " -> " << dst << '\n';
#endif
    }
    else
      remaining_tasks.push_back(i);
  }

  // try again for those long paths
  for (size_t i = 0; i < remaining_tasks.size(); i++)
  {
    size_t task_id = remaining_tasks[i];
    size_t src = tasks[task_id].src, dst = tasks[task_id].dst, task_flow = tasks[task_id].flowSize;

    bool successed = false;

    std::unique_ptr<std::vector<std::pair<size_t, size_t>>> labels;

    labels = g.minLabelWidestPath(src, dst, label_limit, task_flow, successed);

    // commit and select
    if (successed && labels)
    {
      // commit actual capacity cost
      for (const auto& [node_id, lid] : *labels)
        g.getCapacities()[lid] += task_flow;
      
      selected_tasks.push_back(task_id);
      tasks[task_id].labels = std::move(labels);
      final_flow += tasks[task_id].flowSize;
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

  size_t n_selected = selected_tasks.size();
  std::cout << n_selected << '\n';
  for (size_t i = 0; i < n_selected; i++)
  {
      size_t task_id = selected_tasks[i];
      std::cout << tasks[task_id].id << '\n';
      std::cout << tasks[task_id].src << ' ';
#if DEBUG
      show_labels(tasks[task_id].labels);
#endif
      for (size_t j = 0; j < tasks[task_id].labels->size(); j++)
      {
          size_t label = tasks[task_id].labels->at(j).first;
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
  size_t n_node = nodes.size();

  hops.resize(n_node);
  for (auto& vec : hops)
    vec.resize(n_node, SIZE_MAX);
  
  for(size_t src = 0; src < n_node; src++) // iterate for all node id as src
    buildMapBfs(src);
  
}

void Graph::buildMapBfs(size_t src)
{
  std::vector<size_t>& hop = hops[src];
  hop[src] = 0;

  std::queue<size_t> q;
  q.push(src);

  // update when bfs 
  auto& m = nodes[src].getNextHopMap();
  auto& lm = nodes[src].getNextLinkMap();
  m.clear();
  lm.clear();

  // level order bfs
  while (!q.empty())
  {
    size_t u = q.front();
    q.pop();

    auto neighbors = nodes[u].getNeighbors();
    auto adjlinks = nodes[u].getAdjLinks();

    switch ((ShuffleStrategy)(src % 2))
    {
    case REVERSE:
      std::reverse(neighbors.begin(), neighbors.end());
      std::reverse(adjlinks.begin(), adjlinks.end());
      break;
    case SEQUENTIAL:
      break;
    default:
      break;
    }

    for (size_t i = 0; i < neighbors.size(); i++)
    {
      size_t v = neighbors[i];
      size_t l = adjlinks[i];

      if (u == src) // the key of map will only be neighbors 
      {
        m[v] = v;
        lm[v] = l;
      }

      if (hop[v] == SIZE_MAX)
      {
        if (u != src) // the first step is the same of last step
        {
          m[v] = m[u];
          lm[v] = lm[u];
        }
        
        hop[v] = hop[u] + 1;
        q.push(v);
      }
    }
  }
}

std::unique_ptr<std::vector<std::pair<size_t, size_t>>> Graph::widestPathBfs(size_t src, size_t dst, size_t task_flow, bool& success)
{
  success = false;
  size_t n_node = nodes.size();

  // node id / link id connect to v
  std::vector<size_t> parent(n_node, SIZE_MAX);  // SIZE_MAX : no parent
  std::vector<size_t> parent_link(n_node, SIZE_MAX);  // pre link ID

  std::queue<size_t> q;
  std::vector<bool> visited(n_node, false);

  q.push(src);
  visited[src] = true;

  bool found = false; // find path successfully
  
  while (!q.empty() && !found)
  {
    size_t u = q.front(); q.pop();

    auto neighbors = nodes[u].getNeighbors();
    auto adjlinks = nodes[u].getAdjLinks();

    // sort by strategy 
    switch ((ShuffleStrategy)(task_flow % 2)) {
      case REVERSE:
        std::reverse(neighbors.begin(), neighbors.end());
        std::reverse(adjlinks.begin(), adjlinks.end());
        break;
        break;
      case SEQUENTIAL:
      default:
        break;
    }

    for (size_t i = 0; i < neighbors.size(); i++)
    {
      size_t v = neighbors[i];
      size_t lid = adjlinks[i];

      // check for link capacity
      if (capacities[lid] + task_flow <= links[lid].capacity && !visited[v]) 
      {
        visited[v] = true;
        parent[v] = u;
        parent_link[v] = lid;
        q.push(v);

        if (v == dst)
        {
          found = true;
          break;
        }
      }
    }
  }

  if (!found || parent[dst] == SIZE_MAX) // no connected node to dst
      return nullptr;
  

  // reconstruct path (trace back)
  std::vector<std::pair<size_t, size_t>> path;
  size_t curr = dst;
  while (curr != src)
  {
    size_t prev = parent[curr];
    size_t lid = parent_link[curr];
    path.emplace_back(curr, lid);  // (node, link_id)
    curr = prev;
  }

  // actual path
  std::reverse(path.begin(), path.end());
  success = true;

  return std::make_unique<std::vector<std::pair<size_t, size_t>>>(std::move(path));
}

bool Graph::goAPSP(size_t src, size_t dst, size_t task_flow, std::vector<std::pair<size_t, size_t>>& path_out)
{
  // force rebuild
  path_out.clear();
  path_out.shrink_to_fit();  // release old memories
  path_out.reserve(16);      // prealloc

  // find path by navigation map
  size_t curr = src;
  while (curr != dst)
  {
    auto& node_curr = nodes[curr];
    size_t next_hop = node_curr.getNextHop(dst);
    size_t lid = node_curr.getNextLinkMap()[dst];

    if (lid == SIZE_MAX || capacities[lid] + task_flow > links[lid].capacity)
    {
      path_out.clear();  // ensure empty when fail
      return false;
    }

    path_out.emplace_back(next_hop == dst ? dst : SIZE_MAX, lid);
    curr = next_hop;
  }
  return true;
}

bool Graph::checkAPSP(size_t src, size_t task_flow, const std::vector<std::pair<size_t, size_t>>& path)
{
  std::vector<bool> throughlinks(links.size(), false); // avoid for walk on same link twice

  size_t curr_label = src;

  for (size_t i = 0; i < path.size(); )
  {
    // skip to next label
    while (i < path.size() && path[i].first == SIZE_MAX) ++i;
    if (i == path.size()) break;

    size_t next_label = path[i].first;

    // final validation for a multi-label path
    size_t test_node = curr_label;
    while (test_node != next_label)
    {
      size_t next_hop = nodes[test_node].getNextHop(next_label);
      size_t lid = nodes[test_node].getNextLinkMap()[next_label];

      if (lid == SIZE_MAX || throughlinks[lid] || capacities[lid] + task_flow > links[lid].capacity)
        return false;
      
      throughlinks[lid] = true;
      test_node = next_hop;
    }

    curr_label = next_label;
    ++i;
  }

  return true;
}

std::unique_ptr<std::vector<std::pair<size_t, size_t>>> Graph::minLabelWidestPath(size_t src, size_t dst, size_t label_limit, size_t task_flow, bool& success)
{
  success = false;

  std::unique_ptr<std::vector<std::pair<size_t, size_t>>> path_labels;
  path_labels.reset();

  // base path
  bool base_path_ok = false;
  auto base_path = widestPathBfs(src, dst, task_flow, base_path_ok);

  if (!base_path_ok || !base_path) // fail
  {
    path_labels.reset();
    success = false;
    return path_labels;  // nullptr
  }

  const auto& path_nodes_links = *base_path;
  size_t path_length = path_nodes_links.size();

  // dst is neighbor of src
  if (path_length == 1)
  {
    path_labels = std::make_unique<std::vector<std::pair<size_t, size_t>>>();
    size_t link_id = path_nodes_links[0].second;
    size_t node_id = path_nodes_links[0].first;
    if (capacities[link_id] + task_flow <= links[link_id].capacity)
    {
      path_labels->emplace_back(node_id, link_id);
      success = true;
    }
    return path_labels;
  }

  // dp[pos][n_label_used] = max remaining capacity of all links
  std::vector<std::vector<size_t>> dp(path_length + 1, std::vector<size_t>(label_limit + 1, 0));
  std::vector<std::vector<std::pair<int, int>>> predecessor(path_length + 1, std::vector<std::pair<int, int>>(label_limit + 1, { -1, -1 }));

  dp[0][0] = SIZE_MAX;  // src

  // dp
  for (size_t pos = 1; pos <= path_length; ++pos)
  {
    size_t curr_node = path_nodes_links[pos - 1].first;
    size_t curr_link = path_nodes_links[pos - 1].second;
    size_t edge_capacity = links[curr_link].capacity - capacities[curr_link];

    for (size_t labels_used = 0; labels_used <= label_limit; ++labels_used)
    {
      // use curr_node as label
      if (labels_used > 0)
      {
        for (size_t prev_pos = 0; prev_pos < pos; ++prev_pos)
        {
          if (dp[prev_pos][labels_used - 1] == 0) continue;

          size_t prev_node = (prev_pos == 0) ? src : path_nodes_links[prev_pos - 1].first;

          // check for navigation map
          std::vector<std::pair<size_t, size_t>> apsp_segment;
          apsp_segment.reserve(4);
          if (goAPSP(prev_node, curr_node, task_flow, apsp_segment))
          {
            size_t segment_min_cap = SIZE_MAX;
            for (const auto& [n_id, l_id] : apsp_segment)
              segment_min_cap = std::min<size_t>(segment_min_cap, links[l_id].capacity - capacities[l_id]);
            

            size_t new_capacity = std::min(std::min<size_t>(dp[prev_pos][labels_used - 1], segment_min_cap), edge_capacity);
            if (new_capacity > dp[pos][labels_used])
            {
              dp[pos][labels_used] = new_capacity;
              predecessor[pos][labels_used] = { prev_pos, labels_used - 1 };
            }
          }
        }
      }

      // not use curr_node as label
      int prev_label_pos = predecessor[pos - 1][labels_used].first;
      if (prev_label_pos != -1 && dp[pos - 1][labels_used] > 0)
      {
        size_t prev_node = (prev_label_pos == 0) ? src : path_nodes_links[prev_label_pos - 1].first;

        std::vector<std::pair<size_t, size_t>> apsp_segment;
        apsp_segment.reserve(4);
        if (goAPSP(prev_node, curr_node, task_flow, apsp_segment))
        {
          size_t new_capacity = dp[pos - 1][labels_used];
          dp[pos][labels_used] = std::max(dp[pos][labels_used], new_capacity);
          predecessor[pos][labels_used] = predecessor[pos - 1][labels_used];
        }
      }
    }
  }

  // select best label list
  size_t best_capacity = 0;
  int best_labels_used = -1;
  for (size_t k = 0; k <= label_limit; ++k)
  {
    if (dp[path_length][k] > best_capacity)
    {
      best_capacity = dp[path_length][k];
      best_labels_used = k;
    }
  }

  if (best_capacity == 0)
  {
    path_labels.reset();
    success = false;
    return path_labels; // nullptr
  }

  // reconstruct path label select list
  std::vector<int> label_positions;
  int current_pos = path_length;
  int current_labels = best_labels_used;

  while (current_pos > 0)
  {
    auto [prev_pos, prev_labels] = predecessor[current_pos][current_labels];

    if (current_labels != prev_labels) // add label
      label_positions.push_back(current_pos);
    
    current_pos = prev_pos;
    current_labels = prev_labels;
  }

  std::reverse(label_positions.begin(), label_positions.end());

  // merge final path and validation
  size_t current_node = src;
  path_labels = std::make_unique<std::vector<std::pair<size_t, size_t>>>();

  for (size_t label_idx = 0; label_idx <= label_positions.size(); ++label_idx)
  {
    size_t next_label_pos = (label_idx < label_positions.size()) ? label_positions[label_idx] : path_length;
    size_t next_label_node = (next_label_pos == path_length) ? dst : path_nodes_links[next_label_pos - 1].first;

    // construct segment path from current_node to next_label by APSP navigation map
    std::vector<std::pair<size_t, size_t>> segment_edges;
    if (!goAPSP(current_node, next_label_node, task_flow, segment_edges))
    {
      path_labels.reset();
      success = false;
      return path_labels;  // fail
    }

    // add complete segment
    for (const auto& [target_node, link_id] : segment_edges)
      path_labels->emplace_back((target_node == next_label_node) ? next_label_node : SIZE_MAX, link_id);
    

    current_node = next_label_node;
  }
  // final check avoid go through certain link twice
  if (!checkAPSP(src, task_flow, *path_labels))
  {
    path_labels.reset();
    success = false;
    return path_labels;
  }

  success = true;
  return path_labels;
}

void Graph::showNavigationMap() 
{
  size_t v = nodes.size();
  for (size_t src = 0; src < v; src++)
  {
#if DEBUG
    std::cout << "Source: ";
#endif
    std::cout << src << '\n';

#if DEBUG
    const auto& m = nodes[src].getNextHopMap();
#endif
    for (size_t dst = 0; dst < v; dst++)
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
void show_labels(const std::unique_ptr<std::vector<std::pair<size_t, size_t>>>& labels)
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
void SwitchNode::showNeighbors()
{
    std::cout << "neighbors of node: " << id << '\n';
    for (size_t v : neighbors)
    {
        std::cout << v << ' ';
    }
    std::cout << '\n';
}
void SwitchNode::ShowAdjLinks()
{
    std::cout << "adjlinks of node: " << id << '\n';
    for (size_t l : adjLinks)
    {
        std::cout << l << ' ';
    }
    std::cout << '\n';
}
#endif

#if 0
std::unique_ptr<std::vector<std::pair<size_t, size_t>>> Graph::widestPathDijkstra(size_t src, size_t dst, size_t task_flow, bool& success)
{
  size_t n = nodes.size();
  std::vector<size_t> remain_cap(n, 0);
  std::vector<size_t> prev(n, SIZE_MAX);
  std::vector<size_t> prev_link(n, SIZE_MAX);

  remain_cap[src] = SIZE_MAX;

  // max heap
  struct MaxCapacityComp
  {
    bool operator()(const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) const 
      { return a.first < b.first; }
  };
  std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, MaxCapacityComp > pq;
  pq.push({ SIZE_MAX, src });

  while (!pq.empty()) 
  {
    size_t c = pq.top().first;
    size_t u = pq.top().second;
    pq.pop();

    if (c < remain_cap[u]) continue;

    
    if (u == dst) 
    {
      if (remain_cap[dst] < task_flow) // no path to select
      {
          success = false;
          return nullptr;
      }
      // reconstruct path
      std::unique_ptr<std::vector<std::pair<size_t, size_t>>> path = std::make_unique<std::vector<std::pair<size_t, size_t>>>();
      size_t curr = dst;
      while (curr != src)
      {
        path->emplace_back(curr, prev_link[curr]);
        curr = prev[curr];
        if (curr == SIZE_MAX) // no path
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

    for (size_t i = 0; i < neighbors.size(); i++)
    {
      size_t v = neighbors[i];
      size_t l = adjlinks[i];

      // remain = link.capacity - capacity
      size_t edge_remain = links[l].capacity - capacities[l];
      size_t new_remain = std::min<size_t>(remain_cap[u], edge_remain);

      if (new_remain > remain_cap[v])
      {
        remain_cap[v] = new_remain;
        prev[v] = u;  // pre node
        prev_link[v] = l;
        pq.push({ new_remain, v });
      }
    }
  }

  return nullptr;  // nothing
}
#endif