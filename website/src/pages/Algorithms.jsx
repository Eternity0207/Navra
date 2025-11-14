import React from 'react';
import { motion } from 'framer-motion';
import { Cpu, Zap, Target, GitBranch, Network } from 'lucide-react';
import '../styles/algorithms.css';

const Algorithms = () => {
  const algos = [
    {
      icon: Zap,
      name: "Dijkstra's Algorithm",
      complexity: 'O((V + E) log V)',
      use: 'Single-source shortest path',
      description: 'Used for 2-3 locations with fixed order. Guarantees optimal solution for non-negative weighted graphs.',
      features: ['Priority queue implementation', 'Optimal for sparse graphs', 'Works with non-negative weights']
    },
    {
      icon: Target,
      name: 'A* Search',
      complexity: 'O(E)',
      use: 'Heuristic pathfinding',
      description: 'Point-to-point routing using Euclidean distance heuristic. Faster than Dijkstra for 2 locations.',
      features: ['Euclidean distance heuristic', 'Optimal and complete', 'Best for 2-location queries']
    },
    {
      icon: GitBranch,
      name: 'TSP (Traveling Salesman)',
      complexity: 'O(2^n × n^2)',
      use: 'Multi-location tour',
      description: 'Bitmask dynamic programming for optimal visiting order. Used in flexible mode for multiple locations.',
      features: ['Bitmask DP optimization', 'Optimal tour calculation', 'Handles 15-20 locations']
    },
    {
      icon: Network,
      name: "Kruskal's MST",
      complexity: 'O(E log E)',
      use: 'Minimum spanning tree',
      description: 'Validates connectivity and builds MST using Union-Find. Used for full campus traversal mode.',
      features: ['Union-Find (DSU)', 'Edge sorting', 'Connectivity validation']
    }
  ];

  return (
    <div className="algorithms">
      <div className="algorithms-header">
        <h1>Graph Algorithms</h1>
        <p>Understanding the algorithms powering NAVRA</p>
      </div>

      <div className="mode-explanation">
        <h2>Algorithm Selection Strategy</h2>
        <div className="mode-grid">
          <div className="mode-card">
            <h3>Mode 1: Flexible Order</h3>
            <p>System optimizes visiting sequence using TSP with bitmask DP for minimum total time.</p>
          </div>
          <div className="mode-card">
            <h3>Mode 2: Fixed Order</h3>
            <p>Uses Dijkstra (3+ locations) or A* (2 locations) to follow your selected sequence.</p>
          </div>
          <div className="mode-card">
            <h3>Mode 3: Full Campus</h3>
            <p>MST + DFS traversal + A* refinement to visit all reachable campus locations.</p>
          </div>
        </div>
      </div>

      <div className="algorithms-grid">
        {algos.map((algo, idx) => (
          <motion.div
            key={idx}
            className="algo-card"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: idx * 0.1 }}
            whileHover={{ y: -5 }}
          >
            <div className="algo-icon">
              <algo.icon size={28} />
            </div>
            <h3>{algo.name}</h3>
            <div className="algo-meta">
              <span className="complexity">{algo.complexity}</span>
            </div>
            <p className="algo-use">{algo.use}</p>
            <p className="algo-description">{algo.description}</p>
            <ul className="algo-features">
              {algo.features.map((feature, i) => (
                <li key={i}>{feature}</li>
              ))}
            </ul>
          </motion.div>
        ))}
      </div>
    </div>
  );
};

export default Algorithms;
