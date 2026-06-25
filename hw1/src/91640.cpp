#include <iostream>

#include <memory>

#include <vector>
#include <set>
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
  inline std::vector<std::vector<uint>>& getHops() { return hops; }
  
  void findAllPairShortestPath();
  void showNavigationMap();
  

  bool goAPSP(uint src, uint dst, uint task_flow, std::vector<std::pair<uint, uint>>& path_out);
  bool checkAPSP(uint src, uint task_flow, const std::vector<std::pair<uint, uint>>& path);

  std::unique_ptr<std::vector<std::pair<uint, uint>>> widestPathBfs(uint src, uint dst, uint task_flow, bool& success);
  std::unique_ptr<std::vector<std::pair<uint, uint>>> widestPathDijkstra(uint src, uint dst, uint cap_need, bool& success);
  std::unique_ptr<std::vector<std::pair<uint, uint>>> minLabelWidestPath(uint src, uint dst, uint label_limit, uint task_flow, bool& success);

  Graph(uint n_link)
  {
      capacities.assign(n_link, 0);
  }
private:
  enum ShuffleStrategy { SEQUENTIAL= 0, REVERSE = 1, DEGREE = 2, XOR = 3 };

  void buildMapBfs(uint src);
  void sortNeighborsByDegree(uint u);
  void sortNeighborsByXor(uint u);
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
  auto hop_min_comp = [&](const TaskFlow& a, const TaskFlow& b) { return g.getHops()[a.src][a.dst] < g.getHops()[a.src][a.dst]; };
  std::sort(tasks.begin(), tasks.end(), hop_min_comp);

  for (uint i = 0; i < n_task; i++)
  {
      uint task_id = tasks[i].id;
      uint src = tasks[i].src, dst = tasks[i].dst, task_flow = tasks[i].flowSize;

      bool successed = false;
      std::unique_ptr<std::vector<std::pair<uint, uint>>> labels;

      labels = g.minLabelWidestPath(src, dst, label_limit, task_flow, successed);

#if 0
      if (!successed)
      {
          labels = std::make_unique<std::vector<std::pair<uint, uint>>>();
          successed = g.goAPSP(src, dst, task_flow, *labels);
      }
#endif
      // commit and select
      if (successed && labels)
      {
          // commit actual capacity cost
          for (const auto& [node_id, lid] : *labels)
          {
              g.getCapacities()[lid] += task_flow;
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
      buildMapBfs(src);
  }
}

void Graph::buildMapBfs(uint src)
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

        auto neighbors = nodes[u].getNeighbors();
        auto adjlinks = nodes[u].getAdjLinks();

        switch ((ShuffleStrategy)((src * u) % 4))
        {
        case REVERSE:
            std::reverse(neighbors.begin(), neighbors.end());
            std::reverse(adjlinks.begin(), adjlinks.end());
            break;
        case DEGREE:
            sortNeighborsByDegree(u);
            break;
        case XOR:
            sortNeighborsByXor(u);
            break;
        case SEQUENTIAL:
            break;
        default:
            break;
        }

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

std::unique_ptr<std::vector<std::pair<uint, uint>>> Graph::widestPathBfs(uint src, uint dst, uint task_flow, bool& success)
{

    success = false;
    uint n_node = nodes.size();

    // parent[lid] 記錄到達 v 的前驅邊
    std::vector<uint> parent(n_node, UINT_MAX);  // -1 表示無父
    std::vector<uint> parent_link(n_node, UINT_MAX);  // 前驅邊 ID

    std::queue<uint> q;
    std::vector<bool> visited(n_node, false);

    q.push(src);
    visited[src] = true;

    bool found = false;
    while (!q.empty() && !found) {
        uint u = q.front(); q.pop();

        // 獲取鄰居（依 strategy 排序）
        auto neighbors = nodes[u].getNeighbors();
        auto adjlinks = nodes[u].getAdjLinks();

        // Strategy 排序
        switch ((ShuffleStrategy)((u*dst)%4)) {
        case REVERSE:
            std::reverse(neighbors.begin(), neighbors.end());
            std::reverse(adjlinks.begin(), adjlinks.end());
            break;
        case DEGREE:
            // 按度數排序（高到低）
            sortNeighborsByDegree(u);
            break;
        case XOR:
            // XOR hash 排序
            sortNeighborsByXor(u);
            break;
        case SEQUENTIAL:
        default:
            break;
        }

        // 遍歷鄰居，動態檢查剩餘容量
        for (uint i = 0; i < neighbors.size(); i++) {
            uint v = neighbors[i];
            uint lid = adjlinks[i];

            // 動態剩餘容量檢查
            if (capacities[lid] + task_flow <= links[lid].capacity && !visited[v]) 
            {
                visited[v] = true;
                parent[v] = u;
                parent_link[v] = lid;
                q.push(v);

                if (v == dst) {
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found || parent[dst] == UINT_MAX) {
        return nullptr;
    }

    // 重建路徑（逆向）
    std::vector<std::pair<uint, uint>> path;
    uint curr = dst;
    while (curr != src) {
        uint prev = parent[curr];
        uint lid = parent_link[curr];
        path.emplace_back(curr, lid);  // (node, link_id)
        curr = prev;
    }

    std::reverse(path.begin(), path.end());
    success = true;

    return std::make_unique<std::vector<std::pair<uint, uint>>>(std::move(path));
}

std::unique_ptr<std::vector<std::pair<uint, uint>>> Graph::widestPathDijkstra(uint src, uint dst, uint task_flow, bool& success)
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
            if (remain_cap[dst] < task_flow) // no path to select
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
            uint new_remain = std::min<uint>(remain_cap[u], edge_remain);

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


bool Graph::goAPSP(uint src, uint dst, uint task_flow, std::vector<std::pair<uint, uint>>& path_out)
{
    // 強制重建，無條件清空
    path_out.clear();
    path_out.shrink_to_fit();  // 釋放舊容量
    path_out.reserve(16);      // 預分配

    uint curr = src;
    while (curr != dst) {
        auto& node_curr = nodes[curr];
        uint next_hop = node_curr.getNextHop(dst);
        uint lid = node_curr.getNextLinkMap()[dst];

        if (lid == UINT_MAX || capacities[lid] + task_flow > links[lid].capacity) {
            path_out.clear();  // 確保失敗時為空
            return false;
        }

        path_out.emplace_back(next_hop == dst ? dst : UINT_MAX, lid);
        curr = next_hop;
    }
    return true;
}

bool Graph::checkAPSP(uint src, uint task_flow, const std::vector<std::pair<uint, uint>>& path)
{
    std::vector<bool> throughlinks(links.size(), false);

    uint curr_label = src;

    for (uint i = 0; i < path.size(); ) {
        // 跳到下一個非UINT_MAX（label點）
        while (i < path.size() && path[i].first == UINT_MAX) ++i;
        if (i == path.size()) break;

        uint next_label = path[i].first;

        // 驗證 curr_label → next_label 的APSP
        uint test_node = curr_label;
        while (test_node != next_label) {
            uint next_hop = nodes[test_node].getNextHop(next_label);
            uint lid = nodes[test_node].getNextLinkMap()[next_label];

            if (lid == UINT_MAX || throughlinks[lid] || capacities[lid] + task_flow > links[lid].capacity) {
                return false;
            }

            throughlinks[lid] = true;
            test_node = next_hop;
        }

        curr_label = next_label;
        ++i;
    }

    return true;
}

std::unique_ptr<std::vector<std::pair<uint, uint>>> Graph::minLabelWidestPath(uint src, uint dst, uint max_labels, uint task_flow, bool& success)
{

    success = false;
    std::unique_ptr<std::vector<std::pair<uint, uint>>> path_labels;
    path_labels.reset();

    // 1. 基礎路徑：Widest Path
    bool base_path_ok = false;
    auto base_path = widestPathBfs(src, dst, task_flow, base_path_ok);
    if (!base_path_ok || !base_path)
    {
        path_labels.reset();
        success = false;
        return path_labels;  // nullptr
    }

    const auto& path_nodes_links = *base_path;
    int path_length = path_nodes_links.size();

    // 2. 特殊情況：直達或很短路徑
    if (path_length == 1) {
        path_labels = std::make_unique<std::vector<std::pair<uint, uint>>>();
        uint link_id = path_nodes_links[0].second;
        uint node_id = path_nodes_links[0].first;
        if (capacities[link_id] + task_flow <= links[link_id].capacity) {
            path_labels->emplace_back(node_id, link_id);
            success = true;
        }
        return path_labels;
    }

    // 3. DP：dp[pos][used_labels] = 最大剩餘容量
    std::vector<std::vector<uint>> dp(path_length + 1, std::vector<uint>(max_labels + 1, 0));
    std::vector<std::vector<std::pair<int, int>>> predecessor(path_length + 1,
        std::vector<std::pair<int, int>>(max_labels + 1, { -1, -1 }));

    dp[0][0] = UINT_MAX;  // 起點 src

    // 4. DP 填表
    for (int pos = 1; pos <= path_length; ++pos) {
        uint curr_node = path_nodes_links[pos - 1].first;
        uint curr_link = path_nodes_links[pos - 1].second;
        uint edge_capacity = links[curr_link].capacity - capacities[curr_link];

        for (uint labels_used = 0; labels_used <= max_labels; ++labels_used) {
            // A. 使用 label 指定當前點
            if (labels_used > 0) {
                for (int prev_pos = 0; prev_pos < pos; ++prev_pos) {
                    if (dp[prev_pos][labels_used - 1] == 0) continue;

                    uint prev_node = (prev_pos == 0) ? src : path_nodes_links[prev_pos - 1].first;

                    // APSP 檢查：prev_node → curr_node
                    std::vector<std::pair<uint, uint>> apsp_segment;
                    apsp_segment.reserve(4);
                    if (goAPSP(prev_node, curr_node, task_flow, apsp_segment)) {
                        uint segment_min_cap = UINT_MAX;
                        for (const auto& [n_id, l_id] : apsp_segment) {
                            segment_min_cap = std::min<uint>(segment_min_cap,
                                links[l_id].capacity - capacities[l_id]);
                        }

                        uint new_capacity = std::min(std::min<uint>(dp[prev_pos][labels_used - 1], segment_min_cap), edge_capacity);
                        if (new_capacity > dp[pos][labels_used]) {
                            dp[pos][labels_used] = new_capacity;
                            predecessor[pos][labels_used] = { prev_pos, labels_used - 1 };
                        }
                    }
                }
            }

            // B. 不使用 label：APSP 經過當前點
            int prev_label_pos = predecessor[pos - 1][labels_used].first;
            if (prev_label_pos != -1 && dp[pos - 1][labels_used] > 0) {
                uint prev_node = (prev_label_pos == 0) ? src : path_nodes_links[prev_label_pos - 1].first;

                std::vector<std::pair<uint, uint>> apsp_segment;
                apsp_segment.reserve(4);
                if (goAPSP(prev_node, curr_node, task_flow, apsp_segment)) {
                    uint new_capacity = dp[pos - 1][labels_used];
                    dp[pos][labels_used] = std::max(dp[pos][labels_used], new_capacity);
                    predecessor[pos][labels_used] = predecessor[pos - 1][labels_used];
                }
            }
        }
    }

    // 5. 選擇最佳解
    uint best_capacity = 0;
    int best_labels_used = -1;
    for (uint k = 0; k <= max_labels; ++k) {
        if (dp[path_length][k] > best_capacity) {
            best_capacity = dp[path_length][k];
            best_labels_used = k;
        }
    }

    if (best_capacity == 0)
    {
        path_labels.reset();
        success = false;
        return path_labels;
    }

    // 6. 路徑重建：收集 label 點
    std::vector<int> label_positions;
    int current_pos = path_length;
    int current_labels = best_labels_used;

    while (current_pos > 0) {
        auto [prev_pos, prev_labels] = predecessor[current_pos][current_labels];

        if (current_labels != prev_labels) {  // 新增 label
            label_positions.push_back(current_pos);
        }
        current_pos = prev_pos;
        current_labels = prev_labels;
    }

    std::reverse(label_positions.begin(), label_positions.end());

    // 7. 最終驗證並建構完整邊路徑
    uint current_node = src;
    path_labels = std::make_unique<std::vector<std::pair<uint, uint>>>();

    for (size_t label_idx = 0; label_idx <= label_positions.size(); ++label_idx)
    {
        uint next_label_pos = (label_idx < label_positions.size()) ? label_positions[label_idx] : path_length;
        uint next_label_node = (next_label_pos == path_length) ? dst : path_nodes_links[next_label_pos - 1].first;

        // APSP：current_node → next_label_node
        std::vector<std::pair<uint, uint>> segment_edges;
        if (!goAPSP(current_node, next_label_node, task_flow, segment_edges))
        {
            path_labels.reset();
            success = false;
            return path_labels;  // 驗證失敗
        }

        // 加入完整 segment
        for (const auto& [target_node, link_id] : segment_edges)
        {
            path_labels->emplace_back(
                (target_node == next_label_node) ? next_label_node : UINT_MAX,
                link_id
            );
        }

        current_node = next_label_node;
    }
    if (!checkAPSP(src, task_flow, *path_labels))
    {
        path_labels.reset();
        success = false;
        return path_labels;
    }

    success = true;
    return path_labels;
}

void Graph::sortNeighborsByDegree(uint u)
{
    auto& neighbors = nodes[u].getNeighbors();
    auto& adjlinks = nodes[u].getAdjLinks();

    std::vector<std::pair<uint, uint>> deg_list;
    for (uint i = 0; i < neighbors.size(); i++) {
        uint v = neighbors[i];
        deg_list.emplace_back(nodes[v].getNeighbors().size(), i);
    }
    std::sort(deg_list.rbegin(), deg_list.rend());

    std::vector<uint> sorted_neighbors, sorted_links;
    for (auto& [deg, idx] : deg_list) {
        sorted_neighbors.push_back(neighbors[idx]);
        sorted_links.push_back(adjlinks[idx]);
    }
    neighbors = std::move(sorted_neighbors);
    adjlinks = std::move(sorted_links);
}
void Graph::sortNeighborsByXor(uint u)
{
    auto& neighbors = nodes[u].getNeighbors();
    auto& adjlinks = nodes[u].getAdjLinks();

    std::vector<std::pair<uint, uint>> xor_list;
    for (uint i = 0; i < neighbors.size(); i++) {
        uint v = neighbors[i];
        xor_list.emplace_back(u ^ v ^ adjlinks[i], i);
    }
    std::sort(xor_list.begin(), xor_list.end());

    std::vector<uint> sorted_n, sorted_l;
    for (auto& [hash_val, idx] : xor_list) {
        sorted_n.push_back(neighbors[idx]);
        sorted_l.push_back(adjlinks[idx]);
    }
    neighbors = std::move(sorted_n);
    adjlinks = std::move(sorted_l);
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