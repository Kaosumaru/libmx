#pragma once
#include <map>
#include <queue>
#include <functional>


namespace MX
{

	template<typename NodeType>
	class DijkstraPathFinding
	{
	public:
		using DistanceType = float;

		using NeighborCallback = std::function<void(const NodeType& node, DistanceType distance)>;
		using GetNeighborsFunc = std::function<void(const NodeType& node, const NeighborCallback& callback)>;

		using SetDistanceIfLowerFunc = std::function<bool(const NodeType& node, DistanceType distance)>;

		using QueuePair = std::pair<DistanceType, NodeType>;
		using PriorityQueue = std::priority_queue<QueuePair, std::vector<QueuePair>, std::greater<QueuePair>>;

		struct Functions
		{
			GetNeighborsFunc getNeighbors;
			SetDistanceIfLowerFunc setDistanceIfLower;
		};

		DijkstraPathFinding(const std::vector<NodeType>& startNodes, const Functions& f)
		{
			PriorityQueue queue;

			DistanceType startDistance = 0.0f;
			for (auto& start : startNodes)
			{
				queue.push(std::make_pair(startDistance, start));
				f.setDistanceIfLower(start, startDistance);
			}

			PathFind(queue, f);
		}

		DijkstraPathFinding(const NodeType& start, const Functions& f)
		{
			PriorityQueue queue;

			DistanceType startDistance = 0.0f;
			queue.push(std::make_pair(startDistance, start));
			f.setDistanceIfLower(start, startDistance);

			PathFind(queue, f);
		}


	protected:
		void PathFind(PriorityQueue& queue, const Functions& f)
		{
			while (!queue.empty())
			{
				auto p = queue.top();
				queue.pop();

				DistanceType current = p.first;


				auto forN = [&](const NodeType& node, DistanceType distance)
				{
					auto newDistance = current + distance;
					if (!f.setDistanceIfLower(node, newDistance))
						return;
					queue.push(std::make_pair(newDistance, node));

				};
				f.getNeighbors(p.second, forN);
			}
		}
	};

	template<typename NodeType>
	class DefaultDijkstraPathFinding
	{
	public:
		using DijkstraPath = DijkstraPathFinding<NodeType>;
		using DistanceType = typename DijkstraPath::DistanceType;
		using GetNeighborsFunc = typename DijkstraPath::GetNeighborsFunc;
		using NeighborCallback = typename DijkstraPath::NeighborCallback;
		using MapType = std::map<NodeType, DistanceType>;

		DefaultDijkstraPathFinding(const NodeType& start, const GetNeighborsFunc& getNeighbors)
		{
			auto setDistanceIfLower = CreateCallback();
			typename DijkstraPath::Functions f;
			f.getNeighbors = getNeighbors;
			f.setDistanceIfLower = setDistanceIfLower;

			DijkstraPath Dijkstra(start, f);
		}

		DefaultDijkstraPathFinding(const std::vector<NodeType>& start, const GetNeighborsFunc& getNeighbors)
		{
			auto setDistanceIfLower = CreateCallback();
			typename DijkstraPath::Functions f;
			f.getNeighbors = getNeighbors;
			f.setDistanceIfLower = setDistanceIfLower;

			DijkstraPath Dijkstra(start, f);
		}

		const auto& distances() { return _distances; }
	protected:
		auto CreateCallback()
		{
			return [&](const NodeType& node, DistanceType distance) -> bool
			{
				auto r = _distances.insert(std::make_pair(node, distance));
				if (r.second)
					return true;
				auto &old_pair = *(r.first);
				if (old_pair.second < distance)
					return false;

				old_pair.second = distance;
				return true;
			};
		}

		MapType _distances;
	};



}
