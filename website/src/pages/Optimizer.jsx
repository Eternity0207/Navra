import React, { useState } from 'react';
import axios from 'axios';
import { Play, RotateCcw, MapPin } from 'lucide-react';
import GraphVisualization from '../components/GraphVisualization';
import '../styles/optimizer.css';

const Optimizer = () => {
  const [selectedLocations, setSelectedLocations] = useState([]);
  const [routeMode, setRouteMode] = useState(1);
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [search, setSearch] = useState('');

  // Edges data for graph
  const edges = [
    'Main Gate,Admin Block,4',
    'Admin Block,Library,3',
    'Library,CSE Building,2',
    'CSE Building,Dining Hall,3',
    'Dining Hall,Hostel A,3',
    'Hostel A,Main Gate,5',
    'Parking Lot,Admin Block,2',
    'Sports Complex,Parking Lot,3',
    'Sports Complex,Student Activity Center,4',
    'Student Activity Center,Workshop,3',
    'Workshop,Medical Center,2',
    'Medical Center,Library,3',
    'Research Block,CSE Building,4',
    'Research Block,Workshop,5',
    'Innovation Lab,Startup Incubator,3',
    'Startup Incubator,Robotics Bay,4',
    'Robotics Bay,Testing Ground,2',
    'Testing Ground,Control Tower,3',
    'Control Tower,Innovation Lab,4',
  ];

  const locations = [
    'Main Gate', 'Admin Block', 'Library', 'CSE Building', 'Dining Hall',
    'Hostel A', 'Sports Complex', 'Student Activity Center', 'Research Block',
    'Workshop', 'Medical Center', 'Parking Lot', 'Innovation Lab',
    'Startup Incubator', 'Robotics Bay', 'Testing Ground', 'Control Tower', 'Lost Hut'
  ].sort();

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
      const response = await axios.post('http://localhost:5000/api/route', {
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

  return (
    <div className="optimizer-page">
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
            >
              Flexible
            </button>
            <button
              className={`mode-btn ${routeMode === 2 ? 'active' : ''}`}
              onClick={() => setRouteMode(2)}
            >
              Fixed
            </button>
            <button
              className={`mode-btn ${routeMode === 3 ? 'active' : ''}`}
              onClick={() => setRouteMode(3)}
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
            {filteredLocations.map((location) => {
              const isSelected = selectedLocations.includes(location);
              const order = selectedLocations.indexOf(location) + 1;

              return (
                <div
                  key={location}
                  className={`location-item ${isSelected ? 'selected' : ''} ${routeMode === 3 ? 'disabled' : ''}`}
                  onClick={() => handleToggle(location)}
                >
                  <div className="checkbox">
                    {isSelected && '✓'}
                  </div>
                  <span className="location-name">{location}</span>
                  {isSelected && <span className="order-badge">{order}</span>}
                </div>
              );
            })}
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
              disabled={loading || routeMode === 3}
            >
              <RotateCcw size={16} />
            </button>
          </div>
        </div>

        {/* Main Content */}
        <div className="main-area">
          {loading && (
            <div className="status-box">
              <div className="spinner"></div>
              <h3>Computing Route</h3>
              <p>Analyzing optimal path...</p>
            </div>
          )}

          {error && !loading && (
            <div className="status-box error-box">
              <h3>Error</h3>
              <p>{error}</p>
              <button onClick={() => setError(null)}>Dismiss</button>
            </div>
          )}

          {result && !loading && (
            <div className="result-with-graph">
              <div className="graph-container">
                <GraphVisualization routePath={result.routeNames} edges={edges} />
              </div>

              <div className="result-info-panel">
                <div className="result-header">
                  <h3>✓ Route Computed</h3>
                  <p>Optimal path calculated successfully</p>
                </div>

                <div className="result-stats">
                  <div className="stat">
                    <span className="stat-label">Time</span>
                    <span className="stat-value">{formatTime(result.totalTime)}</span>
                  </div>
                  <div className="stat">
                    <span className="stat-label">Stops</span>
                    <span className="stat-value">{result.stopCount}</span>
                  </div>
                  <div className="stat">
                    <span className="stat-label">Algorithm</span>
                    <span className="stat-value">{result.algorithm}</span>
                  </div>
                </div>

                <div className="route-path">
                  <h4>Route Path</h4>
                  <div className="path-steps">
                    {result.routeNames.map((name, idx) => (
                      <div key={idx} className="path-step">
                        <div className="step-number">{idx + 1}</div>
                        <div className="step-name">{name}</div>
                        {idx === 0 && <span className="step-tag start">Start</span>}
                        {idx === result.routeNames.length - 1 && <span className="step-tag end">End</span>}
                      </div>
                    ))}
                  </div>
                </div>
              </div>
            </div>
          )}

          {!loading && !error && !result && (
            <div className="empty-state">
              <GraphVisualization routePath={[]} edges={edges} />
              <div className="empty-overlay">
                <MapPin size={64} strokeWidth={1} />
                <h3>{routeMode === 3 ? 'Full Campus Mode' : 'Select Locations'}</h3>
                <p>
                  {routeMode === 3
                    ? 'Click Compute to traverse entire campus'
                    : 'Choose locations and click Compute to find optimal route'}
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