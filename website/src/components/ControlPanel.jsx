import React, { useState } from 'react';
import { Play, RotateCcw, MapPin } from 'lucide-react';
import LocationSelector from '../components/LocationSelector';
import RouteDisplay from '../components/RouteDisplay';
import LoadingSpinner from '../components/LoadingSpinner';
import ErrorDisplay from '../components/ErrorDisplay';
import ControlPanel from '../components/ControlPanel';
import axios from 'axios';
import '../styles/optimizer.css';

const Optimizer = () => {
  const [selectedLocations, setSelectedLocations] = useState([]);
  const [routeMode, setRouteMode] = useState(1);
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleToggle = (location) => {
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
      const payload = {
        choice: routeMode,
        count: routeMode === 3 ? 0 : selectedLocations.length,
        locations: routeMode === 3 ? [] : selectedLocations
      };

      const response = await axios.post('https://navra-rxsw.vercel.app/api/route', payload);

      if (response.data.success) {
        setResult(response.data);
      } else {
        setError(response.data.error || 'Failed to compute route');
      }
    } catch (err) {
      setError(err.response?.data?.error || 'Backend connection failed');
    } finally {
      setLoading(false);
    }
  };

  const handleClear = () => {
    setSelectedLocations([]);
    setResult(null);
    setError(null);
  };

  const isFullCampusMode = routeMode === 3;

  return (
    <div className="optimizer-page">
      <div className="optimizer-wrapper">
        {/* Sidebar */}
        <aside className="optimizer-sidebar">
          <div className="sidebar-head">
            <h2>Route Optimizer</h2>
            <span className="badge">{isFullCampusMode ? 'All' : selectedLocations.length}</span>
          </div>

          <ControlPanel routeMode={routeMode} setRouteMode={setRouteMode} />

          <LocationSelector
            selectedLocations={selectedLocations}
            onLocationToggle={handleToggle}
            disabled={isFullCampusMode}
          />

          <div className="sidebar-footer">
            <button
              className="btn btn-primary"
              onClick={handleCompute}
              disabled={loading || (!isFullCampusMode && selectedLocations.length === 0)}
            >
              <Play size={18} />
              <span>Compute</span>
            </button>
            
            <button
              className="btn btn-ghost"
              onClick={handleClear}
              disabled={loading || isFullCampusMode}
            >
              <RotateCcw size={16} />
            </button>
          </div>
        </aside>

        {/* Main Content */}
        <main className="optimizer-main">
          {loading && <LoadingSpinner />}
          
          {error && !loading && (
            <ErrorDisplay message={error} onDismiss={() => setError(null)} />
          )}
          
          {result && !loading && <RouteDisplay result={result} />}
          
          {!loading && !error && !result && (
            <div className="empty-placeholder">
              <div className="empty-icon">
                <MapPin size={56} strokeWidth={1.5} />
              </div>
              <h3>{isFullCampusMode ? 'Full Campus Mode' : 'Select Locations'}</h3>
              <p>
                {isFullCampusMode 
                  ? 'Click Compute to traverse entire campus using MST + DFS + A*'
                  : 'Choose locations from sidebar and click Compute'}
              </p>
            </div>
          )}
        </main>
      </div>
    </div>
  );
};

export default Optimizer;
