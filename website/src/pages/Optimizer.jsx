import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { Play, RotateCcw, MapPin, ArrowRight } from 'lucide-react';
import { useTheme } from '../context/ThemeContext';
import GraphVisualization from '../components/GraphVisualization';
import '../styles/optimizer.css';

const Optimizer = () => {
  const { isDark } = useTheme();
  const [selectedLocations, setSelectedLocations] = useState([]);
  const [routeMode, setRouteMode] = useState(1);
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [search, setSearch] = useState('');
  const [graphData, setGraphData] = useState(null);
  const [locations, setLocations] = useState([]);

  // Load CSV data on component mount
  useEffect(() => {
    const loadCSVData = async () => {
      try {
        const attractionsResponse = await fetch('/src/assets/attractions.csv');
        const attractionsText = await attractionsResponse.text();
        
        const roadsResponse = await fetch('/src/assets/roads.csv');
        const roadsText = await roadsResponse.text();

        const attractionLines = attractionsText.trim().split('\n');
        const attractionData = attractionLines.slice(1)
          .map(line => {
            const [name, category, rating, duration, fee, popularity, latitude, longitude] = 
              line.split(',').map(item => item.trim());
            return {
              name,
              category,
              rating: parseFloat(rating),
              duration: parseInt(duration),
              fee: parseInt(fee),
              popularity: parseInt(popularity),
              latitude: parseFloat(latitude),
              longitude: parseFloat(longitude)
            };
          });

        const roadLines = roadsText.trim().split('\n');
        const roadData = roadLines.slice(1)
          .filter(line => !line.startsWith('#') && line.trim())
          .map(line => {
            const [from, to, time] = line.split(',').map(item => item.trim());
            return { from, to, time: parseInt(time) };
          });

        const nodeSet = new Set();
        const links = roadData.map(road => {
          nodeSet.add(road.from);
          nodeSet.add(road.to);
          return {
            source: road.from,
            target: road.to,
            value: road.time
          };
        });

        const bidirectionalLinks = [...links];
        links.forEach(link => {
          bidirectionalLinks.push({
            source: link.target,
            target: link.source,
            value: link.value
          });
        });

        const nodes = Array.from(nodeSet).map(name => {
          const attraction = attractionData.find(a => a.name === name);
          return {
            id: name,
            name: name,
            val: 12,
            ...attraction
          };
        });

        setGraphData({ nodes, links: bidirectionalLinks });
        setLocations(attractionData.map(a => a.name).sort());
      } catch (err) {
        console.error('Error loading CSV data:', err);
        setError('Failed to load graph data from CSV files');
      }
    };

    loadCSVData();
  }, []);

  const filteredLocations = locations.filter(loc =>
    loc.toLowerCase().includes(search.toLowerCase())
  );

  const handleToggle = (location) => {
    if (routeMode === 3) return;

    setSelectedLocations(prev =>
      prev.includes(location)
        ? prev.filter(l => l !== location)
        : [...prev, location]
    );
  };

  const handleCompute = async () => {
    if (routeMode !== 3 && selectedLocations.length === 0) {
      setError('Please select at least one location');
      return;
    }

    setLoading(true);
    setError(null);
    setResult(null);

    try {
      const response = await axios.post('https://navra-rxsw.vercel.app/api/route', {
        choice: routeMode,
        count: routeMode === 3 ? 0 : selectedLocations.length,
        locations: routeMode === 3 ? [] : selectedLocations
      });

      if (response.data.success) {
        setResult(response.data);
      } else {
        setError(response.data.error || 'Failed to compute route');
      }
    } catch (err) {
      setError('Backend connection failed. Make sure server is running on port 5000.');
    } finally {
      setLoading(false);
    }
  };

  const handleClear = () => {
    setSelectedLocations([]);
    setResult(null);
    setError(null);
  };

  const formatTime = (mins) => {
    if (mins < 60) return `${Math.round(mins)} min`;
    const h = Math.floor(mins / 60);
    const m = Math.round(mins % 60);
    return `${h}h ${m}m`;
  };

  const isRequestedStop = (name) => {
    return result?.routeNames && result.routeNames.includes(name);
  };

  const pathToDisplay = result?.fullPathNames && result.fullPathNames.length > 0 
    ? result.fullPathNames 
    : result?.routeNames || [];

  return (
    <div className={`optimizer-page ${isDark ? 'dark' : 'light'}`}>
      <div className="optimizer-container">
        {/* Sidebar */}
        <div className="sidebar">
          <div className="sidebar-header">
            <h2>Route Optimizer</h2>
            <span className="badge">{routeMode === 3 ? 'All' : selectedLocations.length}</span>
          </div>

          {/* Mode Selector */}
          <div className="mode-selector">
            <button
              className={`mode-btn ${routeMode === 1 ? 'active' : ''}`}
              onClick={() => setRouteMode(1)}
              title="Flexible Order - TSP Algorithm"
            >
              Flexible
            </button>
            <button
              className={`mode-btn ${routeMode === 2 ? 'active' : ''}`}
              onClick={() => setRouteMode(2)}
              title="Fixed Order - Dijkstra's Algorithm"
            >
              Fixed
            </button>
            <button
              className={`mode-btn ${routeMode === 3 ? 'active' : ''}`}
              onClick={() => setRouteMode(3)}
              title="Full Campus - MST + DFS + A*"
            >
              Full
            </button>
          </div>

          {/* Search */}
          <div className="search-box">
            <input
              type="text"
              placeholder="Search locations..."
              value={search}
              onChange={(e) => setSearch(e.target.value)}
              disabled={routeMode === 3}
            />
          </div>

          {/* Locations List */}
          <div className="locations-list">
            {routeMode === 3 ? (
              <div className="full-mode-message">
                <MapPin size={48} />
                <h3>Full Campus Mode</h3>
                <p>Click "Compute" to traverse all locations using MST algorithm.</p>
              </div>
            ) : (
              filteredLocations.map((location) => {
                const isSelected = selectedLocations.includes(location);
                const order = selectedLocations.indexOf(location) + 1;

                return (
                  <div
                    key={location}
                    className={`location-item ${isSelected ? 'selected' : ''}`}
                    onClick={() => handleToggle(location)}
                  >
                    <div className="checkbox">
                      {isSelected && '✓'}
                    </div>
                    <span className="location-name">{location}</span>
                    {isSelected && <span className="order-badge">{order}</span>}
                  </div>
                );
              })
            )}
          </div>

          {/* Actions */}
          <div className="sidebar-actions">
            <button
              className="btn-compute"
              onClick={handleCompute}
              disabled={loading || (routeMode !== 3 && selectedLocations.length === 0)}
            >
              <Play size={18} />
              Compute
            </button>
            <button
              className="btn-clear"
              onClick={handleClear}
              disabled={loading}
            >
              <RotateCcw size={16} />
            </button>
          </div>
        </div>

        {/* Main Content */}
        <div className="main-area">
          {loading && (
            <div className="status-overlay">
              <div className="status-box">
                <div className="spinner"></div>
                <h3>Computing Route</h3>
                <p>Analyzing optimal path...</p>
              </div>
            </div>
          )}

          {error && !loading && (
            <div className="status-overlay">
              <div className="status-box error-box">
                <div className="error-icon">⚠️</div>
                <h3>Error</h3>
                <p>{error}</p>
                <button onClick={() => setError(null)}>Dismiss</button>
              </div>
            </div>
          )}

          {result && !loading && !error ? (
            <div className="result-layout">
              {/* Graph Visualization - Better Height */}
              <div className="graph-section">
                <GraphVisualization 
                  routePath={result.fullPathNames || result.routeNames} 
                  graphData={graphData} 
                />
              </div>

              {/* Result Panel */}
              <div className="result-panel">
                {/* Stats Header */}
                <div className="result-header">
                  <div className="success-badge">
                    <span className="success-icon">✓</span>
                    <div>
                      <h3>Route Computed</h3>
                      <p>{result.algorithm}</p>
                    </div>
                  </div>
                  
                  <div className="stats-grid">
                    <div className="stat-item">
                      <span className="stat-label">Time</span>
                      <span className="stat-value">{formatTime(result.totalTime)}</span>
                    </div>
                    <div className="stat-item">
                      <span className="stat-label">Stops</span>
                      <span className="stat-value">{result.stopCount}</span>
                    </div>
                    <div className="stat-item">
                      <span className="stat-label">Total Nodes</span>
                      <span className="stat-value">{pathToDisplay.length}</span>
                    </div>
                  </div>
                </div>

                {/* Path Display */}
                <div className="result-body">
                  <div className="path-header">
                    <h4>Complete Route Path</h4>
                    <span className="path-count">{pathToDisplay.length} nodes</span>
                  </div>
                  
                  <div className="path-horizontal-scroll">
                    <div className="path-horizontal">
                      {pathToDisplay.map((name, idx) => {
                        const isRequested = isRequestedStop(name);
                        const isStart = idx === 0;
                        const isEnd = idx === pathToDisplay.length - 1;

                        return (
                          <React.Fragment key={`${idx}-${name}`}>
                            <div className={`path-node ${isRequested ? 'requested' : 'intermediate'}`}>
                              <div className={`node-circle ${isRequested ? 'requested' : ''}`}>
                                {idx + 1}
                              </div>
                              <div className="node-info">
                                <span className="node-name">{name}</span>
                                <div className="node-tags">
                                  {isStart && <span className="node-tag start">Start</span>}
                                  {isEnd && <span className="node-tag end">End</span>}
                                  {isRequested && !isStart && !isEnd && (
                                    <span className="node-tag destination">Destination</span>
                                  )}
                                  {!isRequested && <span className="node-tag via">Via</span>}
                                </div>
                              </div>
                            </div>
                            
                            {idx < pathToDisplay.length - 1 && (
                              <div className="path-arrow">
                                <ArrowRight size={20} strokeWidth={2.5} />
                              </div>
                            )}
                          </React.Fragment>
                        );
                      })}
                    </div>
                  </div>
                </div>
              </div>
            </div>
          ) : !loading && !error && (
            <div className="empty-state">
              <GraphVisualization routePath={[]} graphData={graphData} />
              <div className="empty-overlay">
                <MapPin size={64} strokeWidth={1.5} />
                <h3>Ready to Optimize</h3>
                <p>
                  {routeMode === 3 
                    ? 'Click "Compute" to traverse the entire campus' 
                    : 'Select locations and click "Compute"'}
                </p>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default Optimizer;
