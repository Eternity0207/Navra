import React from 'react';
import { motion } from 'framer-motion';
import { Code, Layers, Zap, Github } from 'lucide-react';
import '../styles/about.css';

const About = () => {
  return (
    <div className="about">
      <div className="about-header">
        <h1>About NAVRA</h1>
        <p>Campus route optimization for IIT Jodhpur</p>
      </div>

      <div className="about-content">
        <motion.section
          className="about-section"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h2>Project Overview</h2>
          <p>
            NAVRA is a comprehensive campus navigation system for IIT Jodhpur that demonstrates
            practical applications of graph theory and algorithmic optimization. The system
            integrates multiple pathfinding algorithms including Dijkstra, A*, TSP, and Kruskal's
            MST to provide optimal routes across 18 campus locations.
          </p>
        </motion.section>

        <div className="tech-grid">
          {[
            { 
              icon: Code, 
              title: 'Backend', 
              items: ['C++17', 'Graph Algorithms', 'JSON Processing', 'CSV Data Loading'] 
            },
            { 
              icon: Layers, 
              title: 'Server', 
              items: ['Node.js', 'Express', 'CORS', 'Process Spawning'] 
            },
            { 
              icon: Zap, 
              title: 'Frontend', 
              items: ['React 18', 'Vite', 'Framer Motion', 'Axios'] 
            }
          ].map((tech, idx) => (
            <motion.div
              key={idx}
              className="tech-card"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true }}
              transition={{ delay: idx * 0.1 }}
            >
              <tech.icon size={32} />
              <h3>{tech.title}</h3>
              <ul>
                {tech.items.map((item, i) => (
                  <li key={i}>{item}</li>
                ))}
              </ul>
            </motion.div>
          ))}
        </div>

        <motion.section
          className="features-section"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h2>Key Features</h2>
          <div className="features-list">
            <div className="feature-item">
              <h3>Intelligent Algorithm Selection</h3>
              <p>Automatically chooses the most efficient algorithm based on routing mode and number of locations.</p>
            </div>
            <div className="feature-item">
              <h3>Three Routing Modes</h3>
              <p>Flexible order (TSP), fixed order (Dijkstra/A*), and full campus traversal (MST + DFS).</p>
            </div>
            <div className="feature-item">
              <h3>Real Campus Data</h3>
              <p>Uses actual IIT Jodhpur locations with 18 points of interest and road connections.</p>
            </div>
            <div className="feature-item">
              <h3>Connectivity Validation</h3>
              <p>DSU-based validation ensures all selected locations are reachable before computation.</p>
            </div>
          </div>
        </motion.section>

        <motion.div
          className="cta-section"
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          viewport={{ once: true }}
        >
          <Github size={48} />
          <h2>Open Source Project</h2>
          <p>This project is available for educational purposes</p>
          <a href="https://github.com" className="github-btn" target="_blank" rel="noopener noreferrer">
            View on GitHub
          </a>
        </motion.div>
      </div>
    </div>
  );
};

export default About;
