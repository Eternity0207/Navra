import React from 'react';
import { CheckCircle2, Clock, MapPin, Cpu, Navigation } from 'lucide-react';
import '../styles/route-display.css';

const RouteDisplay = ({ result }) => {
  const formatTime = (mins) => {
    if (mins < 60) return `${Math.round(mins)} min`;
    const h = Math.floor(mins / 60);
    const m = Math.round(mins % 60);
    return `${h}h ${m}m`;
  };

  // Use fullPathNames if available, otherwise fall back to routeNames
  const pathToDisplay = result.fullPathNames && result.fullPathNames.length > 0 
    ? result.fullPathNames 
    : result.routeNames || [];

  // Check if a location is a requested stop (in routeNames)
  const isRequestedStop = (name) => {
    return result.routeNames && result.routeNames.includes(name);
  };

  return (
    <div className="route-display">
      <div className="route-header">
        <CheckCircle2 size={24} />
        <div>
          <h3>Route Computed</h3>
          <p>Optimal path with all nodes</p>
        </div>
      </div>

      <div className="route-stats">
        <div className="stat">
          <Clock size={20} />
          <div>
            <span className="label">Time</span>
            <span className="value">{formatTime(result.totalTime)}</span>
          </div>
        </div>

        <div className="stat">
          <MapPin size={20} />
          <div>
            <span className="label">Destinations</span>
            <span className="value">{result.stopCount}</span>
          </div>
        </div>

        <div className="stat">
          <Navigation size={20} />
          <div>
            <span className="label">Total Nodes</span>
            <span className="value">{pathToDisplay.length}</span>
          </div>
        </div>
      </div>

      <div className="route-stats-secondary">
        <div className="stat-algorithm">
          <Cpu size={20} />
          <div>
            <span className="label">Algorithm</span>
            <span className="value algo">{result.algorithm}</span>
          </div>
        </div>
      </div>

      <div className="route-path">
        <h4>Complete Route Path</h4>
        <div className="path-scroll">
          {pathToDisplay.map((name, idx) => {
            const isRequested = isRequestedStop(name);
            const isStart = idx === 0;
            const isEnd = idx === pathToDisplay.length - 1;

            return (
              <div 
                key={`${idx}-${name}`}
                className={`path-step ${isRequested ? 'requested' : 'intermediate'}`}
              >
                <div className={`step-num ${isRequested ? 'requested' : ''}`}>
                  {idx + 1}
                </div>
                <div className="step-info">
                  <span className="step-name">{name}</span>
                  <div className="tags">
                    {isStart && <span className="tag start">Start</span>}
                    {isEnd && <span className="tag end">End</span>}
                    {isRequested && !isStart && !isEnd && (
                      <span className="tag destination">Destination</span>
                    )}
                    {!isRequested && (
                      <span className="tag intermediate-tag">Via</span>
                    )}
                  </div>
                </div>
              </div>
            );
          })}
        </div>
      </div>
    </div>
  );
};

export default RouteDisplay;
