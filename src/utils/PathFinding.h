#pragma once
#include <map>
#include <queue>
#include <functional>


namespace MX
{
#ifdef _DEBUG
	template <class T, typename T2, typename T3>
	class reservable_priority_queue: public std::priority_queue<T, T2, T3>
	{
	public:
		typedef typename std::priority_queue<T>::size_type size_type;
		reservable_priority_queue(size_type capacity = 0) { reserve(capacity); };
		void reserve(size_type capacity) { this->c.reserve(capacity); } 
		size_type capacity() const { return this->c.capacity(); } 
		void clear() { this->c.clear(); }
	};
#endif

	template<typename NodeType>
	class DijkstraPathFinding
	{
	public:
		using DistanceType = float;

		using NeighborCallback = std::function<void(const NodeType& node, DistanceType distance)>;
		//using GetNeighborsFunc = std::function<void(const NodeType& node, const NeighborCallback& callback)>;
		//using SetDistanceIfLowerFunc = std::function<bool(const NodeType& node, DistanceType distance)>;

		using QueuePair = std::pair<DistanceType, NodeType>;

#ifdef _DEBUG
		using PriorityQueue = reservable_priority_queue<QueuePair, std::vector<QueuePair>, std::greater<QueuePair>>;
#else
		using PriorityQueue = std::priority_queue<QueuePair, std::vector<QueuePair>, std::greater<QueuePair>>;
#endif

		template<typename GetNeighborsType, typename SetDistanceIfLowerType>
		DijkstraPathFinding(const std::vector<NodeType>& startNodes, GetNeighborsType&& getNeighbors, SetDistanceIfLowerType&& setDistanceIfLower )
		{
			PriorityQueue queue;
#ifdef _DEBUG
			queue.clear();
			queue.reserve(100);
#endif

			DistanceType startDistance = 0.0f;
			for (auto& start : startNodes)
			{
				queue.push(std::make_pair(startDistance, start));
				setDistanceIfLower(start, startDistance);
			}

			PathFind(queue, getNeighbors, setDistanceIfLower);
		}

		template<typename GetNeighborsType, typename SetDistanceIfLowerType>
		DijkstraPathFinding(const NodeType& start, GetNeighborsType&& getNeighbors, SetDistanceIfLowerType&& setDistanceIfLower )
		{
			PriorityQueue queue;
#ifdef _DEBUG
			queue.clear();
			queue.reserve(100);
#endif

			DistanceType startDistance = 0.0f;
			queue.push(std::make_pair(startDistance, start));
			setDistanceIfLower(start, startDistance);

			PathFind(queue, getNeighbors, setDistanceIfLower);
		}

		DijkstraPathFinding(DijkstraPathFinding&& other) = default;

	protected:
		template<typename GetNeighborsType, typename SetDistanceIfLowerType>
		void PathFind(PriorityQueue& queue, GetNeighborsType&& getNeighbors, SetDistanceIfLowerType&& setDistanceIfLower)
		{
			while (!queue.empty())
			{
				auto p = queue.top();
				queue.pop();

				DistanceType current = p.first;


				auto forN = [&](const NodeType& node, DistanceType distance)
				{
					auto newDistance = current + distance;
					if (!setDistanceIfLower(node, newDistance))
						return;
					queue.push(std::make_pair(newDistance, node));

				};
				getNeighbors(current, p.second, forN);
			}
		}
	};

	template<typename NodeType>
	class DefaultDijkstraPathFinding
	{
	public:
		using DijkstraPath = DijkstraPathFinding<NodeType>;
		using DistanceType = typename DijkstraPath::DistanceType;
		//using NeighborCallback = typename DijkstraPath::NeighborCallback;
		using MapType = std::map<NodeType, DistanceType>;

		template<typename GetNeighborsFunc>
		DefaultDijkstraPathFinding(const NodeType& start, GetNeighborsFunc&& getNeighbors)
		{
			auto setDistanceIfLower = CreateCallback();
			DijkstraPath Dijkstra(start, getNeighbors, setDistanceIfLower);
		}

		template<typename GetNeighborsFunc>
		DefaultDijkstraPathFinding(const std::vector<NodeType>& start, GetNeighborsFunc&& getNeighbors)
		{
			auto setDistanceIfLower = CreateCallback();
			DijkstraPath Dijkstra(start, getNeighbors, setDistanceIfLower);
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
				if (old_pair.second <= distance)
					return false;

				old_pair.second = distance;
				return true;
			};
		}

		MapType _distances;
	};



}
