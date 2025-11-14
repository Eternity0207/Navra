import React from 'react';
import { Link } from 'react-router-dom';
import { ArrowRight, Zap, Route, Navigation, Cpu, Map, Target, Globe } from 'lucide-react';
import { motion } from 'framer-motion';
import '../styles/home.css';

const Home = () => {
  const features = [
    {
      icon: Route,
      title: 'Optimal Path Finding',
      desc: 'Find shortest routes using Dijkstra and A* algorithms'
    },
    {
      icon: Cpu,
      title: 'TSP Optimization',
      desc: 'Dynamic programming with bitmask for multi-location tours'
    },
    {
      icon: Navigation,
      title: 'Flexible Routes',
      desc: 'Fixed or optimized order based on your preference'
    },
    {
      icon: Globe,
      title: 'Full Campus Coverage',
      desc: 'MST + DFS traversal for complete campus exploration'
    }
  ];

  return (
    <div className="home">
      <section className="hero">
        <motion.div
          className="hero-content"
          initial={{ opacity: 0, y: 30 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6 }}
        >
          <motion.div
            className="hero-badge"
            initial={{ opacity: 0, scale: 0.8 }}
            animate={{ opacity: 1, scale: 1 }}
            transition={{ delay: 0.2 }}
          >
            <Zap size={16} />
            <span>Powered by Graph Algorithms</span>
          </motion.div>

          <h1 className="hero-title">
            Campus Route
            <span className="gradient-text"> Optimization</span>
          </h1>

          <p className="hero-subtitle">
            Navigate IIT Jodhpur campus efficiently with intelligent pathfinding
            using Dijkstra, A*, TSP, and Kruskal's MST algorithms.
          </p>

          <div className="hero-actions">
            <Link to="/optimizer" className="btn-primary">
              Start Optimizing
              <ArrowRight size={20} />
            </Link>
            <Link to="/algorithms" className="btn-secondary">
              Learn More
            </Link>
          </div>

          <div className="hero-stats">
            <div className="stat">
              <Target className="stat-icon" />
              <div>
                <div className="stat-number">4</div>
                <div className="stat-label">Algorithms</div>
              </div>
            </div>
            <div className="stat">
              <Zap className="stat-icon" />
              <div>
                <div className="stat-number">18</div>
                <div className="stat-label">Locations</div>
              </div>
            </div>
            <div className="stat">
              <Route className="stat-icon" />
              <div>
                <div className="stat-number">3</div>
                <div className="stat-label">Modes</div>
              </div>
            </div>
          </div>
        </motion.div>
      </section>

      <section className="features">
        <div className="section-header">
          <h2>Key Features</h2>
          <p>Everything you need for optimal campus navigation</p>
        </div>

        <div className="features-grid">
          {features.map((feature, idx) => (
            <motion.div
              key={idx}
              className="feature-card"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true }}
              transition={{ delay: idx * 0.1 }}
              whileHover={{ y: -5 }}
            >
              <div className="feature-icon">
                <feature.icon size={24} />
              </div>
              <h3>{feature.title}</h3>
              <p>{feature.desc}</p>
            </motion.div>
          ))}
        </div>
      </section>
    </div>
  );
};

export default Home;
