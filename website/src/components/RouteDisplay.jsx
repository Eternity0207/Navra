import React from 'react';
import { CheckCircle2, Clock, MapPin, Cpu } from 'lucide-react';
import '../styles/route-display.css';

const RouteDisplay = ({ result }) => {
  const formatTime = (mins) => {
    if (mins < 60) return `${Math.round(mins)} min`;
    const h = Math.floor(mins / 60);
    const m = Math.round(mins % 60);
    return `${h}h ${m}m`;
  };

  return (
    <div className="route-display">
      <div className="route-header">
        <CheckCircle2 size={24} />
        <div>
          <h3>Route Computed</h3>
          <p>Optimal path calculated</p>
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
            <span className="label">Stops</span>
            <span className="value">{result.stopCount}</span>
          </div>
        </div>

        <div className="stat">
          <Cpu size={20} />
          <div>
            <span className="label">Algorithm</span>
            <span className="value algo">{result.algorithm}</span>
          </div>
        </div>
      </div>

      <div className="route-path">
        <h4>Route Path</h4>
        <div className="path-scroll">
          {result.routeNames.map((name, idx) => (
            <div key={idx} className="path-step">
              <div className="step-num">{idx + 1}</div>
              <div className="step-info">
                <span className="step-name">{name}</span>
                {idx === 0 && <span className="tag start">Start</span>}
                {idx === result.routeNames.length - 1 && <span className="tag end">End</span>}
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default RouteDisplay;
